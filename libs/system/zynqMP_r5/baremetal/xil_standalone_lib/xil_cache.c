/******************************************************************************
*
* Copyright (C) 2014 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information  of Xilinx, Inc.
* and is protected under U.S. and  international copyright and other
* intellectual property  laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any  rights to the
* materials distributed herewith. Except as  otherwise provided in a valid
* license issued to you by  Xilinx, and to the maximum extent permitted by
* applicable law:
* (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND  WITH ALL FAULTS, AND
* XILINX HEREBY DISCLAIMS ALL WARRANTIES  AND CONDITIONS, EXPRESS, IMPLIED,
* OR STATUTORY, INCLUDING  BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and
* (2) Xilinx shall not be liable (whether in contract or tort,  including
* negligence, or under any other theory of liability) for any loss or damage of
* any kind or nature  related to, arising under or in connection with these
* materials, including for any direct, or any indirect,  special, incidental,
* or consequential loss or damage  (including loss of data, profits, goodwill,
* or any type of  loss or damage suffered as a result of any action brought
* by a third party) even if such damage or loss was  reasonably foreseeable
* or Xilinx had been advised of the  possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe  performance, such as life-support or
* safety devices or  systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any  other applications
* that could lead to death, personal  injury, or severe property or environmental
* damage  (individually and collectively, "Critical  Applications").
* Customer assumes the sole risk and liability of any use of Xilinx products in
* Critical  Applications, subject only to applicable laws and  regulations
* governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xil_cache.c
*
* Contains required functions for the ARM cache functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver    Who Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00 	pkp  02/20/14 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_cache.h"
#include "xil_io.h"
#include "xpseudo_asm.h"
#include "xparameters.h"
#include "xreg_cortexr5.h"
#include "xil_exception.h"


/************************** Variable Definitions *****************************/

#define IRQ_FIQ_MASK 0xC0	/* Mask IRQ and FIQ interrupts in cpsr */


extern int  _stack_end;
extern int  __undef_stack;

/****************************************************************************
/************************** Function Prototypes ******************************/

/****************************************************************************
*
* Enable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheEnable(void)
{
	register unsigned int CtrlReg;

	/* enable caches only if they are disabled */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);

	if ((CtrlReg & XREG_CP15_CONTROL_C_BIT)==0) {
		/* invalidate the Data cache */
		Xil_DCacheInvalidate();

		/* enable the Data cache */
		CtrlReg |= (XREG_CP15_CONTROL_C_BIT);

		mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
	}
}

/****************************************************************************
*
* Disable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheDisable(void)
{
	register unsigned int CtrlReg;

	/* clean and invalidate the Data cache */
	Xil_DCacheFlush();

	/* disable the Data cache */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);

	CtrlReg &= ~(XREG_CP15_CONTROL_C_BIT);
dsb();
	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
		isb();
}

/****************************************************************************
*
* Invalidate the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheInvalidate(void)
{
	unsigned int currmask;
	unsigned int stack_start,stack_end,stack_size;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);


	stack_end = (unsigned int )&_stack_end;
	stack_start = (unsigned int )&__undef_stack;
	stack_size=stack_start-stack_end;

	/*Flush stack memory to save return address*/
	Xil_DCacheFlushRange(stack_end, stack_size);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);
dsb();
	/*invalidate all D cache*/
	mtcp(XREG_CP15_INVAL_DC_ALL, 0);
	isb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheInvalidateLine(INTPTR adr)
{
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);
	mtcp(XREG_CP15_INVAL_DC_LINE_MVA_POC, (adr & (~0x1F)));

		/* Wait for invalidate to complete */
	dsb();

	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate the Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheInvalidateRange(INTPTR adr, unsigned len)
{
	const unsigned cacheline = 32;
	unsigned int end;
	unsigned int tempadr = adr;
	unsigned int tempend;
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0) {
		end = tempadr + len;
		tempend = end;
		/* Select L1 Data cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);

		if (tempadr & (cacheline-1)) {
			tempadr &= ~(cacheline - 1);

			Xil_DCacheFlushLine(tempadr);
		}
		if (tempend & (cacheline-1)) {
			tempend &= ~(cacheline - 1);

			Xil_DCacheFlushLine(tempend);
		}

		while (tempadr < tempend) {

		/* Invalidate Data cache line */
		__asm__ __volatile__("mcr " \
		XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (tempadr));

		tempadr += cacheline;
		}
	}

	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheFlush(void)
{
	register unsigned int CsidReg, C7Reg;
	unsigned int CacheSize, LineSize, NumWays;
	unsigned int Way, WayIndex, Set, SetIndex, NumSet;
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	/* Select cache level 0 and D cache in CSSR */
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);

	CsidReg = mfcp(XREG_CP15_CACHE_SIZE_ID);

	/* Determine Cache Size */

	CacheSize = (CsidReg >> 13) & 0x1FF;
	CacheSize +=1;
	CacheSize *=128;    /* to get number of bytes */

	/* Number of Ways */
	NumWays = (CsidReg & 0x3ff) >> 3;
	NumWays += 1;

	/* Get the cacheline size, way size, index size from csidr */
	LineSize = (CsidReg & 0x07) + 4;

	NumSet = CacheSize/NumWays;
	NumSet /= (1 << LineSize);

	Way = 0UL;
	Set = 0UL;

	/* Invalidate all the cachelines */
	for (WayIndex =0; WayIndex < NumWays; WayIndex++) {
		for (SetIndex =0; SetIndex < NumSet; SetIndex++) {
			C7Reg = Way | Set;
			/* Flush by Set/Way */
			__asm__ __volatile__("mcr " \
				XREG_CP15_CLEAN_INVAL_DC_LINE_SW :: "r" (C7Reg));

			Set += (1 << LineSize);
		}
		Set = 0UL;
		Way += 0x40000000;
	}

	/* Wait for flush to complete */
	dsb();
	mtcpsr(currmask);

}

/****************************************************************************
*
* Flush a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheFlushLine(INTPTR adr)
{
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);

	mtcp(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, (adr & (~0x1F)));

		/* Wait for flush to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
* Flush the Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the written to system memory first before the
* before the line is invalidated.
*
* @param	Start address of range to be flushed.
* @param	Length of range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheFlushRange(INTPTR adr, unsigned len)
{
	const unsigned cacheline = 32;
	unsigned int end;
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = adr + len;
		adr &= ~(cacheline - 1);

		while (adr < end) {
			/* Flush Data cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (adr));

			adr += cacheline;
		}
	}
	dsb();
	mtcpsr(currmask);
}
/****************************************************************************
*
* Store a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache and the cacheline is modified (dirty),
* the entire contents of the cacheline are written to system memory.
* After the store completes, the cacheline is marked as unmodified
* (not dirty).
*
* @param	Address to be stored.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheStoreLine(INTPTR adr)
{
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);
	mtcp(XREG_CP15_CLEAN_DC_LINE_MVA_POC, (adr & (~0x1F)));

	/* Wait for store to complete */
	dsb();
	isb();

	mtcpsr(currmask);
}

/****************************************************************************
*
* Enable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheEnable(void)
{
	register unsigned int CtrlReg;

	/* enable caches only if they are disabled */

	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);

	if ((CtrlReg & XREG_CP15_CONTROL_I_BIT)==0) {
		/* invalidate the instruction cache */
		mtcp(XREG_CP15_INVAL_IC_POU, 0);

		/* enable the instruction cache */
		CtrlReg |= (XREG_CP15_CONTROL_I_BIT);

		mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
	}
}

/****************************************************************************
*
* Disable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheDisable(void)
{
	register unsigned int CtrlReg;

	dsb();

	/* invalidate the instruction cache */
	mtcp(XREG_CP15_INVAL_IC_POU, 0);

		/* disable the instruction cache */

	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);

	CtrlReg &= ~(XREG_CP15_CONTROL_I_BIT);
	dsb();
	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
	isb();
}

/****************************************************************************
*
* Invalidate the entire instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheInvalidate(void)
{
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 1);

	/* invalidate the instruction cache */
	mtcp(XREG_CP15_INVAL_IC_POU, 0);

	/* Wait for invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate an instruction cache line.	If the instruction specified by the
* parameter adr is cached by the instruction cache, the cacheline containing
* that instruction is invalidated.
*
* @param	None.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_ICacheInvalidateLine(INTPTR adr)
{
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	mtcp(XREG_CP15_CACHE_SIZE_SEL, 1);
	mtcp(XREG_CP15_INVAL_IC_LINE_MVA_POU, (adr & (~0x1F)));

		/* Wait for invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate the instruction cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated. If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheInvalidateRange(INTPTR adr, unsigned len)
{
	const unsigned cacheline = 32;
	unsigned int end;
	unsigned int currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	if (len != 0) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = adr + len;
		adr = adr & ~(cacheline - 1);

		/* Select cache L0 I-cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 1);

		while (adr < end) {

			/* Invalidate L1 I-cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (adr));

			adr += cacheline;
		}
	}

	/* Wait for invalidate to complete */
	dsb();
	mtcpsr(currmask);
}
