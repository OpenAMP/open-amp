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
#include "baremetal.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"

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


#define XScuGic_CPUWriteReg(RegOffset, Data) \
	(Xil_Out32((XPAR_SCUGIC_0_CPU_BASEADDR + RegOffset), Data))

#define XScuGic_CPUReadReg(RegOffset) \
	(Xil_In32(XPAR_SCUGIC_0_CPU_BASEADDR + RegOffset))

#define XScuGic_DistWriteReg(RegOffset, Data) \
	(Xil_Out32((XPAR_SCUGIC_0_DIST_BASEADDR + RegOffset), Data))

#define XScuGic_DistReadReg(RegOffset) \
	(Xil_In32(XPAR_SCUGIC_0_DIST_BASEADDR + RegOffset))
	
void zynqMP_r5_irq_isr();

int zynqMP_r5_gic_initialize() {

	Xil_ExceptionDisable();

	/* Assuming master has initialized the GIC distributor,
	 * do not initialize distributor from slave */

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
int _fstat(int file, struct stat * st)
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
int _isatty(int file)
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
int _lseek(int file, int ptr, int dir)
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
int _open(const char * filename, int flags, int mode)
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
int _close(int file)
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
 int _read(int fd, char * buffer, int buflen)
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
 int _write (int file, const char * ptr, int len)
{
    return 0;
}
#endif
