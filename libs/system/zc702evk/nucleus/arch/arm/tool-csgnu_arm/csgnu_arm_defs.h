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
*       csgnu_arm_defs.h
*
*   DESCRIPTION
*
*       This file contains all definitions, structures, etc for the
*       CS GNU for ARM toolset
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

#ifndef         CSGNU_ARM_DEFS_H
#define         CSGNU_ARM_DEFS_H

/* Define required stack pointer alignment for the given toolset.*/
#define         ESAL_TS_REQ_STK_ALIGNMENT               8

/* Define if toolset supports 64-bit data types (long long) */
#define         ESAL_TS_64BIT_SUPPORT                   NU_TRUE

/* Define, in bytes, toolset minimum required alignment for structures */
#define         ESAL_TS_STRUCT_ALIGNMENT                4

/* Size, in bits, of integers for the given toolset / architecture */
#define         ESAL_TS_INTEGER_SIZE                    32

/* Size, in bits, of code pointer for the given toolset / architecture */
#define         ESAL_TS_CODE_PTR_SIZE                   32

/* Size, in bits, of data pointer for the given toolset / architecture */
#define         ESAL_TS_DATA_PTR_SIZE                   32

/* Define if necessary to copy code / data from ROM to RAM */
#define         ESAL_TS_ROM_TO_RAM_COPY_SUPPORT         CFG_NU_OS_KERN_PLUS_CORE_ROM_TO_RAM_COPY

/* Define, in bytes, toolset maximum alignment for data types. */
#define         ESAL_TS_MAX_TYPE_ALIGNMENT              ESAL_TS_REQ_STK_ALIGNMENT

/* Define tool specific type for HUGE and FAR data pointers - these will usually
   be defined to nothing.  Some 16-bit architectures may require these
   to be defined differently to access data across memory pages */
#define         ESAL_TS_HUGE_PTR_TYPE
#define         ESAL_TS_FAR_PTR_TYPE

/* Define if position-independent code / data (PIC/PID) support (if available)
   is enabled.
   NOTE:  This may be required to be set to NU_TRUE when utilizing any
          OS components requiring position-independent code / data */
#define         ESAL_TS_PIC_PID_SUPPORT                 NU_FALSE

/* External variable declarations */
extern UINT32       _ld_bss_start;
extern UINT32       _ld_bss_end;
extern UINT32       _ld_ram_data_start;
extern UINT32       _ld_ram_data_end;
extern UINT32       _ld_rom_data_start;

/* Macros for memory definitions */
#define TOOLSET_BSS_START_ADDR      (VOID *)&_ld_bss_start
#define TOOLSET_BSS_END_ADDR        (VOID *)&_ld_bss_end
#define TOOLSET_BSS_SIZE            ((UINT32)&_ld_bss_end - (UINT32)&_ld_bss_start)
#define TOOLSET_DATA_SRC_ADDR       (VOID *)&_ld_rom_data_start
#define TOOLSET_DATA_DST_ADDR       (VOID *)&_ld_ram_data_start
#define TOOLSET_DATA_SIZE           ((UINT32)&_ld_ram_data_end - (UINT32)&_ld_ram_data_start)

/* This define is used to add quotes to anything passed in */
#define         ESAL_TS_RTE_QUOTES(x)           #x

/* This macro reads the program counter. */
#define         ESAL_TS_RTE_PC_READ()                                               \
                ({                                                                  \
                    VOID * pc_ptr;                                                  \
                    asm(" MOV   %0,pc": "=r" (pc_ptr));                             \
                    pc_ptr;                                                         \
                })

/* This macro writes the stack pointer. */
#define         ESAL_TS_RTE_SP_WRITE(stack_ptr)                                     \
                {                                                                   \
                    /* Set hardware stack pointer to passed in address */           \
                    asm volatile(" MOV     sp, %0"                                  \
                                 : : "r" (stack_ptr) );                             \
                }

/* This macro reads the stack pointer. */
#define         ESAL_TS_RTE_SP_READ()                                               \
                ({                                                                  \
                    VOID * stk_ptr;                                                 \
                    asm(" MOV   %0,sp": "=r" (stk_ptr));                            \
                    stk_ptr;                                                        \
                })

#if defined(__thumb__) && !defined(__ARM_ARCH_7R__) && !defined(__ARM_ARCH_7A__)

/* Must do function call to CPSR read/write functions written in assembly */
VOID            ESAL_TS_RTE_CPSR_CXSF_Read(INT *);
VOID            ESAL_TS_RTE_CPSR_CXSF_Write(INT);
VOID            ESAL_TS_RTE_CPSR_C_Write(UINT32);
#define         ESAL_TS_RTE_CPSR_CXSF_READ      ESAL_TS_RTE_CPSR_CXSF_Read
#define         ESAL_TS_RTE_CPSR_CXSF_WRITE     ESAL_TS_RTE_CPSR_CXSF_Write
#define         ESAL_TS_RTE_CPSR_C_WRITE        ESAL_TS_RTE_CPSR_C_Write

#else

/* This macro reads the current program status register (CPSR - all fields) */
#define         ESAL_TS_RTE_CPSR_CXSF_READ(cpsr_cxsf_ptr)                                   \
                {                                                                           \
                    asm volatile("    MRS     %0, CPSR"                                     \
                                     : "=r" (*(cpsr_cxsf_ptr))                              \
                                     : /* No inputs */ );                                   \
                }

/* This macro writes the current program status register (CPSR - all fields) */
#define         ESAL_TS_RTE_CPSR_CXSF_WRITE(cpsr_cxsf_value)                                \
                {                                                                           \
                    asm volatile("    MSR     CPSR_cxsf, %0"                                \
                                     : /* No outputs */                                     \
                                     : "r" (cpsr_cxsf_value) );                             \
                }

#ifdef __thumb__
/* This macro writes the c (control) bits of the current program status register (CPSR) */
#define         ESAL_TS_RTE_CPSR_C_WRITE(c_bits)                                            \
                {                                                                           \
                    UINT32  __temp;                                                         \
                                                                                            \
                    ESAL_TS_RTE_CPSR_CXSF_READ(&__temp);                                    \
                    __temp &= ~(ESAL_AR_INT_CPSR_MODE_MASK|ESAL_AR_INTERRUPTS_DISABLE_BITS);\
                    __temp |= c_bits;                                                       \
                    asm volatile("   MSR     CPSR_cxsf, %0"                                 \
                                     : /* No outputs */                                     \
                                     : "r"  (__temp) );                                     \
                }
#else

/* This macro writes the c (control) bits of the current program status register (CPSR) */
#define         ESAL_TS_RTE_CPSR_C_WRITE(c_bits)                                    \
                {                                                                   \
                    asm volatile("    MSR     CPSR_c, %0"                           \
                                     : /* No outputs */                             \
                                     : "I"  (c_bits) );                             \
                }

#endif  /* __thumb__ */
#endif  /* __thumb && !__ARM_ARCH_7R__ && !__ARM_ARCH_7A__ */

#if defined(__thumb__) && !defined(__ARM_ARCH_7R__) && !defined(__ARM_ARCH_7A__)

/* This macro writes to a coprocessor register */
#define         ESAL_TS_RTE_CP_WRITE(cp, op1, cp_value, crn, crm, op2)              \
                {                                                                   \
                    asm volatile("    .align                                  \n\t" \
                                      "MOV   r0, pc                           \n\t" \
                                      "BX    r0                               \n\t" \
                                      ".arm                                   \n\t" \
                                      "MCR    " ESAL_TS_RTE_QUOTES(cp) ","          \
                                             #op1                                   \
                                             ", %0, "                               \
                                             ESAL_TS_RTE_QUOTES(crn) ","            \
                                             ESAL_TS_RTE_QUOTES(crm) ","            \
                                             #op2 "                           \n\t" \
                                      "ADD   r0, pc, #1                       \n\t" \
                                      "BX    r0                               \n\t" \
                                      ".thumb                                 \n\t" \
                                    : /* No outputs */                              \
                                    : "r" (cp_value)                                \
                                    : "r0");                                        \
                }

/* This macro reads from a coprocessor register */
#define         ESAL_TS_RTE_CP_READ(cp, op1, cp_value_ptr, crn, crm, op2)           \
                {                                                                   \
                    asm volatile("    .align                                  \n\t" \
                                      "MOV   r0, pc                           \n\t" \
                                      "BX    r0                               \n\t" \
                                      ".arm                                   \n\t" \
                                      "MRC    " ESAL_TS_RTE_QUOTES(cp) ","          \
                                             #op1                                   \
                                             ", %0, "                               \
                                             ESAL_TS_RTE_QUOTES(crn) ","            \
                                             ESAL_TS_RTE_QUOTES(crm) ","            \
                                             #op2"                            \n\t" \
                                      "ADD   r0, pc, #1                       \n\t" \
                                      "BX    r0                               \n\t" \
                                      ".thumb                                 \n\t" \
                                    : "=r" (*(UINT32 *)(cp_value_ptr))              \
                                    : /* No inputs */                               \
                                    : "r0");                                        \
                }

#else

/* This macro writes to a coprocessor register */
#define         ESAL_TS_RTE_CP_WRITE(cp, op1, cp_value, crn, crm, op2)              \
                {                                                                   \
                    asm volatile("    MCR    " ESAL_TS_RTE_QUOTES(cp) ","           \
                                             #op1                                   \
                                             ", %0, "                               \
                                             ESAL_TS_RTE_QUOTES(crn) ","            \
                                             ESAL_TS_RTE_QUOTES(crm) ","            \
                                             #op2                                   \
                                    : /* No outputs */                              \
                                    : "r" (cp_value));                              \
                }

/* This macro reads from a coprocessor register */
#define         ESAL_TS_RTE_CP_READ(cp, op1, cp_value_ptr, crn, crm, op2)           \
                {                                                                   \
                    asm volatile("    MRC    " ESAL_TS_RTE_QUOTES(cp) ","           \
                                             #op1                                   \
                                             ", %0, "                               \
                                             ESAL_TS_RTE_QUOTES(crn) ","            \
                                             ESAL_TS_RTE_QUOTES(crm) ","            \
                                             #op2                                   \
                                        : "=r" (*(UINT32 *)(cp_value_ptr))          \
                                        : /* No inputs */ );                        \
                }

#endif  /* __thumb && !__ARM_ARCH_7R__ && !__ARM_ARCH_7A__ */

/* This macro executes a ISB instruction */
#define         ESAL_TS_RTE_ISB_EXECUTE()                                           \
                {                                                                   \
                    asm volatile("    ISB");                                        \
                }

/* This macro executes a DSB instruction */
#define         ESAL_TS_RTE_DSB_EXECUTE()                                           \
                {                                                                   \
                    asm volatile("    DSB");                                        \
                }

/* This macro executes a NOP instruction */
#define         ESAL_TS_RTE_NOP_EXECUTE()                                           \
                {                                                                   \
                    asm volatile("    NOP");                                        \
                }

/* This macro executes a WFI instruction */
#define         ESAL_TS_RTE_WFI_EXECUTE()                                           \
                {                                                                   \
                    asm volatile("    WFI");                                        \
                }

/* This macro executes a breakpoint instruction
   NOTE:  This instruction is only usable by ARM v5 cores. */
#define         ESAL_TS_RTE_BRK_EXECUTE(brk_point_val)                              \
                {                                                                   \
                    asm volatile("    BKPT    %0"                                   \
                             : /* No Outputs */                                     \
                             : "n" (brk_point_val) );                               \
                }

/* This macro executes an ARM undefined (breakpoint) instruction. */
#define         ESAL_TS_RTE_UNDEF_BRK_EXECUTE()                                     \
                {                                                                   \
                    asm volatile("   .long    0xE7FDDEFE");                         \
                }

/* This macro executes SMI for the OMAP35xx device.
   NOTE:  The ROM code provides three primitive services. These services are
          implemented in monitor mode and do not use any resources outside the
          MPU subsystem. The services are described below. To call a service,
          a register r12 must be set to service ID and the SMI instruction must
          be executed.
            - r12=1: To use the L2 cache, all L2 line data must be invalidated
                   through the CP15 registers. This service invalidates the
                   entire L2 cache and must be performed after a POR or a loss
                   of L2 cache after reset. This register can also be read.
            - r12=2: This service writes the value of the central processing unit (CPU)
                     register R0 in the L2 cache auxiliary control register. This
                     register can also be read.
            - r12=3: This service writes the value of the CPU register R0 in the
                     auxiliary control. This register can also be read.
                     For more information about ARM L2 cache and registers, see
                     the Cortex-A8 Technical Reference Manual. For more information
                     about ARM CP15 registers, see the ARM Architecture Reference Manual. */
#define         ESAL_TS_RTE_SMI_EXECUTE(r0_val, r12_op)                             \
                {                                                                   \
                    asm volatile("    MOV     r0,  %0\n\t"                          \
                                 "    MOV     r12, %1\n\t"                          \
                                 "    SMC     #0"                                   \
                                 : /* No Output */                                  \
                                 : "r" (r0_val), "r" (r12_op)                       \
                                 : "r0", "r12");                                    \
                    ESAL_TS_RTE_ISB_EXECUTE();                                      \
                }

#if (ESAL_TS_PIC_PID_SUPPORT == NU_TRUE)

/* This macro sets the PIC/PID base address register */
#define         ESAL_TS_RTE_PIC_PID_BASE_SET(pic_base, pid_base)                    \
                {                                                                   \
                    /* Access unused param */                                       \
                    NU_UNUSED_PARAM(pic_base);                                    \
                                                                                    \
                    /* Set the PIC/PID register (r9) */                             \
                    asm volatile(" MOV     r9, %0"                                  \
                                 : : "r" (pid_base) );                              \
                }

/* This macro gets the PIC/PID base address register */
#define         ESAL_TS_RTE_PIC_PID_BASE_GET(pic_base_ptr, pid_base_ptr)            \
                {                                                                   \
                    /* Access unused param */                                       \
                    NU_UNUSED_PARAM(pic_base_ptr);                                \
                                                                                    \
                    /* Read the PIC/PID register (r9) */                            \
                    asm volatile(" MOV   %0,r9": "=r" (*pid_base_ptr));             \
                }

#endif  /* ESAL_TS_PIC_PID_SUPPORT == NU_TRUE */

/* This macro gets the current function's return address, see GCC manual for argument usage */
#define         ESAL_GET_RETURN_ADDRESS(level) __builtin_return_address(level)

/* This macro marks a symbol declaration as weakly linked */
#define         ESAL_TS_WEAK_REF(decl) decl __attribute((weak))

/* This macro marks a symbol definition as weakly linked */
#define         ESAL_TS_WEAK_DEF(decl) __attribute((weak)) decl

/* This macro returns the passed value */
#define         ESAL_TS_NO_RETURN(return_val) return(return_val)

/* This macro generates deprecation warnings */
#define         ESAL_TS_RTE_DEPRECATED __attribute__((deprecated))

/* This macro places a compiler memory barrier to ensure read / write commands
 * cannot be re-ordered around it */
#define         ESAL_TS_RTE_COMPILE_MEM_BARRIER()   asm volatile("" ::: "memory")

/* This macro returns the 2-byte value with the order of the bytes reversed */
#define         ESAL_TS_BYTE_SWAP16(temp16) __builtin_bswap16(temp16)

/* This macro returns the 4-byte value with the order of the bytes reversed */
#define         ESAL_TS_BYTE_SWAP32(temp32) __builtin_bswap32(temp32)

/* This macro returns the 8-byte value with the order of the bytes reversed */
#define         ESAL_TS_BYTE_SWAP64(temp64) __builtin_bswap64(temp64)

#endif  /* CSGNU_ARM_DEFS_H */

