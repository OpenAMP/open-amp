/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
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
#include <string.h>
#include "baremetal.h"
#include "metal/sys.h"

#ifndef BAREMETAL_MASTER
#define BAREMETAL_MASTER 0
#endif

/* Memory Regions for MMU Mapping */
#if (BAREMETAL_MASTER == 1)

#define ELF_START       0x10000000	/* Image entry point address */
#define ELF_END         0x0FE00000	/* size of code,data,heap and stack sections */

#define TLB_MEM_START   0x1FE00000	/* Address of TLB memory */

#else

#define ELF_START       0x00000000	/* Image entry point address */
#define ELF_END         0x08000000	/* size of code,data,heap and stack sections */

#define TLB_MEM_START   0x0FE00000	/* Address of TLB memory */

#endif

/* The vector table address is the same as image entry point */
#define RAM_VECTOR_TABLE_ADDR           ELF_START

unsigned char ARM_AR_ISR_IRQ_Data[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_FIQ_Data[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_SUP_Stack[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_SYS_Stack[ARM_AR_ISR_STACK_SIZE];

extern void bm_env_isr(int vector);

/* IRQ handler */
void __attribute__ ((interrupt("IRQ"))) __cs3_isr_irq()
{
	unsigned long raw_irq;
	int irq_vector;

	/* Read the Interrupt ACK register */
	raw_irq = MEM_READ32(INT_GIC_CPU_BASE + INT_GIC_CPU_ACK);

	/* mask interrupt to get vector */
	irq_vector = raw_irq & INT_ACK_MASK;

	bm_env_isr(irq_vector);

	/* Clear the interrupt */
	MEM_WRITE32(INT_GIC_CPU_BASE + INT_GIC_CPU_ENDINT, raw_irq);
}

/* Only applicable for remote/slave node */
void zynq7_gic_pr_int_initialize(void)
{

	/* Disable the GIC controller */
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_CTRL, 0x00000000);

	/* Enable the interrupt distributor controller */
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_CTRL, INT_DIST_ENABLE);

	/* Secondary cores  just need to disable their private interrupts */
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_ENABLE_CLEAR + 0x00,
		    0xffffffff);
	/* 0  - 31 */

	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_CONFIG + 0x00, 0xAAAAAAAA);
	/* 0  - 15 */
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_CONFIG + 0x04, 0xAAAAAAAA);

	/* Disable the CPU Interface */
	MEM_WRITE32(INT_GIC_CPU_BASE + INT_GIC_CPU_CTRL, 0x00000000);

	/* Allow interrupts with more priority (i.e. lower number) than FF */
	MEM_WRITE32(INT_GIC_CPU_BASE + INT_GIC_CPU_PRIORITY, 0x000000FF);

	/* No binary point */
	MEM_WRITE32(INT_GIC_CPU_BASE + INT_GIC_CPU_POINT, 0x00000000);

	/* Enable the CPU Interface */
	MEM_WRITE32(INT_GIC_CPU_BASE + INT_GIC_CPU_CTRL, INT_CPU_ENABLE);
}

int zynq7_gic_initialize()
{

	unsigned long reg_val;

	/* Disable architecture interrupts (IRQ and FIQ)
	 * before initialization */
	ARM_AR_CPSR_CXSF_READ(&reg_val);
	reg_val |= (0x02 << 6);
	ARM_AR_CPSR_CXSF_WRITE(reg_val);

	zynq7_gic_pr_int_initialize();

	/* Enable architecture Interrupts */
	ARM_AR_CPSR_CXSF_READ(&reg_val);
	reg_val &= ~(0x02 << 6);
	ARM_AR_CPSR_CXSF_WRITE(reg_val);

	return 0;
}

void arm_arch_install_isr_vector_table(unsigned long addr)
{
	unsigned long arch = 0;
	void *dst_addr;

	/* Assign destination address of vector table to RAM address */
	dst_addr = (void *)addr;
	/* Read Main ID Register (MIRD) */
	ARM_AR_CP_READ(p15, 0, &arch, c0, c0, 0);

	/* Check if Cortex-A series of ARMv7 architecture. */
	if (((arch & MIDR_ARCH_MASK) >> 16) == MIDR_ARCH_ARMV7
	    && ((arch & MIDR_PART_NO_MASK) >> 4)
	    == MIDR_PART_NO_CORTEX_A) {
		/* Set vector base address */
		ARM_AR_CP_WRITE(p15, 0, dst_addr, c12, c0, 0);
		ARM_AR_NOP_EXECUTE();
		ARM_AR_NOP_EXECUTE();
		ARM_AR_NOP_EXECUTE();
	}
}

void init_arm_stacks(void)
{

	/* Switch to IRQ mode (keeping interrupts disabled) */
	ARM_AR_CPSR_C_WRITE(ARM_AR_INT_CPSR_IRQ_MODE |
			    ARM_AR_INTERRUPTS_DISABLE_BITS);

	/* Set IRQ stack pointer */
	ARM_AR_SP_WRITE(ARM_GE_STK_ALIGN
			(&ARM_AR_ISR_IRQ_Data[ARM_AR_ISR_STACK_SIZE - 1]));

	/* Switch to FIQ mode (keeping interrupts disabled) */
	ARM_AR_CPSR_C_WRITE(ARM_AR_INT_CPSR_FIQ_MODE |
			    ARM_AR_INTERRUPTS_DISABLE_BITS);

	/* Set FIQ stack pointer */
	ARM_AR_SP_WRITE(ARM_GE_STK_ALIGN
			(ARM_AR_ISR_FIQ_Data[ARM_AR_ISR_STACK_SIZE - 1]));

	/* Switch to Supervisor mode (keeping interrupts disabled) */
	ARM_AR_CPSR_C_WRITE(ARM_AR_INT_CPSR_SUP_MODE |
			    ARM_AR_INTERRUPTS_DISABLE_BITS);

	/* Set Supervisor stack pointer */
	ARM_AR_SP_WRITE(ARM_GE_STK_ALIGN
			(&ARM_AR_ISR_SUP_Stack[ARM_AR_ISR_STACK_SIZE - 1]));

	/* Switch to System mode (keeping interrupts disabled) */
	ARM_AR_CPSR_C_WRITE(ARM_AR_INT_CPSR_SYS_DISABLED);
}

/***********************************************************************
 *
 *  arm_ar_mem_enable_mmu
 *
 *  Enables MMU and MAP the required memory regions.
 *
 ***********************************************************************/
int arm_ar_mem_enable_mmu()
{
	unsigned int cp15_ctrl_val;
	void *tlb_mem = (void *)TLB_MEM_START;

	ARM_AR_MEM_CACHE_ALL_INVALIDATE();

	/* Read current CP15 control register value */
	ARM_AR_CP_READ(ARM_AR_CP15, 0, &cp15_ctrl_val, ARM_AR_C1, ARM_AR_C0, 0);

	/* Clear the V bit(13) to set Normal exception vectors range. */
	cp15_ctrl_val &= ~(ARM_AR_MEM_CP15_CTRL_V);

	/* Clear the alignment bit(1) to enable unaligned memory accesses */
	cp15_ctrl_val &= ~(ARM_AR_MEM_CP15_CTRL_A);

	/* Write updated CP15 control register value */
	ARM_AR_CP_WRITE(ARM_AR_CP15, 0, cp15_ctrl_val, ARM_AR_C1, ARM_AR_C0, 0);

	ARM_AR_NOP_EXECUTE();
	ARM_AR_NOP_EXECUTE();
	ARM_AR_NOP_EXECUTE();

	/* Check alignment of available memory pointer */
	if (!(MEM_ALIGNED_CHECK(tlb_mem, ARM_AR_MEM_TTB_SIZE))) {
		/* Align the pointer to the required boundary */
		tlb_mem = MEM_PTR_ALIGN(tlb_mem, ARM_AR_MEM_TTB_SIZE);
	}

	/* Clear the entire translation table */
	memset(tlb_mem, 0x00, ARM_AR_MEM_TTB_SIZE);

	/* Set translation table base address */
	ARM_AR_CP_WRITE(ARM_AR_CP15, 0, tlb_mem, ARM_AR_C2, ARM_AR_C0, 0);

	ARM_AR_CP_READ(ARM_AR_CP15, 0, &cp15_ctrl_val, ARM_AR_C2, ARM_AR_C0, 0);

	/* Map the given memory regions here */
	arm_ar_map_mem_region(ELF_START, ELF_START, ELF_END, 0, WRITEBACK);
	arm_ar_map_mem_region((unsigned int)tlb_mem, (unsigned int)tlb_mem,
			      TLB_SIZE, 0, NOCACHE);
	arm_ar_map_mem_region(PERIPH_BASE, PERIPH_BASE,
			      PERIPH_SIZE, 1, NOCACHE);
	arm_ar_map_mem_region(SLCR_BASE, SLCR_BASE, SLCR_SIZE, 1, NOCACHE);
	arm_ar_map_mem_region(CPU_BASE, CPU_BASE, CPU_SIZE, 1, NOCACHE);

	/* Set the domain access for domain D0 */
	ARM_AR_CP_WRITE(ARM_AR_CP15, 0, ARM_AR_MEM_DOMAIN_D0_MANAGER_ACCESS,
			ARM_AR_C3, ARM_AR_C0, 0);

	ARM_AR_CP_READ(ARM_AR_CP15, 0, &cp15_ctrl_val, ARM_AR_C3, ARM_AR_C0, 0);

	/* Invalidate all TLB entries before enabling the MMU */
	ARM_AR_CP_WRITE(ARM_AR_CP15, 0, 0, ARM_AR_C8, ARM_AR_C7, 0);

	/* Read current CP15 control register value */
	ARM_AR_CP_READ(ARM_AR_CP15, 0, &cp15_ctrl_val, ARM_AR_C1, ARM_AR_C0, 0);

	/* Set instruction cache enable / data cache enable / MMU enable bits */
	cp15_ctrl_val |= (ARM_AR_MEM_CP15_CTRL_I | ARM_AR_MEM_CP15_CTRL_C
			  | ARM_AR_MEM_CP15_CTRL_M | ARM_AR_MEM_CP15_CTRL_Z);

	/* Write updated CP15 control register value */
	ARM_AR_CP_WRITE(ARM_AR_CP15, 0, cp15_ctrl_val, ARM_AR_C1, ARM_AR_C0, 0);

	ARM_AR_NOP_EXECUTE();
	ARM_AR_NOP_EXECUTE();
	ARM_AR_NOP_EXECUTE();

	return 0;
}

void init_system()
{
	/* Place the vector table at the image entry point */
	arm_arch_install_isr_vector_table(RAM_VECTOR_TABLE_ADDR);

	/* Enable MMU */
	arm_ar_mem_enable_mmu();

	/* Initialize ARM stacks */
	init_arm_stacks();

	/* Initialize GIC */
	zynq7_gic_initialize();
}

void cleanup_system()
{
	metal_finish();
	Xil_DCacheInvalidate();
	Xil_ICacheInvalidate();
	Xil_DCacheDisable();
	Xil_ICacheDisable();
}
