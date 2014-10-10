/***********************************************************************
*
*             Copyright 2006 Mentor Graphics Corporation
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
*       arm_debug_defs.h
*
*   DESCRIPTION
*
*       This file contains all definitions, structures, etc for the
*       ARM Debug.
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

#ifndef         ARM_DEBUG_DEFS_H
#define         ARM_DEBUG_DEFS_H

/* External functions */
extern VOID     ESAL_AR_DBG_Exception_Exit(VOID);

/* Debug Support flags */
#define         ESAL_GE_DBG_SUPPORT_HW_STEP         (0x1 << 0)

/* Protocol specific register model definitions */
#define         NU_REG_RSP_MODE                   1               
#define         NU_REG_MDP_MODE                   2

/* Remote Serial Protocol for GNU Debugger */
#define         ESAL_AR_DBG_RSP_NUM_REGS            26
#define         ESAL_AR_DBG_RSP_BLOCK_REGS          16
#define         ESAL_AR_DBG_STACK_PTR_REGID         13
#define         ESAL_AR_DBG_PC_IDX_REGID            15

/* Register Mapping values (positive values are register offsets) */
#define         ESAL_AR_DBG_REG_NOT_MAPPED      -1
#define         ESAL_AR_DBG_REG_STACK_POINTER   -2

/* Register definition */
#define         ESAL_AR_DBG_REG_R0                  0x00
#define         ESAL_AR_DBG_REG_R1                  0x01
#define         ESAL_AR_DBG_REG_R2                  0x02
#define         ESAL_AR_DBG_REG_R3                  0x03
#define         ESAL_AR_DBG_REG_R4                  0x04
#define         ESAL_AR_DBG_REG_R5                  0x05
#define         ESAL_AR_DBG_REG_R6                  0x06
#define         ESAL_AR_DBG_REG_R7                  0x07
#define         ESAL_AR_DBG_REG_R8                  0x08
#define         ESAL_AR_DBG_REG_R9                  0x09
#define         ESAL_AR_DBG_REG_R10                 0x0A
#define         ESAL_AR_DBG_REG_R11                 0x0B
#define         ESAL_AR_DBG_REG_R12                 0x0C
#define         ESAL_AR_DBG_REG_R13                 0x0D
#define         ESAL_AR_DBG_REG_R14                 0x0E
#define         ESAL_AR_DBG_REG_R15                 0x0F
#define         ESAL_AR_DBG_REG_FPR0                0x10
#define         ESAL_AR_DBG_REG_FPR1                0x11
#define         ESAL_AR_DBG_REG_FPR2                0x12
#define         ESAL_AR_DBG_REG_FPR3                0x13
#define         ESAL_AR_DBG_REG_FPR4                0x14
#define         ESAL_AR_DBG_REG_FPR5                0x15
#define         ESAL_AR_DBG_REG_FPR6                0x16
#define         ESAL_AR_DBG_REG_FPR7                0x17
#define         ESAL_AR_DBG_REG_FPS                 0x18
#define         ESAL_AR_DBG_REG_CPSR                0x19

/* Number of maximum registers in stack frame */
#define         ESAL_AR_DBG_NUM_REGS                ESAL_AR_DBG_RSP_NUM_REGS

/* Number of expedited registers */
#define         ESAL_AR_DBG_EXP_NUM_REGS            3

/* breakpoint instructions for specific instruction replacement */
#define         ESAL_AR_DBG_ARM_BRK_OPCODE          0xE7FDDEFE
#define         ESAL_AR_DBG_THUMB_BRK_OPCODE        0x0000DEFE

/* NOP instructions for specific instruction replacement */
#define         ESAL_AR_DBG_ARM_NOP_OPCODE          0xE1A00000 
#define         ESAL_AR_DBG_THUMB_NOP_OPCODE        0x1C00

/* Define flags for PSR */
#define         ESAL_AR_DBG_PSR_N                   ESAL_GE_MEM_32BIT_SET(31)
#define         ESAL_AR_DBG_PSR_Z                   ESAL_GE_MEM_32BIT_SET(30)
#define         ESAL_AR_DBG_PSR_C                   ESAL_GE_MEM_32BIT_SET(29)
#define         ESAL_AR_DBG_PSR_V                   ESAL_GE_MEM_32BIT_SET(28)
#define         ESAL_AR_DBG_PSR_Q                   ESAL_GE_MEM_32BIT_SET(27) 
#define         ESAL_AR_DBG_PSR_I                   ESAL_GE_MEM_32BIT_SET(7)
#define         ESAL_AR_DBG_PSR_F                   ESAL_GE_MEM_32BIT_SET(6)
#define         ESAL_AR_DBG_PSR_T                   ESAL_GE_MEM_32BIT_SET(5)
#define         ESAL_AR_DBG_PSR_M                   0x0000001f
#define         ESAL_AR_DBG_PSR_M32                 0x00000010
#define         ESAL_AR_DBG_PSR_M_USER              0x00000000
#define         ESAL_AR_DBG_PSR_M_FIQ               0x00000001
#define         ESAL_AR_DBG_PSR_M_IRQ               0x00000002
#define         ESAL_AR_DBG_PSR_M_SVC               0x00000003
#define         ESAL_AR_DBG_PSR_M_ABORT             0x00000007
#define         ESAL_AR_DBG_PSR_M_UNDEF             0x0000000b
#define         ESAL_AR_DBG_PSR_M_SYSTEM            0x0000000f
#define         ESAL_AR_DBG_PSR_32_IF               (ESAL_ARM_PSR_I+ESAL_ARM_PSR_F)

/* Macro definitions for helping decoding instruction */
#define         ESAL_AR_DBG_ToPsr(x)                  (((x) & ~ESAL_AR_DBG_ARM_PSR_32_IF) | (((x) & ESAL_AR_DBG_ARM_PSR_32_IF) << 20))
#define         ESAL_AR_DBG_PcplusPsr(pc_val, psr_val)((psr_val & ESAL_AR_DBG_ARM_PSR_M32) ? (pc_val) : \
                                                                   (pc_val) | ESAL_AR_DBG_ToPsr(psr_val))

/* Defines ARM shift type */
typedef enum
{
    ESAL_ST_LSL, 
    ESAL_ST_LSR, 
    ESAL_ST_ASR, 
    ESAL_ST_ROR

} ESAL_AR_DBG_SHIFT_TYPE;

/* Define Opcode type */
typedef         UINT32                              ESAL_AR_DBG_OPCODE;

/* Define Register type */
typedef         UINT32                              ESAL_AR_DBG_REG;

#endif  /* ARM_DEBUG_DEFS_H */
