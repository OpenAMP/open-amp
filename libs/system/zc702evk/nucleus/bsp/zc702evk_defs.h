/***********************************************************************
*
*             Copyright 2013 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************

************************************************************************
*
*   FILE NAME
*
*       zc702evk_defs.h
*
*   DESCRIPTION
*
*       This file contains platform definitions for the Xilinx ZC702 EVK board
*
*   DATA STRUCTURES
*
*       None
*
*   DEPENDENCIES
*
*       None
*
***********************************************************************/

#ifndef         ZC702EVK_DEFS_H
#define         ZC702EVK_DEFS_H

/* Define the board clock rate (in hertz).
   NOTE: This clock rate is used to calculate the rate of the OS timer.
         Therefore, if multiple clock sources are fed to the processor,
         this clock rate value must represent the source used
         by the on-chip timer unit. */
#define         ESAL_DP_REF_CLOCK_RATE                  33333333UL

/* Define number of memory regions contained on the given development platform */
#define         ESAL_DP_MEM_NUM_REGIONS                 6

/* Define if an interrupt controller (off processor) exists on the board and
   controlling / handling of interrupts from this interrupt controller must
   be accommodated for.  Setting this to NU_FALSE means off-chip interrupts
   will NOT be controlled or handled.  Setting this to NU_TRUE means off-chip
   interrupts will be controlled and handled */
#define         ESAL_DP_INTERRUPTS_AVAILABLE            NU_FALSE

/* Define the base of address on-chip peripheral registers */
#define         ESAL_DP_PERIPH_BASE                     0xE0000000

/* Define ESAL interrupt vector IDs for this development platform.
   These IDs match up with development platform interrupts.
   Values correspond to the index of entries in ESAL_GE_ISR_Interrupt_Handler[].
   Names are of the form ESAL_DP_<Name>_INT_VECTOR_ID, where <Name> comes
   directly from the hardware documentation */


/* Define the last ESAL interrupt vector ID for this development platform + 1 */
#define         ESAL_DP_INT_VECTOR_ID_DELIMITER         (ESAL_PR_INT_VECTOR_ID_DELIMITER + 1)

/* Define the system level control registers address*/
#define         ESAL_DP_SLCR_BASE                       0xF8000000


#endif  /* ZC702EVK_DEFS_H */
