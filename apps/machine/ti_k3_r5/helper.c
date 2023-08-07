/*
 * Copyright (C) 2023 Texas Instruments Incorporated - https://www.ti.com/
 *	Andrew Davis <afd@ti.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/open_amp.h>

#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/alloc.h>

#include "helper.h"
#include "platform_info.h"

#include "r5/kernel/dpl/HwiP.h"
#include "r5/kernel/dpl/CacheP.h"
#include "r5/kernel/dpl/MpuP_armv7.h"

/* Place debug trace buffer in special ELF section */
#define __section_t(S) __attribute__((__section__(#S)))
#define __log_shared __section_t(.log_shared_mem)

/* ----------- HwiP ----------- */
#ifndef RPMSG_NO_IPI
HwiP_Config gHwiConfig = {
	.intcBaseAddr = INT_BASE_ADDR,
};
#endif

// global structures used by MPU and cache init code
CacheP_Config gCacheConfig = { 1, 0 }; // cache on, no forced writethrough
MpuP_Config gMpuConfig = { 3, 1, 1 }; // 2 regions, background region on, MPU on
MpuP_RegionConfig gMpuRegionConfig[] =
{
	// DDR region
	{
		.baseAddr = DDR_BASE_ADDR,
		.size = MpuP_RegionSize_2G,
		.attrs = {
			.isEnable = 1,
			.isCacheable = 1,
			.isBufferable = 1,
			.isSharable = 0,
			.isExecuteNever = 0,
			.tex = 7,
			.accessPerm = MpuP_AP_ALL_RW,
			.subregionDisableMask = 0x0u,
		},
	},
	// rpmsg region
	{
		.baseAddr = RPMSG_BASE_ADDR,
		.size = MpuP_RegionSize_1M,
		.attrs = {
			.isEnable = 1,
			.isCacheable = 0,
			.isBufferable = 0,
			.isSharable = 1,
			.isExecuteNever = 1,
			.tex = 1,
			.accessPerm = MpuP_AP_ALL_RW,
			.subregionDisableMask = 0x0u,
		},
	},

	// resource table region
	{
		.baseAddr = RSC_TABLE_BASE_ADDR,
		.size = MpuP_RegionSize_4K,
		.attrs = {
			.isEnable = 1,
			.isCacheable = 0,
			.isBufferable = 0,
			.isSharable = 1,
			.isExecuteNever = 1,
			.tex = 1,
			.accessPerm = MpuP_AP_ALL_RW,
			.subregionDisableMask = 0x0u,
		},
	},
};

// NOTE: R5FSS defaults to ARM at reset so these must all be ARM instead of Thumb

void Reset_Handler(void) __attribute__((naked, section(".boot.reset"), target("arm")));
void Default_Handler(void) __attribute__((naked, section(".boot.handler"), target("arm")));

void Undef_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PAbt_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DAbt_Handler(void) __attribute__((weak, alias("Default_Handler")));
void IRQ_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FIQ_Handler(void) __attribute__((weak, alias("Default_Handler")));

__attribute__((naked, section(".isr_vector"), target("arm"))) void vectors()
{
	asm volatile(
		"LDR PC, =Reset_Handler \n"
		"LDR PC, =Undef_Handler \n"
		"LDR PC, =SVC_Handler   \n"
		"LDR PC, =PAbt_Handler  \n"
		"LDR PC, =DAbt_Handler  \n"
		"NOP                    \n"
		"LDR PC, =IRQ_Handler   \n"
		"LDR PC, =FIQ_Handler   \n");
}

// newlib startup code
extern void _start();

void Reset_Handler()
{
	asm volatile(
		// initialize stack
		"ldr sp, =__stack \n"

		// disable interrupts
		"mrs r0, cpsr \n"
		"orr r0, r0, #0xc0 \n"
		"msr cpsr_cf, r0 \n");

	// must initialize MPU if code is on external memory
	MpuP_init();
	CacheP_init();

	_start();
}

void Default_Handler()
{
	while (1)
		;
}

char __log_shared debug_log_memory[DEBUG_LOG_SIZE];

extern void CacheP_wb(void *addr, uint32_t size, uint32_t type);

// retarget stdout to remoteproc trace buffer
int _write(int handle, char *data, int size)
{
	static size_t idx = 0;
	int count;

	metal_unused(handle);

	for (count = 0; count < size; count++) {
		if (idx > DEBUG_LOG_SIZE)
			idx = 0;

		debug_log_memory[idx++] = data[count];
	}

	/* null terminate end of trace buffer */
	if (idx > DEBUG_LOG_SIZE)
		idx = 0;
	debug_log_memory[idx] = '\0';

	CacheP_wb(debug_log_memory, DEBUG_LOG_SIZE, 0);

	return count;
}
