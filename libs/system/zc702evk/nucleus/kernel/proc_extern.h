/*************************************************************************
*
*               Copyright Mentor Graphics Corporation 2013
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
*************************************************************************/
/*************************************************************************
*
*   FILE NAME
*
*       proc_extern.h
*
*   COMPONENT
*
*       Nucleus Processes
*
*   DESCRIPTION
*
*       Nucleus Processes API.
*
*   DATA STRUCTURES
*
*       None
*
*   FUNCTIONS
*
*       NU_EXPORT_SYMBOL
*       NU_SYMBOL_COMPONENT
*       NU_KEEP_COMPONENT_SYMBOLS
*
*************************************************************************/

#ifndef PROC_EXTERN_H
#define PROC_EXTERN_H

#include "nucleus.h"
#include "kernel/plus_core.h"

/****************************************/
/* Symbol Table Export Macros           */
/****************************************/

#ifdef CFG_NU_OS_KERN_PROCESS_CORE_ENABLE

/* Symbol Entry - An entry in a Nucleus symbol table. */
typedef struct NU_SYMBOL_ENTRY_STRUCT
{
    VOID *          symbol_address;
    const CHAR *    symbol_name;

} NU_SYMBOL_ENTRY;

/* No trampoline for process symbols and kernel data */
#define NU_SYM_TRAMP_PROCESS_DATA(symbol_name, export_name)

/* Use actual symbol address in process symbol table and kernel symbol table for data */
#define NU_SYM_ADDR_PROCESS_DATA(symbol_name, export_name)  symbol_name

/* Kernel symbols (functions) use trampoline */
#define NU_SYM_TRAMP_KERNEL_FUNC(symbol_name, export_name)                                                           \
        /* Create trampoline function for this symbol */                                                \
        static void __nutramp_##export_name(void) __attribute__((section("nutramp"),naked));            \
        static void __nutramp_##export_name(void)                                                       \
        {                                                                                               \
            /* Perform architecture specific trampoline code */                                         \
            PROC_AR_TRAMPOLINE(symbol_name);                                                            \
        }

/* Use trampoline symbol address in kernel process symbol table */
#define NU_SYM_ADDR_KERNEL_FUNC(symbol_name, export_name)   __nutramp_##export_name

/* This macro, ultimately, exports a symbol.  There are 3 different scenarios needing to be accounted for:
       1.  Process functions and data - all symbols get put into the process symbol table "as is"
       2.  Kernel data - all kernel data symbols get put into the kernel symbol table "as is"
       3.  Kernel functions - all kernel function symbols get put into the kernel symbol table using a
                              "trampoline" function that switches modes (user/kernel) and performs MMU ops */
#define PROC_EXPORT_SYMBOL(symbol_name, export_name, trampoline_macro, symbol_addr_macro)                            \
            /* First, extern the symbol */                                                              \
            extern typeof(symbol_name) symbol_name;                                                     \
                                                                                                        \
            /* Place a "stringified" version of the symbol into a string table */                       \
            static const char __nusymstr_##export_name[]                                                \
                __attribute__((section("nusymstr"), aligned(1))) = #export_name;                        \
                                                                                                        \
            /* Trampoline (if needed) */                                                                \
            trampoline_macro(symbol_name, export_name);                                                              \
                                                                                                        \
            /* Place symbol entry into symbol table */                                                  \
            static const NU_SYMBOL_ENTRY __nusymtab_##export_name                                       \
                __attribute__((section("nusymtab"), used)) =                                            \
                {(VOID *)&symbol_addr_macro(symbol_name, export_name), __nusymstr_##export_name }

/* Thic macro exports a symbol for use by kernel mode processes. It uses a separate symbol table and
   bypasses the trampoline. */
#define PROC_EXPORT_KSYMBOL(symbol_name, export_name)                                                                \
            /* First, extern the symbol */                                                              \
            extern typeof(symbol_name) symbol_name;                                                     \
                                                                                                        \
            /* Place a "stringified" version of the symbol into a string table */                       \
            static const char __nuksymstr_##export_name[]                                               \
                __attribute__((section("nuksymstr"), aligned(1))) = #export_name;                       \
                                                                                                        \
            /* Place symbol entry into symbol table */                                                  \
            static const NU_SYMBOL_ENTRY __nuksymtab_##export_name                                      \
                __attribute__((section("nuksymtab"), used)) =                                           \
                {(VOID *)&(symbol_name), __nuksymstr_##export_name }

#if (!defined(NU_PROCESS) || (defined(NU_PROCESS) && defined(NU_KERNEL_MODE)))

#if ((CFG_NU_OS_KERN_PROCESS_CORE_SUP_USER_MODE == NU_TRUE) || defined(CFG_NU_OS_KERN_PROCESS_MEM_MGMT_ENABLE))

/* Macro to export functions when mode switching or MMU support enabled */
#define NU_EXPORT_SYMBOL_ALIAS(symbol, alias)                                                           \
            PROC_EXPORT_SYMBOL(symbol, alias, NU_SYM_TRAMP_KERNEL_FUNC, NU_SYM_ADDR_KERNEL_FUNC);       \
            PROC_EXPORT_KSYMBOL(symbol, alias)

/* Macro to export functions for other kernel mode processes. This uses no trampoline regardless of enablement of mode
   switching or MMU support. */
#define NU_EXPORT_KSYMBOL_ALIAS(symbol, alias)  PROC_EXPORT_KSYMBOL(symbol, alias)

#else

/* Macro used by process to export functions and data and by kernel to export functions when mode switching & MMU support off
   NOTE:  Since supervisor / user mode switching is off, no trampoline is used for kernel functions */
#define NU_EXPORT_SYMBOL_ALIAS(symbol, alias)   PROC_EXPORT_SYMBOL(symbol, alias, NU_SYM_TRAMP_PROCESS_DATA, NU_SYM_ADDR_PROCESS_DATA)

/* With mode switching disabled, kernel mode symbol table is not used. */
#define NU_EXPORT_KSYMBOL_ALIAS(symbol, alias)  NU_EXPORT_SYMBOL_ALIAS(symbol, alias)

#endif

/* Macro to export data symbols. */
#define NU_EXPORT_DATA_SYMBOL_ALIAS(symbol, alias)                                                      \
            PROC_EXPORT_SYMBOL(symbol, alias, NU_SYM_TRAMP_PROCESS_DATA, NU_SYM_ADDR_PROCESS_DATA);     \
            PROC_EXPORT_KSYMBOL(symbol, alias)

/* Macro to export data symbols to kernel-mode processes only. */
#define NU_EXPORT_DATA_KSYMBOL_ALIAS(symbol, alias)                                                     \
            PROC_EXPORT_KSYMBOL(symbol, alias)

#else

/* Non-kernel mode processes need not use trampoline since they are already behind trampoline. */
#define NU_EXPORT_SYMBOL_ALIAS(symbol, alias)   PROC_EXPORT_SYMBOL(symbol, alias, NU_SYM_TRAMP_PROCESS_DATA, NU_SYM_ADDR_PROCESS_DATA)

/* Macro to export data symbols. User-mode processes do not have the kernel-mode symbol table so
   the symbol is exported only to user-mode symbol table. */
#define NU_EXPORT_DATA_SYMBOL_ALIAS(symbol, alias)                                                      \
            PROC_EXPORT_SYMBOL(symbol, alias, NU_SYM_TRAMP_PROCESS_DATA, NU_SYM_ADDR_PROCESS_DATA);

#endif

/* Regular export macros that use symbol name as the export name */
#define NU_EXPORT_SYMBOL(symbol)                NU_EXPORT_SYMBOL_ALIAS(symbol, symbol)
#define NU_EXPORT_KSYMBOL(symbol)               NU_EXPORT_KSYMBOL_ALIAS(symbol, symbol)
#define NU_EXPORT_DATA_SYMBOL(symbol)           NU_EXPORT_DATA_SYMBOL_ALIAS(symbol, symbol)
#define NU_EXPORT_DATA_KSYMBOL(symbol)          NU_EXPORT_DATA_KSYMBOL_ALIAS(symbol, symbol)

/* Symbol Component - Used at the start of a symbol file to define the component in which the
                      symbols are defined.  This is used in conjunction with NU_KEEP_COMPONENT_SYMBOLS. */
#define NU_SYMBOL_COMPONENT(component_name)                                                             \
        /* Define global for this component */                                                          \
        UINT8 nusymtab_keep_##component_name;

/* Keep Component Symbols - This macro will ensure the symbols for the given component are included as
                            long as the NU_SYMBOL_COMPONENT macro is used as well. */
#define NU_KEEP_COMPONENT_SYMBOLS(component_name)                                                       \
        /* Extern the global associated with this component's symbols */                                \
        extern UINT8 nusymtab_keep_##component_name;                                                    \
                                                                                                        \
        /* Assign a value to the global */                                                              \
        nusymtab_keep_##component_name = 1;

/* Return NU_SUCCESS to these now unused macros */
#define NU_BIND_TASK_TO_KERNEL(task)    NU_SUCCESS
#define NU_BIND_HISR_TO_KERNEL(hisr)    NU_SUCCESS

/* Process exit codes specific to Nucleus (based on 128 + POSIX signal value that is closest to what Nucleus is doing) */
#include    <signal.h>
#define     EXIT_ABORT              (128 + SIGABRT) /* Exit code returned when abort() is called */
#define     EXIT_FPU_ERROR          (128 + SIGFPE)  /* Exit code returned when a floating point error occurs */
#define     EXIT_ILLEGAL_INST       (128 + SIGILL)  /* Exit code returned when illegal instruction executed */
#define     EXIT_KILL               (128 + SIGKILL) /* Exit code returned when NU_Kill() is called */
#define     EXIT_BUS_ERROR          (128 + SIGBUS)  /* Exit code returned for bus error */
#define     EXIT_INVALID_MEM        (128 + SIGSEGV) /* Exit code returned for invalid memory access */
#define     EXIT_STOP               (128 + SIGSTOP) /* Exit code returned when NU_Stop() is called indirectly via shell or symbol usage */
#define     EXIT_CONTINUE           (128 + SIGCONT) /* If returned by main(), root task is suspended instead of doing normal exit() */

/* Process action definitions */
#define     PROC_CMD_START          0               /* Start the process */
#define     PROC_CMD_STOP           1               /* Stop the process */

/* Process names will be file names */
#define     PROC_NAME_LENGTH        CFG_NU_OS_KERN_PROCESS_CORE_MAX_NAME_LENGTH

/*******************************************************/
/* Structure for process information                   */
/*******************************************************/
typedef struct
{
    INT             pid;
    CHAR            name[PROC_NAME_LENGTH];
    INT             state;
    VOID *          load_addr;
    VOID            (*entry_addr)(INT, VOID *);
    INT             exit_code;
    BOOLEAN         kernel_mode;

} NU_PROCESS_INFO;

/*******************************************************/
/* Structure for memory map information                */
/*******************************************************/
typedef struct
{
    CHAR            name[NU_MAX_NAME];
    VOID           *phys_base;
    VOID           *virt_base;
    UNSIGNED        size;
    UNSIGNED        attributes;
} NU_MEMORY_MAP_INFO;

/*******************************************************/
/* Structure for exception handling                    */
/*******************************************************/
typedef struct
{
    INT             pid;                    /* Process ID */
    NU_TASK        *task;                   /* Offending thread */
    VOID           *address;                /* Address where error occurred */
    VOID           *return_address;         /* Point of exception and return address */
    UNSIGNED        type;                   /* Type of error that occurred */
    BOOLEAN         interrupt_context;      /* Did exception occur in interrupt context? */
    BOOLEAN         kernel_process;         /* Did exception occur in kernel process / module? */
    VOID           *exception_information;  /* Additional information such as stack frame (arch specific) */
} NU_PROCESS_EXCEPTION;

/* Return values from exception handler */
#define     NU_PROC_SCHEDULE            0   /* Returning this from Process_Exception_Handler
                                               will have the system return to the scheduler
                                               when exception handling is complete */
#define     NU_PROC_RESUME_TASK         1   /* Returning this from Process_Exception_Handler
                                               will have the system return to the point of
                                               exception */
#define     NU_PROC_UNRECOVERABLE       2   /* Returning this from Process_Exception_Handler
                                               will result in execution going to the system error
                                               handler trap / while(1) */

/*******************************************************/
/* Defines that will only work properly with processes */
/*******************************************************/

/* Public interfaces */
STATUS      NU_Load(CHAR * name, INT * pid, VOID * load_addr, VOID * extension, UNSIGNED suspend);
STATUS      NU_Start(INT pid, VOID * args, UNSIGNED suspend);
STATUS      NU_Stop(INT pid, INT exit_code, UNSIGNED suspend);
STATUS      NU_Unload(INT pid, UNSIGNED suspend);
STATUS      NU_Kill(INT pid, UNSIGNED suspend);
STATUS      NU_Symbol(INT pid, CHAR * sym_name, VOID ** sym_addr);
STATUS      NU_Symbol_Close (INT pid, BOOLEAN stop, BOOLEAN * stopped);
INT         NU_Getpid(VOID);
STATUS      NU_Get_Exit_Code(INT pid, INT * exit_code);
STATUS      NU_Memory_Map(INT *mem_id, CHAR *name, VOID *phys_addr, VOID **actual_addr, UNSIGNED size, UNSIGNED options);
STATUS      NU_Memory_Unmap(INT mem_id);
STATUS      NU_Memory_Get_ID(INT *mem_id, VOID *phys_addr, CHAR *name);
STATUS      NU_Memory_Share(INT *mem_id, INT source_id, CHAR *name);
UNSIGNED    NU_Established_Processes(VOID);
STATUS      NU_Processes_Information(UNSIGNED * max_processes, NU_PROCESS_INFO info_array[]);
STATUS      NU_Process_Information(INT pid, NU_PROCESS_INFO * info);
STATUS      NU_Memory_Map_Information(INT mem_id, NU_MEMORY_MAP_INFO *info);

NU_WEAK_REF(UNSIGNED Process_Exception_Handler(NU_PROCESS_EXCEPTION *exception_info));

/* Structure definition for load extension */
typedef struct
{
    UNSIGNED    heap_size;      /* size (in bytes) of process memory pool */
    UNSIGNED    stack_size;     /* size (in bytes) of process root thread stack */
    BOOLEAN     kernel_mode;    /* indicates if process loaded in kernel mode */

} NU_LOAD_EXTENSION;

/* Definitions */
#define NU_LOAD_DYNAMIC                 (VOID *)0xFFFFFFFF

/* Kernel process ID */
#define PROC_KERNEL_ID                  0

/* Memory region attributes */
#define NU_MEM_READ                     (1 << 0)
#define NU_MEM_WRITE                    (1 << 1)
#define NU_MEM_EXEC                     (1 << 2)
#define NU_SHARE_READ                   (1 << 3)
#define NU_SHARE_WRITE                  (1 << 4)
#define NU_SHARE_EXEC                   (1 << 5)
#define NU_CACHE_INHIBIT                (1 << 6)
#define NU_CACHE_WRITE_THROUGH          (1 << 7)
#define NU_CACHE_NO_COHERENT            (1 << 8)

#define NU_MEMORY_UNDEFINED             (VOID *)0xFFFFFFFF

/* Internal global functions */
STATUS  PROC_Kernel_Initialize(VOID);
STATUS  PROC_Get_Memory_Pool(NU_MEMORY_POOL **memory_pool_ptr);
STATUS  PROC_Bind_Task(NU_TASK *task_ptr);
STATUS  PROC_Unbind_Task(NU_TASK *task_ptr);
STATUS  PROC_Bind_HISR(NU_HISR *hisr_ptr);
VOID    PROC_Stopped(VOID);
BOOLEAN PROC_Get_Abort_Flag(VOID);

#ifndef     NU_PROCESS

#include    "process/core/arch_proc_mode.h"

#endif  /* !NU_PROCESS */

/* Define process set-up function */
VOID            PROC_Schedule(VOID);
#define         NU_PROCESS_SETUP()              PROC_Schedule()

#else /* CFG_NU_OS_KERN_PROCESS_CORE_ENABLE */

/* Versions of various macros used if process support disabled. */
#define NU_EXPORT_SYMBOL(symbol_name)
#define NU_EXPORT_KSYMBOL(symbol_name)
#define NU_EXPORT_DATA_SYMBOL(symbol_name)
#define NU_EXPORT_DATA_KSYMBOL(symbol_name)
#define NU_EXPORT_SYMBOL_ALIAS(symbol_name)
#define NU_EXPORT_KSYMBOL_ALIAS(symbol_name)
#define NU_EXPORT_DATA_SYMBOL_ALIAS(symbol_name)
#define NU_EXPORT_DATA_KSYMBOL_ALIAS(symbol_name)
#define NU_SYMBOL_COMPONENT(component_name)
#define NU_KEEP_COMPONENT_SYMBOLS(component_name)
#define NU_PROCESS_SETUP()
#define NU_Getpid()     0

#endif  /* CFG_NU_OS_KERN_PROCESS_CORE_ENABLE */

#endif  /* PROC_EXTERN_H */
