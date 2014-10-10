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
*       arm_defs.h
*
*   DESCRIPTION
*
*       This file contains all definitions, structures, etc for the
*       base ARM architecture.
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

#ifndef         ARM_DEFS_H
#define         ARM_DEFS_H

/* Define if the OS timer is part of the architecture (NU_TRUE)
   NOTE:  The OS timer can be located at only one of the following levels of
          abstraction: the processor level or the architecture level. 
          If ESAL_AR_OS_TIMER_USED is NU_TRUE, the OS timer is contained
          within the architecture.  If ESAL_AR_OS_TIMER_USED is NU_FALSE,
          the OS timer is contained within the processor component. */
#define         ESAL_AR_OS_TIMER_USED                   NU_FALSE

/* Define if architecture supports unaligned 16-bit accesses to memory. */
#define         ESAL_AR_UNALIGNED_16BIT_SPT             NU_FALSE

/* Define if architecture supports unaligned 32-bit accesses to memory. */
#define         ESAL_AR_UNALIGNED_32BIT_SPT             NU_FALSE

/* Define number of accesses required to read or write a pointer */
#define         ESAL_AR_PTR_ACCESS                      1

/* Define number of accesses required to read or write a 32-bit value */
#define         ESAL_AR_32BIT_ACCESS                    1

/* Size, in bytes, of architecture system stack.  This stack will be
   utilized when servicing interrupts. */
#define         ESAL_AR_SYSTEM_STACK_SIZE               CFG_NU_OS_ARCH_ARM_COM_SYSTEM_STACK_SIZE

#if (CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT == 2)

/* Define number of single-precision floating point registers */
#define         ESAL_AR_STK_NUM_FPU_REGS                64

#else

/* Define number of single-precision floating point registers */
#define         ESAL_AR_STK_NUM_FPU_REGS                32

#endif /* (CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT == 2) */

/* Define number of single-precision floating point registers required to
   be saved within a toolset stack frame (s16-s31) */
#define         ESAL_TS_STK_NUM_FPU_REGS                16

/* Define bit mask used to determine if THUMB state for a given code pointer */
#define         ESAL_AR_STK_THUMB_MASK                  ESAL_GE_MEM_32BIT_SET(0)

/* Define stack frame structure for minimum architecture registers required
   to be saved in order to enter a C environment during in interrupt / exception.
   These registers are the "scratch" registers that will not be preserved across 
   a function call boundary and any interrupt state registers that must preserved
   to allow interrupt nesting. */
typedef struct
{
    UINT32              r0;
    UINT32              r1;
    UINT32              r2;
    UINT32              r3;
    UINT32              r12;
    UINT32              spsr;    
    UINT32              lr;
    UINT32              rtn_address;

} ESAL_AR_STK_MIN;

/* Define stack frame structure for the architecture supported. 
   This stack frame contains all registers that must be preserved
   across an (unsolicited) interrupt context switch.
   NOTE:  This stack frame includes the minimum stack frame
          defined above AND all other registers for the given
          architecture. */
typedef struct  ESAL_AR_STK_STRUCT
{
    UINT32              stack_type;
    UINT32              r4;
    UINT32              r5;
    UINT32              r6;
    UINT32              r7;
    UINT32              r8;
    UINT32              r9;
    UINT32              r10;
    UINT32              r11;

#if (CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT > 0)

    UINT32              fpscr;
    UINT32              s[ESAL_AR_STK_NUM_FPU_REGS];

#endif  /* CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT > 0 */

    ESAL_AR_STK_MIN     min_stack;

} ESAL_AR_STK;

/* Define stack frame structure for the toolset / architecture supported. */
typedef struct  ESAL_TS_STK_STRUCT
{
    UINT32          stack_type;
    UINT32          r4;
    UINT32          r5;
    UINT32          r6;
    UINT32          r7;
    UINT32          r8;
    UINT32          r9;
    UINT32          r10;
    UINT32          r11;
    UINT32          rtn_address;

#if (CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT > 0)

    UINT32          fpscr;
    UINT32          s[ESAL_TS_STK_NUM_FPU_REGS];

#endif  /* CFG_NU_OS_ARCH_ARM_COM_FPU_SUPPORT > 0 */

} ESAL_TS_STK;

/* Coprocessor registers */
#define         ESAL_TS_RTE_CP0                 p0
#define         ESAL_TS_RTE_CP1                 p1
#define         ESAL_TS_RTE_CP2                 p2
#define         ESAL_TS_RTE_CP3                 p3
#define         ESAL_TS_RTE_CP4                 p4
#define         ESAL_TS_RTE_CP5                 p5
#define         ESAL_TS_RTE_CP6                 p6
#define         ESAL_TS_RTE_CP7                 p7
#define         ESAL_TS_RTE_CP8                 p8
#define         ESAL_TS_RTE_CP9                 p9
#define         ESAL_TS_RTE_CP10                p10
#define         ESAL_TS_RTE_CP11                p11
#define         ESAL_TS_RTE_CP12                p12
#define         ESAL_TS_RTE_CP13                p13
#define         ESAL_TS_RTE_CP14                p14
#define         ESAL_TS_RTE_CP15                p15

/* CRn and CRm register values */
#define         ESAL_TS_RTE_C0                  c0
#define         ESAL_TS_RTE_C1                  c1
#define         ESAL_TS_RTE_C2                  c2
#define         ESAL_TS_RTE_C3                  c3
#define         ESAL_TS_RTE_C4                  c4
#define         ESAL_TS_RTE_C5                  c5
#define         ESAL_TS_RTE_C6                  c6
#define         ESAL_TS_RTE_C7                  c7
#define         ESAL_TS_RTE_C8                  c8
#define         ESAL_TS_RTE_C9                  c9
#define         ESAL_TS_RTE_C10                 c10
#define         ESAL_TS_RTE_C11                 c11
#define         ESAL_TS_RTE_C12                 c12
#define         ESAL_TS_RTE_C13                 c13
#define         ESAL_TS_RTE_C14                 c14
#define         ESAL_TS_RTE_C15                 c15

/* Size, in bytes, of architecture exception stack.  This stack will be
   utilized when servicing exceptions. */
#define         ESAL_AR_EXCEPTION_STACK_SIZE            2048

/* Define if interrupt servicing initialization is required at the 
   architecture level. */
#define         ESAL_AR_ISR_INIT_REQUIRED               NU_TRUE

/* Define if architecture mandates that all interrupt handlers perform a
   "return from interrupt" (RTI) instruction in order for the hardware to
   correctly restore the state of execution to the pre-interrupt condition.
   NOTE:  Most architectures allow the state of execution to be restored 
          without needing to perform an RTI.  In most cases, this will be set 
          to NU_FALSE */
#define         ESAL_AR_ISR_RTI_MANDATORY               NU_FALSE

/* Define bit values for the architecture's status register / machine state register /
   etc that are used to enable and disable interrupts for the given architecture. */
#define         ESAL_AR_INTERRUPTS_DISABLE_BITS         0x000000C0
#define         ESAL_AR_INTERRUPTS_ENABLE_BITS          0x00000000

/* CPSR bit defines / masks */
#define         ESAL_AR_INT_CPSR_THUMB                  ESAL_GE_MEM_32BIT_SET(5)
#define         ESAL_AR_INT_CPSR_MODE_MASK              0x0000001F
#define         ESAL_AR_INT_CPSR_SYS_MODE               0x0000001F
#define         ESAL_AR_INT_CPSR_IRQ_MODE               0x00000012
#define         ESAL_AR_INT_CPSR_FIQ_MODE               0x00000011
#define         ESAL_AR_INT_CPSR_SUP_MODE               0x00000013
#define         ESAL_AR_INT_CPSR_E_BIT                  0x00000200 
#define         ESAL_AR_INT_CPSR_IRQ_BIT                ESAL_GE_MEM_32BIT_SET(7)
#define         ESAL_AR_INT_CPSR_FIQ_BIT                ESAL_GE_MEM_32BIT_SET(6)
#define         ESAL_AR_INT_CPSR_SYS_DISABLED           (ESAL_AR_INT_CPSR_SYS_MODE |     \
                                                        ESAL_AR_INTERRUPTS_DISABLE_BITS)

/* Defines used to specify an interrupt source type as FIQ or IRQ.  This value
   can be bitwise OR'd with the priority field when enabling an interrupt source via
   the ESAL_GE_INT_Enable API to specify if an interrupt will be routed as an FIQ or IRQ.
   The default for all interrupt sources is IRQ routed (interrupt will be IRQ routed if
   no value is bitwise OR'd with the priority).
   NOTE:  This operation is not supported by all ARM processor implementations - some 
          processors allow any interrupt source to be either FIQ or IRQ while other
          do not allow this functionality. */
#define         ESAL_AR_INT_IRQ_ROUTED                  (INT)0x00001000
#define         ESAL_AR_INT_FIQ_ROUTED                  (INT)0x00002000

/* Define ESAL interrupt vector IDs for this architecture.
   These IDs match up with architecture interrupts.
   Values correspond to the index of entries in ESAL_GE_ISR_Interrupt_Handler[].
   Names are of the form ESAL_AR_<Name>_INT_VECTOR_ID, where <Name> comes
   directly from the hardware documentation */
#define         ESAL_AR_IRQ_INT_VECTOR_ID               0
#define         ESAL_AR_FIQ_INT_VECTOR_ID               1

/* Define the last ESAL interrupt vector ID for this architecture + 1 */
#define         ESAL_AR_INT_VECTOR_ID_DELIMITER         (ESAL_AR_FIQ_INT_VECTOR_ID + 1)

/* Define ESAL exception vector IDs for the architecture.
   These IDs match up with architecture exceptions.
   Values correspond to the index of entries in ESAL_GE_ISR_Exception_Handler[].
   Names are of the form ESAL_AR_<Name>_EXCEPT_VECTOR_ID, where <Name> comes
   directly from the hardware documentation */
#define         ESAL_AR_UNDEF_EXCEPT_VECTOR_ID          0
#define         ESAL_AR_SWI_EXCEPT_VECTOR_ID            1
#define         ESAL_AR_PREFETCH_EXCEPT_VECTOR_ID       2
#define         ESAL_AR_DATA_EXCEPT_VECTOR_ID           3

/* Define the last ESAL exception vector ID for this architecture + 1 */
#define         ESAL_AR_EXCEPT_VECTOR_ID_DELIMITER      (ESAL_AR_DATA_EXCEPT_VECTOR_ID + 1)

/* Define variable(s) required to save / restore architecture interrupt state.
   These variable(s) are used in conjunction with the ESAL_AR_INT_ALL_DISABLE() and
   ESAL_AR_INT_ALL_RESTORE() macros to hold any data that must be preserved in
   order to allow these macros to function correctly. */
#define         ESAL_AR_INT_CONTROL_VARS                INT  esal_ar_int_tmp;

/* This macro locks out interrupts and saves the current
   architecture status register / state register to the specified
   address.  This function does not attempt to mask any bits in
   the return register value and can be used as a quick method
   to guard a critical section.
   NOTE:  This macro is used in conjunction with ESAL_AR_INT_ALL_RESTORE
          defined below and ESAL_AR_INT_CONTROL_VARS defined above. */
#define         ESAL_AR_INT_ALL_DISABLE()                                       \
                {                                                               \
                    ESAL_TS_RTE_CPSR_CXSF_READ(&esal_ar_int_tmp);               \
                    ESAL_TS_RTE_CPSR_C_WRITE(ESAL_AR_INT_CPSR_SYS_DISABLED);    \
                }

/* This macro restores the architecture status / state register
   used to lockout interrupts to the value provided.  The
   intent of this function is to be a fast mechanism to restore the
   interrupt level at the end of a critical section to its
   original level.
   NOTE:  This macro is used in conjunction with ESAL_AR_INT_ALL_DISABLE
          and ESAL_AR_INT_CONTROL_VARS defined above. */
#define         ESAL_AR_INT_ALL_RESTORE()                                       \
                {                                                               \
                    ESAL_TS_RTE_CPSR_CXSF_WRITE(esal_ar_int_tmp);               \
                }

/* This macro locks-out interrupts but doesn't save the status
   register / control register value. */
#define         ESAL_AR_INT_FAST_ALL_DISABLE()                                  \
                {                                                               \
                    ESAL_TS_RTE_CPSR_C_WRITE(ESAL_AR_INT_CPSR_SYS_DISABLED);    \
                }

/* This macro unlocks interrupts but doesn't save the status
   register / control register value. */
#define         ESAL_AR_INT_FAST_ALL_ENABLE()                                   \
                {                                                               \
                    ESAL_TS_RTE_CPSR_C_WRITE(ESAL_AR_INT_CPSR_SYS_MODE);        \
                }

/* This macro sets the interrupt related bits in the status register / control
   register to the specified value. */
#define         ESAL_AR_INT_BITS_SET(set_bits)                                  \
                {                                                               \
                    INT     tmp_val;                                            \
                                                                                \
                    ESAL_TS_RTE_CPSR_CXSF_READ(&tmp_val);                       \
                    tmp_val &= ~ESAL_AR_INTERRUPTS_DISABLE_BITS;                \
                    tmp_val |= set_bits;                                        \
                    ESAL_TS_RTE_CPSR_CXSF_WRITE(tmp_val);                       \
                }

/* This macro gets the interrupt related bits from the status register / control
   register. */
#define         ESAL_AR_INT_BITS_GET(get_bits_ptr)                              \
                {                                                               \
                    INT     tmp_val;                                            \
                                                                                \
                    ESAL_TS_RTE_CPSR_CXSF_READ(&tmp_val);                       \
                    tmp_val &= ESAL_AR_INTERRUPTS_DISABLE_BITS;                 \
                    *get_bits_ptr = tmp_val;                                    \
                }

/* Vector table address for ARM when running from ROM */
#define         ESAL_AR_ISR_VECTOR_TABLE_DEST_ADDR_ROM      0x00000000

/* Size of ARM vector table (in bytes) */
#define         ESAL_AR_ISR_VECTOR_TABLE_SIZE               64

/* Number of 32-bit registers saved on interrupt stack */
#define         ESAL_AR_ISR_STACK_SIZE                      3

/* Size, in bytes, of supervisor stack */
#define         ESAL_AR_ISR_SUP_STACK_SIZE                  256

/* Define breakpoint instruction used for XScale 
   processors (after vector table installation) */
#define         ESAL_AR_ISR_XSCALE_VECT_INSTALL_BRK         0x1234

/* Determines where the ISR nesting counter is incremented.  
   When set to 0 the increment occurs in assembly files, when 
   set to 1 the increment will occur in c files. */
#define         ESAL_AR_ISR_INCREMENT_IN_C                  NU_TRUE


#endif  /* ARM_DEFS_H */

