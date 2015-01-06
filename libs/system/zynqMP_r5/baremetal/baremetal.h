/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the <ORGANIZATION> nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BAREMETAL_H
#define _BAREMETAL_H

#include "xil_cache.h"
#include "xreg_cortexr5.h"
#include "xpseudo_asm_gcc.h"

#define         MEM_READ8(addr)         *(volatile unsigned char *)(addr)
#define         MEM_READ16(addr)        *(volatile unsigned short *)(addr)
#define         MEM_READ32(addr)        *(volatile unsigned long *)(addr)
#define         MEM_WRITE8(addr,data)   *(volatile unsigned char *)(addr) = (unsigned char)(data)
#define         MEM_WRITE16(addr,data)  *(volatile unsigned short *)(addr) = (unsigned short)(data)
#define         MEM_WRITE32(addr,data)  *(volatile unsigned long *)(addr) = (unsigned long)(data)

#ifndef BAREMETAL_MASTER
#define BAREMETAL_MASTER 0
#endif

/* Define Interrupt Ack Mask */
#define         INT_ACK_MASK                    0x000003FF

/* The vector table address is the same as image entry point */
#define RAM_VECTOR_TABLE_ADDR           ELF_START

typedef enum {
	TRIG_NOT_SUPPORTED,
	TRIG_RISING_EDGE,
	TRIG_FALLING_EDGE,
	TRIG_LEVEL_LOW,
	TRIG_LEVEL_HIGH,
	TRIG_RISING_FALLING_EDGES,
	TRIG_HIGH_LOW_RISING_FALLING_EDGES
} INT_TRIG_TYPE;

typedef enum {
    NOCACHE,
    WRITEBACK,
    WRITETHROUGH
} CACHE_TYPE;

#define CORTEXR5_CPSR_INTERRUPTS_BITS (XREG_CPSR_IRQ_ENABLE | XREG_CPSR_FIQ_ENABLE)

/* This macro writes the current program status register (CPSR - all fields) */
#define ARM_AR_CPSR_CXSF_WRITE(cpsr_cxsf_value) \
	{ \
		asm volatile("    MSR     CPSR_cxsf, %0" \
			: /* No outputs */ \
			: "r" (cpsr_cxsf_value) ); \
	}

/* This macro sets the interrupt related bits in the status register / control
 register to the specified value. */
#define ARM_AR_INT_BITS_SET(set_bits) \
	{ \
		int     tmp_val; \
		tmp_val = mfcpsr(); \
		tmp_val &= ~((unsigned int)CORTEXR5_CPSR_INTERRUPTS_BITS); \
		tmp_val |= set_bits; \
		ARM_AR_CPSR_CXSF_WRITE(tmp_val); \
	}

/* This macro gets the interrupt related bits from the status register / control
 register. */
#define ARM_AR_INT_BITS_GET(get_bits_ptr) \
	{ \
		int     tmp_val; \
		tmp_val = mfcpsr(); \
		tmp_val &= CORTEXR5_CPSR_INTERRUPTS_BITS; \
		*get_bits_ptr = tmp_val; \
	}

#define SWITCH_TO_SYS_MODE() \
	{ \
		mtcpsr((mfcpsr() | XREG_CPSR_SYSTEM_MODE) & ~((unsigned int)CORTEXR5_CPSR_INTERRUPTS_BITS));\
	}

void zynqMP_r5_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr,
		unsigned int size, int is_mem_mapped, CACHE_TYPE cache_type);

int zynqMP_r5_gic_initialize();

int zynqMP_r5_gic_interrupt_enable(int vector_id, INT_TRIG_TYPE trigger_type,
		int priority);
int zynqMP_r5_gic_interrupt_disable(int vector_id);
void restore_global_interrupts();
void disable_global_interrupts();

/* define function macros for OpenAMP API */
#define platform_cache_all_flush_invalidate() \
	{ \
		Xil_DCacheFlush(); \
		Xil_DCacheInvalidate(); \
		Xil_ICacheInvalidate(); \
	}	

#define platform_cache_disable() \
	{ \
		Xil_DCacheDisable(); \
		Xil_ICacheDisable(); \
	}

#define platform_interrupt_enable(...) zynqMP_r5_gic_interrupt_enable(__VA_ARGS__)
#define platform_interrupt_disable(...) zynqMP_r5_gic_interrupt_disable(__VA_ARGS__)
#define platform_map_mem_region(...) 

#endif /* _BAREMETAL_H */
