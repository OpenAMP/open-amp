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
#include <stdio.h>
#include <string.h>
#include "baremetal.h"
#include "../../../../porting/env/env.h"
unsigned char ARM_AR_ISR_IRQ_Data[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_FIQ_Data[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_SUP_Stack[ARM_AR_ISR_STACK_SIZE];
unsigned char ARM_AR_ISR_SYS_Stack[ARM_AR_ISR_STACK_SIZE];

static inline unsigned int get_cpu_id_arm(void);

int zc702evk_gic_initialize()
{

	unsigned long reg_val;

	/* Disable architecture interrupts (IRQ and FIQ)
	 * before initialization */
	ARM_AR_CPSR_CXSF_READ(&reg_val);
	reg_val |= (0x02 << 6);
	ARM_AR_CPSR_CXSF_WRITE(reg_val);

	zc702evk_gic_pr_int_initialize();

	/* Enable architecture Interrupts */
	ARM_AR_CPSR_CXSF_READ(&reg_val);
	reg_val &= ~(0x02 << 6);
	ARM_AR_CPSR_CXSF_WRITE(reg_val);

	return 0;
}

/* Only applicable for remote/slave node */
void zc702evk_gic_pr_int_initialize(void)
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

int platform_interrupt_enable(unsigned int vector_id, unsigned int polarity,
			      unsigned int priority)
{
	unsigned long reg_offset;
	unsigned long bit_shift;
	unsigned long temp32 = 0;
	unsigned long targ_cpu;

	temp32 = get_cpu_id_arm();

	/* Determine the necessary bit shift in this target / priority register
	   for this interrupt vector ID */
	bit_shift = ((vector_id) % 4) * 8;

	/* Build a target value based on the bit shift calculated above and the CPU core
	   that this code is executing on */
	targ_cpu = (1 << temp32) << bit_shift;

	/* Determine the Global interrupt controller target / priority register
	   offset for this interrupt vector ID
	   NOTE:  Each target / priority register supports 4 interrupts */
	reg_offset = ((vector_id) / 4) * 4;

	/* Read-modify-write the priority register for this interrupt */
	temp32 = MEM_READ32(INT_GIC_DIST_BASE + INT_GIC_DIST_PRI + reg_offset);

	/* Set new priority. */
	temp32 |= (priority << (bit_shift + 4));
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_PRI + reg_offset, temp32);

	/* Read-modify-write the target register for this interrupt to allow this
	   cpu to accept this interrupt */
	temp32 =
	    MEM_READ32(INT_GIC_DIST_BASE + INT_GIC_DIST_TARGET + reg_offset);
	temp32 |= targ_cpu;
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_TARGET + reg_offset,
		    temp32);

	/* Determine the Global interrupt controller enable set register offset
	   for this vector ID
	   NOTE:  There are 32 interrupts in each enable set register */
	reg_offset = (vector_id / 32) * 4;

	/* Write to the appropriate bit in the enable set register for this
	   vector ID to enable the interrupt */

	temp32 = (1UL << (vector_id - (reg_offset * 0x08)));
	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_ENABLE_SET + reg_offset,
		    temp32);

	/* Return the vector ID */
	return (vector_id);
}

int platform_interrupt_disable(unsigned int vector_id)
{
	unsigned long reg_offset;
	unsigned long bit_shift;
	unsigned long temp32 = 0;
	unsigned long targ_cpu;

	temp32 = get_cpu_id_arm();

	/* Determine the Global interrupt controller enable set register offset
	   for this vector ID
	   NOTE:  There are 32 interrupts in each enable set register */
	reg_offset = (vector_id / 32) * 4;

	/* Write to the appropriate bit in the enable clear register for this
	   vector ID to disable the interrupt */

	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_ENABLE_CLEAR + reg_offset,
		    (1UL << (vector_id - (reg_offset * 0x08))));

	/* Determine the Global interrupt controller target register offset for
	   this interrupt vector ID
	   NOTE:  Each target register supports 4 interrupts */
	reg_offset = (vector_id / 4) * 4;

	/* Determine the necessary bit shift in this target register for this
	   vector ID */
	bit_shift = (vector_id % 4) * 8;

	/* Build a value based on the bit shift calculated above and the CPU core
	   that this code is executing on */
	targ_cpu = (1 << temp32) << bit_shift;

	/* Read-modify-write the target register for this interrupt and remove this cpu from
	   accepting this interrupt */
	temp32 =
	    MEM_READ32(INT_GIC_DIST_BASE + INT_GIC_DIST_TARGET + reg_offset);
	temp32 &= ~targ_cpu;

	MEM_WRITE32(INT_GIC_DIST_BASE + INT_GIC_DIST_TARGET + reg_offset,
		    temp32);

	/* Return the vector ID */
	return (vector_id);
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

/* FIQ Handler */
void __attribute__ ((interrupt("FIQ"))) __cs3_isr_fiq()
{
	while (1) ;
}

static inline unsigned int get_cpu_id_arm(void)
{
	unsigned long cpu_id = 0;

	asm volatile ("MRC p15 ,"
		      "0," "%0," "c0," "c0," "5":[cpu_id] "=&r"(cpu_id)
		      : /* No inputs */ );

	/*
	 * Return cpu id to caller, extract last two bits from Multiprocessor
	 * Affinity Register */
	return (cpu_id & 0x03);
}

int old_value = 0;

void restore_global_interrupts()
{
	ARM_AR_INT_BITS_SET(old_value);
}

void disable_global_interrupts()
{
	int value = 0;
	ARM_AR_INT_BITS_GET(&value);
	if (value != old_value) {
		ARM_AR_INT_BITS_SET(ARM_AR_INTERRUPTS_DISABLE_BITS);
		old_value = value;
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

/***********************************************************************
 *
 *
 * arm_ar_map_mem_region
 *
 *
 * This function sets-up the region of memory based on the given
 * attributes

 *
 * @param vrt_addr       - virtual address of region
 * @param phy_addr       - physical address of region
 * @parma size           - size of region
 * @param is_mem_mapped  - memory mapped or not

 * @param cache_type     - cache type of region
 *
 *
 *   OUTPUTS
 *
 *       None
 *
 ***********************************************************************/
void arm_ar_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr,
			   unsigned int size, int is_mem_mapped,
			   CACHE_TYPE cache_type)
{
	unsigned int section_offset;
	unsigned int ttb_offset;
	unsigned int ttb_value;
	unsigned int ttb_base;

	/* Read ttb base address */
	ARM_AR_CP_READ(ARM_AR_CP15, 0, &ttb_base, ARM_AR_C2, ARM_AR_C0, 0);

	/* Ensure the virtual and physical addresses are aligned on a
	   section boundary */
	vrt_addr &= ARM_AR_MEM_TTB_SECT_SIZE_MASK;
	phy_addr &= ARM_AR_MEM_TTB_SECT_SIZE_MASK;

	/* Loop through entire region of memory (one MMU section at a time).
	   Each section requires a TTB entry. */
	for (section_offset = 0; section_offset < size; section_offset +=
	     ARM_AR_MEM_TTB_SECT_SIZE) {

		/* Calculate translation table entry offset for this memory section */
		ttb_offset = ((vrt_addr + section_offset)
			      >> ARM_AR_MEM_TTB_SECT_TO_DESC_SHIFT);

		/* Build translation table entry value */
		ttb_value = (phy_addr + section_offset)
		    | ARM_AR_MEM_TTB_DESC_ALL_ACCESS;

		if (!is_mem_mapped) {

			/* Set cache related bits in translation table entry.
			   NOTE: Default is uncached instruction and data. */
			if (cache_type == WRITEBACK) {
				/* Update translation table entry value */
				ttb_value |=
				    (ARM_AR_MEM_TTB_DESC_B |
				     ARM_AR_MEM_TTB_DESC_C);
			} else if (cache_type == WRITETHROUGH) {
				/* Update translation table entry value */
				ttb_value |= ARM_AR_MEM_TTB_DESC_C;
			}
			/* In case of un-cached memory, set TEX 0 bit to set memory
			   attribute to normal. */
			else if (cache_type == NOCACHE) {
				ttb_value |= ARM_AR_MEM_TTB_DESC_TEX;
			}
		}

		/* Write translation table entry value to entry address */
		MEM_WRITE32(ttb_base + ttb_offset, ttb_value);

	}			/* for loop */
}

void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr,
			     unsigned int size, unsigned int flags)
{
	int is_mem_mapped = 0;
	int cache_type = 0;

	if ((flags & (0x0f << 4)) == MEM_MAPPED) {
		is_mem_mapped = 1;
	}

	if ((flags & 0x0f) == WB_CACHE) {
		cache_type = WRITEBACK;
	} else if ((flags & 0x0f) == WT_CACHE) {
		cache_type = WRITETHROUGH;
	} else {
		cache_type = NOCACHE;
	}

	arm_ar_map_mem_region(vrt_addr, phy_addr, size, is_mem_mapped,
			      cache_type);
}

void platform_cache_all_flush_invalidate()
{
	ARM_AR_MEM_DCACHE_ALL_OP(1);
}

void platform_cache_disable()
{
	ARM_AR_MEM_CACHE_DISABLE();
}

unsigned long platform_vatopa(void *addr)
{
	return (((unsigned long)addr & (~(0x0fff << 20))) | (0x08 << 24));
}

void *platform_patova(unsigned long addr)
{
	return ((void *)addr);

}

/*==================================================================*/
/* The function definitions below are provided to prevent the build */
/* warnings for missing I/O function stubs in case of unhosted libs */
/*==================================================================*/

#include            <sys/stat.h>

/**
 * _fstat
 *
 * Status of an open file. For consistency with other minimal
 * implementations in these examples, all files are regarded
 * as character special devices.
 *
 * @param file    - Unused.
 * @param st      - Status structure.
 *
 *
 *       A constant value of 0.
 *
 **/
__attribute__ ((weak))
int _fstat(int file, struct stat *st)
{
	return (0);
}

/**
 *  isatty
 *
 *
 * Query whether output stream is a terminal. For consistency
 * with the other minimal implementations, which only support
 * output to stdout, this minimal implementation is suggested
 *
 * @param file    - Unused
 *
 * @return s - A constant value of 1.
 *
 */
__attribute__ ((weak))
int _isatty(int file)
{
	return (1);
}

/**
 *_lseek
 *
 * Set position in a file. Minimal implementation.

 *
 * @param file    - Unused
 *
 * @param ptr     - Unused
 *
 * @param dir     - Unused
 *
 * @return - A constant value of 0.
 *
 */
__attribute__ ((weak))
int _lseek(int file, int ptr, int dir)
{
	return (0);
}

/**
 *  _open
 *
 * Open a file.  Minimal implementation
 *
 * @param filename    - Unused
 * @param flags       - Unused
 * @param mode        - Unused
 *
 * return -  A constant value of 1.
 *
 */
__attribute__ ((weak))
int _open(const char *filename, int flags, int mode)
{
	/* Any number will work. */
	return (1);
}

/**
 *  _close
 *
 * Close a file.  Minimal implementation.
 *
 *
 * @param file    - Unused
 *
 *
 * return A constant value of -1.
 *
 */
__attribute__ ((weak))
int _close(int file)
{
	return (-1);
}

/**
 * _read
 *
 *  Low level function to redirect IO to serial.
 *
 * @param fd          - Unused
 * @param buffer      - Buffer where read data will be placed.
 * @param buflen      - Size (in bytes) of buffer.
 *
 * return -  A constant value of 1.
 *
 */
__attribute__ ((weak))
int _read(int fd, char *buffer, int buflen)
{
	return -1;
}

/**
 * _write
 *
 * Low level function to redirect IO to serial.
 *
 *
 * @param file                          - Unused
 * @param CHAR *ptr                         - String to output
 * @param len                           - Length of the string
 *
 * return len                            - The length of the string
 *
 */
__attribute__ ((weak))
int _write(int file, const char *ptr, int len)
{
	return 0;
}
