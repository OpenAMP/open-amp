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
#include <stdio.h>
#include <string.h>
#include "xparameters.h"
#include "baremetal.h"
#include "xil_io.h"
#include "xil_exception.h"
	
void zynqMP_r5_irq_isr();

int zynqMP_r5_gic_initialize() {
	unsigned int int_id = 0;
	unsigned int local_cpu_id = (unsigned int)XPAR_CPU_ID + (unsigned int)1;

	Xil_ExceptionDisable();

	/* Initialize the GIC distributor */
	XScuGic_DistWriteReg(XSCUGIC_DIST_EN_OFFSET, 0U);

	/*
	 * Set the security domains in the int_security registers for non-secure
	 * interrupts. All are secure, so leave at the default. Set to 1 for
	 * non-secure interrupts.
	 */

	/*
	 * For the Shared Peripheral Interrupts INT_ID[MAX..32], set:
	 */

	/*
	 * 1. The trigger mode in the int_config register
	 * Only write to the SPI interrupts, so start at 32
	 */
	for (int_id = 32U; int_id<XSCUGIC_MAX_NUM_INTR_INPUTS;int_id=int_id+16U) {
	/*
	 * Each INT_ID uses two bits, or 16 INT_ID per register
	 * Set them all to be level sensitive, active HIGH.
	 */
	 XScuGic_DistWriteReg(XSCUGIC_INT_CFG_OFFSET_CALC(int_id), 0U);
	}


#define DEFAULT_PRIORITY	0xa0a0a0a0U
	for (int_id = 0U; int_id<XSCUGIC_MAX_NUM_INTR_INPUTS;int_id=int_id+4U) {
	/*
	 * 2. The priority using int the priority_level register
	 * The priority_level and spi_target registers use one byte per
	 * INT_ID.
	 * Write a default value that can be changed elsewhere.
	 */
	 XScuGic_DistWriteReg(XSCUGIC_PRIORITY_OFFSET_CALC(int_id), DEFAULT_PRIORITY);
	}

	for (int_id = 32U; int_id<XSCUGIC_MAX_NUM_INTR_INPUTS;int_id=int_id+4U) {
	/*
	 * 3. The CPU interface in the spi_target register
	 * Only write to the SPI interrupts, so start at 32
	 */
		local_cpu_id |= local_cpu_id << 8U;
		local_cpu_id |= local_cpu_id << 16U;
		XScuGic_DistWriteReg(XSCUGIC_SPI_TARGET_OFFSET_CALC(int_id), local_cpu_id);
	}

	for (int_id = 0U; int_id<XSCUGIC_MAX_NUM_INTR_INPUTS;int_id=int_id+32U) {
	/*
	 * 4. Enable the SPI using the enable_set register. Leave all disabled
	 * for now.
	 */
		XScuGic_DistWriteReg(XSCUGIC_EN_DIS_OFFSET_CALC(XSCUGIC_DISABLE_OFFSET, int_id), 0xFFFFFFFFU);
	}

	XScuGic_DistWriteReg(XSCUGIC_DIST_EN_OFFSET, XSCUGIC_EN_INT_MASK);

	/* Program the priority mask of the CPU using the Priority mask register */
	XScuGic_CPUWriteReg(XSCUGIC_CPU_PRIOR_OFFSET, 0xF0U);
	XScuGic_CPUWriteReg(XSCUGIC_CONTROL_OFFSET, 0x07U);	

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)zynqMP_r5_irq_isr, NULL);

	Xil_ExceptionEnable();

	return 0;
}

extern void bm_env_isr(int vector);

void zynqMP_r5_irq_isr() {

	unsigned int raw_irq;
	int irq_vector;
	raw_irq = (unsigned int)XScuGic_CPUReadReg(XSCUGIC_INT_ACK_OFFSET);
	
	irq_vector = (int) (raw_irq & INT_ACK_MASK);

	bm_env_isr(irq_vector);

	XScuGic_CPUWriteReg(XSCUGIC_EOI_OFFSET, raw_irq);
}

int zynqMP_r5_gic_interrupt_enable(int vector_id, INT_TRIG_TYPE trigger_type,
	int priority) {
	/* Not implement setting priority of interrupt */
	unsigned int mask;
	
	mask = 0x00000001U << ((unsigned int)(vector_id) % 32U);
	/*
	 * Enable the selected interrupt source by setting the
	 * corresponding bit in the Enable Set register.
	 */
	XScuGic_DistWriteReg((u32)XSCUGIC_ENABLE_SET_OFFSET + (((unsigned int)(vector_id) / 32U) * 4U), mask);
	return vector_id;
}

int zynqMP_r5_gic_interrupt_disable(int vector_id) {

	unsigned int mask;
	/*
	 * The Int_Id is used to create the appropriate mask for the
   * desired bit position. Int_Id currently limited to 0 - 31
	 */
	mask = 0x00000001U << ((unsigned int)(vector_id) % 32U);

	/*
	 * Disable the selected interrupt source by setting the
	 * corresponding bit in the IDR.
	 */
	XScuGic_DistWriteReg((u32)XSCUGIC_DISABLE_OFFSET + (((unsigned int)(vector_id) / 32U) * 4U), mask);

	return vector_id;
}

extern void bm_env_isr(int vector);


unsigned int old_value = 0;

void restore_global_interrupts() {
	ARM_AR_INT_BITS_SET(old_value);
}

void disable_global_interrupts() {
	unsigned int value = 0;
	ARM_AR_INT_BITS_GET(&value);
	if (value != old_value) {
		ARM_AR_INT_BITS_SET(CORTEXR5_CPSR_INTERRUPTS_BITS);
		old_value = value;
	}
}

/*
 ***********************************************************************
 * IPI handling
 *
 ***********************************************************************
 */

#define IPI_TOTAL 11

typedef void (*ipi_handler_t)(unsigned long ipi_base_addr, unsigned int intr_mask, void *data);

struct ipi_handler_info {
	unsigned long ipi_base_addr;
	unsigned int intr_mask;
	void *data;
	ipi_handler_t ipi_handler;
};

struct ipi_handler_info ipi_handler_table[IPI_TOTAL];

int ipi_index_map (unsigned int ipi_intr_mask) {
	switch (ipi_intr_mask) {
		case 0x08000000:
			return 10;
		case 0x04000000:
			return 9;
		case 0x02000000:
			return 8;
		case 0x01000000:
			return 7;
		case 0x00080000:
			return 6;
		case 0x00040000:
			return 5;
		case 0x00020000:
			return 4;
		case 0x00010000:
			return 3;
		case 0x00000200:
			return 2;
		case 0x00000100:
			return 1;
		case 0x00000001:
			return 0;
		default:
			return -1;
	}
}

void ipi_trigger(unsigned long ipi_base_addr, unsigned int trigger_mask) {
	Xil_Out32((ipi_base_addr + IPI_TRIG_OFFSET), trigger_mask);
}

void ipi_register_handler(unsigned long ipi_base_addr, unsigned int intr_mask, void *data,
	void *ipi_handler) {
	int ipi_hd_i = ipi_index_map(intr_mask);
	if (ipi_hd_i < 0)
		return;
	ipi_handler_table[ipi_hd_i].ipi_base_addr = ipi_base_addr;
	ipi_handler_table[ipi_hd_i].intr_mask = intr_mask;
	ipi_handler_table[ipi_hd_i].ipi_handler = (ipi_handler_t)ipi_handler;
	ipi_handler_table[ipi_hd_i].data = data;
	Xil_Out32((ipi_base_addr + IPI_IER_OFFSET), intr_mask);
}

void ipi_unregister_handler(unsigned long ipi_base_addr, unsigned int intr_mask) {
	int ipi_hd_i = ipi_index_map(intr_mask);
	if (ipi_hd_i < 0)
		return;
	memset(&(ipi_handler_table[ipi_hd_i]), 0, sizeof(struct ipi_handler_info));
}

void ipi_isr(int vect_id, void *data) {
	unsigned long ipi_base_addr = *((unsigned long *)data);
	unsigned int ipi_intr_status = (unsigned int)Xil_In32(ipi_base_addr + IPI_ISR_OFFSET);
	int i = 0;
	do {
		for (i = 0; i < IPI_TOTAL; i++) {
			if (ipi_base_addr != ipi_handler_table[i].ipi_base_addr)
				continue;
			if (!(ipi_intr_status && (ipi_handler_table[i].intr_mask)))
				continue;
			Xil_Out32((ipi_base_addr + IPI_ISR_OFFSET), ipi_handler_table[i].intr_mask);
			ipi_handler_table[i].ipi_handler(ipi_base_addr, ipi_handler_table[i].intr_mask, ipi_handler_table[i].data);
		}
		ipi_intr_status = (unsigned int)Xil_In32(ipi_base_addr + IPI_ISR_OFFSET);
	}while (ipi_intr_status);
}

/***********************************************************************
 *
 *
 * zynqMP_r5_map_mem_region
 *
 *
 * This function sets-up the region of memory based on the given
 * attributes
 * There is no MMU for R5, no need to map phy address to vrt_addr
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
void zynqMP_r5_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr,
                unsigned int size, int is_mem_mapped,
                CACHE_TYPE cache_type) {
	return;
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
__attribute__((weak)) int _fstat(int file, struct stat * st)
{
    return(0);
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
__attribute__((weak)) int _isatty(int file)
{
    return(1);
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
__attribute__((weak)) int _lseek(int file, int ptr, int dir)
{
    return(0);
}

#if (RTL_RPC == 0)
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
__attribute__((weak)) int _open(const char * filename, int flags, int mode)
{
    /* Any number will work. */
    return(1);
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
__attribute__((weak)) int _close(int file)
{
    return(-1);
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
__attribute__((weak)) int _read(int fd, char * buffer, int buflen)
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
__attribute__((weak)) int _write (int file, const char * ptr, int len)
{
    return 0;
}
#endif
