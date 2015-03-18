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
* @file xil_mmu.c
*
* This file provides APIs for enabling/disabling MMU and setting the memory
* attributes for sections, in the MMU translation table.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.00  pkp  02/10/14 Initial version
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_cache.h"
#include "xpseudo_asm.h"
#include "xil_types.h"
#include "xil_mpu.h"


/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/************************** Variable Definitions *****************************/

u32 MpuAddr[8];	/* addresses configured in MPU*/
u8 RegNum[8];	/* region number configures corresponding to address in MpuAddr */
u8 CurrIndex;

/************************** Function Prototypes ******************************/

/*****************************************************************************
*
* Set the memory attributes for a section of memory with starting address addr
* of the region size defined by reg_size having attributes attrib of region number
* reg_num
*
* @param	addr is the address for which attributes are to be set.
* @param	attrib specifies the attributes for that memory region.
* @param	reg_size specifies the size for that memory region.
* @param	reg_num specifies the number for that memory region.
* @return	None.
*
*
******************************************************************************/
void Xil_SetTlbAttributes(INTPTR addr, u32 attrib)
{
	u32 CtrlReg,reg_size,status = 0;
	u32 Index;
	INTPTR Local_addr = addr;
	u8 reg_num;
	s32 DCacheStatus=0, ICacheStatus=0;

	/* disbale caches only if they are enabled */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
	if ((CtrlReg & XREG_CP15_CONTROL_C_BIT) != 0x00000000U) {
		DCacheStatus=1;
	}
	if ((CtrlReg & XREG_CP15_CONTROL_I_BIT) != 0x00000000U) {
		ICacheStatus=1;
	}
	if(DCacheStatus != 0) {
		Xil_DCacheDisable();
	}
	if(ICacheStatus != 0){
		Xil_ICacheDisable();
	}

	/* adjust the address for 4K alignment */
	Local_addr = (Local_addr & (~(0xFFFFF)));

	if(MpuAddr[0] == 0){
		/* define the region to be 8 for first use of
		Xil_SetTlbAttributes API as first 7 regions are defined
		for given address map */
		RegNum[CurrIndex] = mfcp(XREG_CP15_MPU_MEMORY_REG_NUMBER);
		MpuAddr[CurrIndex] = Local_addr;
	} else {
		/* Check if the address is already configured with different attributes before */
		for(Index=0; Index < RegNum[CurrIndex] - 8; Index++){
			if(addr == MpuAddr[CurrIndex-Index]){
				/*if the address already exist in memory region of mpu
					use the same region number with given attribute*/
				reg_num = RegNum[CurrIndex-Index];
				status = 1;
				break;
			}
		}
	}
	/* add the new entry of array of address and region number
	for first time configuration */
	if(status == 0){
		CurrIndex++;
		RegNum[CurrIndex] = RegNum[CurrIndex-1] + 1;
		MpuAddr[CurrIndex] = Local_addr;
		reg_num = RegNum[CurrIndex];
	}

	reg_size = REGION_1M<<1;						/* region size is hardcoded to 4KB */
	reg_size|=REGION_EN;
	mtcp(XREG_CP15_MPU_MEMORY_REG_NUMBER,reg_num);
	mtcp(XREG_CP15_MPU_REG_BASEADDR,Local_addr); 			/* Set base address of a region */
	mtcp(XREG_CP15_MPU_REG_ACCESS_CTRL,attrib); 	/* Set the control attribute */
	mtcp(XREG_CP15_MPU_REG_SIZE_EN,reg_size); 		/* set the region size and enable it*/
	dsb(); 											/* ensure completion of the BP invalidation */
	isb();											/* synchronize context on this processor */
	/* enable caches only if they are disabled in routine*/
	if(DCacheStatus != 0) {
		Xil_DCacheEnable();
	}
	if(ICacheStatus != 0) {
		Xil_ICacheEnable();
	}
}

/*****************************************************************************
*
* Enable MPU for Cortex R5 processor. This function invalidates I cache and
* flush the D Caches before enabling the MPU.
*
*
* @param	None.
* @return	None.
*
******************************************************************************/
void Xil_EnableMPU(void)
{
	u32 CtrlReg, Reg;
	s32 DCacheStatus=0, ICacheStatus=0;
	/* enable caches only if they are disabled */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
	if ((CtrlReg & XREG_CP15_CONTROL_C_BIT) != 0x00000000U) {
		DCacheStatus=1;
	}
	if ((CtrlReg & XREG_CP15_CONTROL_I_BIT) != 0x00000000U) {
		ICacheStatus=1;
	}

	if(DCacheStatus != 0) {
		Xil_DCacheDisable();
	}
	if(ICacheStatus != 0){
		Xil_ICacheDisable();
	}
	Reg = mfcp(XREG_CP15_SYS_CONTROL);
	Reg |= 0x00000001U;
	dsb();
	mtcp(XREG_CP15_SYS_CONTROL, Reg);
	isb();
	/* enable caches only if they are disabled in routine*/
	if(DCacheStatus != 0) {
		Xil_DCacheEnable();
	}
	if(ICacheStatus != 0) {
		Xil_ICacheEnable();
	}
}

/*****************************************************************************
*
* Disable MPU for Cortex R5 processors. This function invalidates I cache and
* flush the D Caches before disabling the MPU.
*
* @param	None.
*
* @return	None.
*
******************************************************************************/
void Xil_DisableMPU(void)
{
	u32 CtrlReg, Reg;
	s32 DCacheStatus=0, ICacheStatus=0;
	/* enable caches only if they are disabled */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
	if ((CtrlReg & XREG_CP15_CONTROL_C_BIT) != 0x00000000U) {
		DCacheStatus=1;
	}
	if ((CtrlReg & XREG_CP15_CONTROL_I_BIT) != 0x00000000U) {
		ICacheStatus=1;
	}

	if(DCacheStatus != 0) {
		Xil_DCacheDisable();
	}
	if(ICacheStatus != 0){
		Xil_ICacheDisable();
	}

	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);
	Reg = mfcp(XREG_CP15_SYS_CONTROL);
	Reg &= ~(0x00000001U);
	dsb();
	mtcp(XREG_CP15_SYS_CONTROL, Reg);
	isb();
	/* enable caches only if they are disabled in routine*/
	if(DCacheStatus != 0) {
		Xil_DCacheEnable();
	}
	if(ICacheStatus != 0) {
		Xil_ICacheEnable();
	}
}
