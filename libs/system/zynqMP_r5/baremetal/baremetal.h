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
#include "xparameters.h"

/*
 * The maximum number of interrupts supported by the hardware.
 */
#define XSCUGIC_MAX_NUM_INTR_INPUTS	190U

/** name CPU Interface Register Map
 *
 * Define the offsets from the base address for all CPU registers of the
 * interrupt controller, some registers may be reserved in the hardware device.
 */
#define XSCUGIC_CONTROL_OFFSET		0x00000000U /**< CPU Interface Control
							Register */
#define XSCUGIC_CPU_PRIOR_OFFSET	0x00000004U /**< Priority Mask Reg */
#define XSCUGIC_BIN_PT_OFFSET		0x00000008U /**< Binary Point Register */
#define XSCUGIC_INT_ACK_OFFSET		0x0000000CU /**< Interrupt ACK Reg */
#define XSCUGIC_EOI_OFFSET		0x00000010U /**< End of Interrupt Reg */
#define XSCUGIC_RUN_PRIOR_OFFSET	0x00000014U /**< Running Priority Reg */
#define XSCUGIC_HI_PEND_OFFSET		0x00000018U /**< Highest Pending Interrupt
							Register */
#define XSCUGIC_ALIAS_BIN_PT_OFFSET	0x0000001CU /**< Aliased non-Secure
							Binary Point Register */

/** name Distributor Interface Register Map
 *
 * Define the offsets from the base address for all Distributor registers of
 * the interrupt controller, some registers may be reserved in the hardware
 * device.
 */
#define XSCUGIC_DIST_EN_OFFSET          0x00000000U /**< Distributor Enable
                                                        Register */
#define XSCUGIC_IC_TYPE_OFFSET          0x00000004U /**< Interrupt Controller
                                                        Type Register */
#define XSCUGIC_DIST_IDENT_OFFSET       0x00000008U /**< Implementor ID
                                                        Register */
#define XSCUGIC_SECURITY_OFFSET         0x00000080U /**< Interrupt Security
                                                        Register */
#define XSCUGIC_ENABLE_SET_OFFSET       0x00000100U /**< Enable Set
                                                        Register */
#define XSCUGIC_DISABLE_OFFSET          0x00000180U /**< Enable Clear Register */
#define XSCUGIC_PENDING_SET_OFFSET      0x00000200U /**< Pending Set
                                                        Register */
#define XSCUGIC_PENDING_CLR_OFFSET      0x00000280U /**< Pending Clear
                                                        Register */
#define XSCUGIC_ACTIVE_OFFSET           0x00000300U /**< Active Status Register */
#define XSCUGIC_PRIORITY_OFFSET         0x00000400U /**< Priority Level Register */
#define XSCUGIC_SPI_TARGET_OFFSET       0x00000800U /**< SPI Target
                                                        Register 0x800-0x8FB */
#define XSCUGIC_INT_CFG_OFFSET          0x00000C00U /**< Interrupt Configuration
                                                        Register 0xC00-0xCFC */
#define XSCUGIC_PPI_STAT_OFFSET         0x00000D00U /**< PPI Status Register */
#define XSCUGIC_SPI_STAT_OFFSET         0x00000D04U /**< SPI Status Register
                                                        0xd04-0xd7C */
#define XSCUGIC_AHB_CONFIG_OFFSET       0x00000D80U /**< AHB Configuration
                                                        Register */
#define XSCUGIC_SFI_TRIG_OFFSET         0x00000F00U /**< Software Triggered
                                                        Interrupt Register */
#define XSCUGIC_PERPHID_OFFSET          0x00000FD0U /**< Peripheral ID Reg */
#define XSCUGIC_PCELLID_OFFSET          0x00000FF0U /**< Pcell ID Register */

#define XSCUGIC_EN_INT_MASK             0x00000001U /**< Interrupt In Enable */

#define XSCUGIC_SPI_TARGET_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_SPI_TARGET_OFFSET + (((InterruptID)/4U) * 4U))

#define XSCUGIC_INT_CFG_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_INT_CFG_OFFSET + (((InterruptID)/16U) * 4U))

#define XSCUGIC_EN_DIS_OFFSET_CALC(Register, InterruptID) \
		((Register) + (((InterruptID)/32U) * 4U))

#define XSCUGIC_PRIORITY_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_PRIORITY_OFFSET + (((InterruptID)/4U) * 4U))

#define XScuGic_CPUWriteReg(RegOffset, Data) \
	(Xil_Out32((XPAR_SCUGIC_0_CPU_BASEADDR + RegOffset), Data))

#define XScuGic_CPUReadReg(RegOffset) \
	(Xil_In32(XPAR_SCUGIC_0_CPU_BASEADDR + RegOffset))

#define XScuGic_DistWriteReg(RegOffset, Data) \
	(Xil_Out32((XPAR_SCUGIC_0_DIST_BASEADDR + RegOffset), Data))

#define XScuGic_DistReadReg(RegOffset) \
	(Xil_In32(XPAR_SCUGIC_0_DIST_BASEADDR + RegOffset))

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
