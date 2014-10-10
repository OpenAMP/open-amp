/***********************************************************************
*
*            Copyright 2011 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
************************************************************************

************************************************************************
*
*   DESCRIPTION
*
*       This file contains external data structures and definitions of
*       all PLUS core components.
*
***********************************************************************/

#ifndef PLUS_CORE_H
#define PLUS_CORE_H

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif


/***********************************************************************/
/*                  PLUS Release string macros                         */
/***********************************************************************/

/* Define Nucleus PLUS release string */
#define         PLUS_RELEASE_STRING                 "Nucleus PLUS 2.3"

/* Define major, minor, and patch release numbers for Nucleus PLUS */
#define         NU_PLUS_RELEASE_MAJOR_VERSION       2
#define         NU_PLUS_RELEASE_MINOR_VERSION       3
#define         NU_PLUS_RELEASE_PATCH_VERSION       0

/* Starting with PLUS 2.0, PLUS_VERSION_COMP reflects the actual
   version number.  PLUS_VERSION_COMP is MMNNPP, where MM is the major version,
   NN is the minor version, and PP is the patch version */

/* Define Nucleus PLUS version numbering */
#define         PLUS_1_11                           1
#define         PLUS_1_13                           2
#define         PLUS_1_14                           3
#define         PLUS_1_15                           4
#define         PLUS_2_0                            NU_RELEASE_VERSION_MAKE(2,0,0)
#define         PLUS_2_1                            NU_RELEASE_VERSION_MAKE(2,1,0)
#define         PLUS_2_2                            NU_RELEASE_VERSION_MAKE(2,2,0)
#define         PLUS_2_3                            NU_RELEASE_VERSION_MAKE(2,3,0)

/* Current version */
#define         PLUS_VERSION_COMP                   NU_RELEASE_VERSION_MAKE(            \
                                                        NU_PLUS_RELEASE_MAJOR_VERSION,  \
                                                        NU_PLUS_RELEASE_MINOR_VERSION,  \
                                                        NU_PLUS_RELEASE_PATCH_VERSION)
                  
                
/**********************************************************************/
/*                 Other Configuration Settings                       */
/**********************************************************************/

/* DEFINE:      NU_PLUS_INLINING
   DEFAULT:     NU_FALSE
   DESCRIPTION: Frequently used Nucleus PLUS code is inlined (using macros) when this
                define is set to NU_TRUE.  Setting this define to NU_FALSE results in no
                inlining (using macros) - actual function declarations are called in
                this case.
   NOTE:        The Nucleus PLUS library, Nucleus Middleware Initialization library,
                and application must be rebuilt after changing
                this define.            */
#ifndef         NU_PLUS_INLINING
#define         NU_PLUS_INLINING                    CFG_NU_OS_KERN_PLUS_CORE_INLINING
#endif

/* DEFINE:      NU_STACK_CHECKING
   DEFAULT:     NU_FALSE
   DESCRIPTION: Stack checking code is included for all Nucleus PLUS API calls when this
                define is set to NU_TRUE.  Setting this define to NU_FALSE will turn-off
                stack checking code.
   NOTE:        If NU_STACK_FILL is enabled below (NU_TRUE), an additional check for
                stack overflow will occur in the scheduler using the stack fill pattern.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_STACK_CHECKING
#define         NU_STACK_CHECKING                   CFG_NU_OS_KERN_PLUS_CORE_STACK_CHECKING
#endif

/* DEFINE:      NU_STACK_FILL
   DEFAULT:     NU_FALSE
   DESCRIPTION: Stack fill code is included for all Nucleus PLUS API calls when this
                define is set to NU_TRUE.  Setting this define to NU_FALSE will turn-off
                stack fill code.
   NOTE:        The fill pattern used is defined by NU_STACK_FILL_PATTERN found within
                this file
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_STACK_FILL
#define         NU_STACK_FILL                       CFG_NU_OS_KERN_PLUS_CORE_STACK_FILL
#endif

/* Define the stack fill pattern used when NU_STACK_FILL (defined above) is set to NU_TRUE */
#ifndef         NU_STACK_FILL_PATTERN
#define         NU_STACK_FILL_PATTERN               0xA5
#endif

/* DEFINE:      NU_GLOBAL_INT_LOCKING
   DEFAULT:     NU_FALSE
   DESCRIPTION: Global interrupt locking is allowed (interrupt lock-out controlled at
                global level instead of thread / task level) when this define is set
                to NU_TRUE.  Interrupts are controlled at thread / task level if this
                define is set to NU_FALSE (interrupts not controlled at global level)
   NOTE:        Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_GLOBAL_INT_LOCKING
#define         NU_GLOBAL_INT_LOCKING               CFG_NU_OS_KERN_PLUS_CORE_GLOBAL_INT_LOCKING
#endif

/* DEFINE:      NU_MIN_RAM_ENABLED
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to minimize the use of RAM by Nucleus PLUS.  The
                trade-off for minimizing RAM usage is a decrease in speed / performance.
                Setting this define to NU_TRUE will reduce RAM usage.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_MIN_RAM_ENABLED
#define         NU_MIN_RAM_ENABLED                  CFG_NU_OS_KERN_PLUS_CORE_MIN_RAM
#endif

#ifndef         NU_TICK_SUPPRESSION
#define         NU_TICK_SUPPRESSION                 CFG_NU_OS_KERN_PLUS_CORE_TICK_SUPPRESSION
#endif

/* Define the number of Nucleus PLUS ticks that will occur every second.
   By default, the PLUS timer generates an interrupt every 10ms causing the
   PLUS System Clock to tick 100 times in one second */
#ifndef         NU_PLUS_TICKS_PER_SEC
#define         NU_PLUS_TICKS_PER_SEC               CFG_NU_OS_KERN_PLUS_CORE_TICKS_PER_SEC
#endif

/* Define the minimum stack size accepted when creating a task or HISR */
#ifndef         NU_MIN_STACK_SIZE
#define         NU_MIN_STACK_SIZE                   CFG_NU_OS_KERN_PLUS_CORE_MIN_STACK_SIZE 
#endif

/* Define size of Nucleus Timer HISR stack and priority of Nucleus Timer HISR (0-2) */
#ifndef         NU_TIMER_HISR_STACK_SIZE
#define         NU_TIMER_HISR_STACK_SIZE            CFG_NU_OS_KERN_PLUS_CORE_TIMER_HISR_STACK_SIZE
#endif
#ifndef         NU_TIMER_HISR_PRIORITY
#define         NU_TIMER_HISR_PRIORITY              2
#endif

/**********************************************************************/
/*                    Service Parameters                              */
/**********************************************************************/

/* Define TASK suspension constants.  */
#define         NU_EVENT_SUSPEND                    7
#define         NU_FINISHED                         11
#define         NU_MAILBOX_SUSPEND                  3
#define         NU_MEMORY_SUSPEND                   9
#define         NU_PARTITION_SUSPEND                8
#define         NU_PIPE_SUSPEND                     5
#define         NU_PURE_SUSPEND                     1
#define         NU_QUEUE_SUSPEND                    4
#define         NU_READY                            0
#define         NU_SEMAPHORE_SUSPEND                6
#define         NU_SLEEP_SUSPEND                    2
#define         NU_TERMINATED                       12
#define         NU_DEBUG_SUSPEND                    13

/* Internal HISR queue control block buf_status values */
#define        NU_HISR_QUEUE_EMPTY       0
#define        NU_HISR_QUEUE_FULL        1
#define        NU_HISR_QUEUE_DATA        2

/* Used within NU_Send_To_HISR_Queue to either activate the HISR once or every time */
#define        NU_ACTIVATE_HISR          1
#define        NU_ACTIVATE_HISR_ONCE     2


/**********************************************************************/
/*                 ERROR MANAGEMENT Definitions                       */
/**********************************************************************/

/* DEFINE:      NU_ERROR_CHECKING
   DEFAULT:     NU_TRUE
   DESCRIPTION: Error checking code is included for all applicable Nucleus PLUS API calls
                when this define is set to NU_TRUE.  Setting this define to NU_FALSE will
                cause Nucleus PLUS API calls to skip / bypass error checking code.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_ERROR_CHECKING
#define         NU_ERROR_CHECKING                   CFG_NU_OS_KERN_PLUS_CORE_ERROR_CHECKING
#endif

/* DEFINE:      NU_ASSERT_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: Setting NU_ASSERT_ENABLE to NU_TRUE includes additional debugging services
                (NU_ASSERT / NU_CHECK).  With NU_ASSERT_ENABLE set to NU_FALSE, these services
                are not compiled as part of the Nucleus PLUS library.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_ASSERT_ENABLE
#define         NU_ASSERT_ENABLE                    CFG_NU_OS_KERN_PLUS_CORE_ASSERT
#endif

/* DEFINE:      NU_ERROR_STRING
   DEFAULT:     NU_FALSE
   DESCRIPTION: Setting NU_ERROR_STRING to NU_TRUE includes an additional set of
                error strings used within ERC_System_Error.  With NU_ERROR_STRING set
                to NU_FALSE, these strings and corresponding logic are not compiled
                as part of the Nucleus PLUS library.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_ERROR_STRING
#define         NU_ERROR_STRING                     CFG_NU_OS_KERN_PLUS_CORE_ERROR_STRING
#endif

#if (NU_ERROR_CHECKING == NU_TRUE)

/* Define macro for NU_ERROR_CHECK with error checking enabled.
   This macro is used to test any expression as long as the status
   variable is set to NU_SUCCESS */
#define NU_ERROR_CHECK(expression, status_var, status_err)               \
                       if ((status_var == NU_SUCCESS) && (expression))   \
                       {                                                 \
                           status_var = status_err;                      \
                       }

/* Define macro for NU_PARAM_CHECK with error checking enabled.
   This macro is simple in nature */
#define NU_PARAM_CHECK(expression, name, value)                          \
                       if (expression)                                   \
                       {                                                 \
                           name = value;                                 \
                       }

#else

/* Define macro for NU_ERROR_CHECK with error checking disabled */
#define NU_ERROR_CHECK(expression, status_var, status_err)

/* Define macro for NU_PARAM_CHECK with error checking disabled */
#define NU_PARAM_CHECK(expression, name, value)

#endif  /* NU_ERROR_CHECKING == NU_TRUE */


/* Ensure that NU_ASSERT and NU_CHECK are not currently defined */

#ifdef      NU_ASSERT
#undef      NU_ASSERT
#endif

#ifdef      NU_CHECK
#undef      NU_CHECK
#endif

#if         (NU_ASSERT_ENABLE == NU_TRUE)

/* External function declarations */
void        ERC_Assert(CHAR *test, CHAR *name, UNSIGNED line);

/* Define macro for ASSERT */
#define     NU_ASSERT(test)                                             \
                        if ( !(test) )                                  \
                        {                                               \
                            ERC_Assert( #test, __FILE__, __LINE__ );    \
                        }                                               \
                        ((VOID) 0)

/* Define macro for ASSERT2 */
#define     NU_ASSERT2(test)                                            \
                        if ( !(test) )                                  \
                        {                                               \
                            ERC_Assert( #test, __FILE__, __LINE__ );    \
                        }                                               \
                        ((VOID) 0)

#else

/* Define macros for ASSERT and ASSERT2 as VOID */
#define     NU_ASSERT(test) 
#define     NU_ASSERT2(test)

#endif  /* NU_ASSERT_ENABLE == NU_TRUE */

#if         (NU_ERROR_CHECKING == NU_TRUE)

/* Define macro for NU_CHECK with error checking enabled */
#define     NU_CHECK(test, statement)                                   \
                        NU_ASSERT2( test );                             \
                        if ( !(test) )                                  \
                        {                                               \
                            statement;                                  \
                        }                                               \
                        ((void) 0)

#else

/* Define macro for NU_CHECK with error checking disabled */
#define     NU_CHECK(test, statement)       NU_ASSERT2( test )

#endif  /* NU_ERROR_CHECKING == NU_TRUE */


/**********************************************************************/
/*       Other GENERIC ESAL macros/defines needed by PLUS             */
/**********************************************************************/
/* Define all externally accessible, interrupt related function prototypes */
INT                 ESAL_GE_INT_Global_Set(INT new_value);


/**********************************************************************************/
/*          Nucleus Toolset specific macros used by the Kernel                    */
/**********************************************************************************/

/* Define interrupt lockout and enable constants. */
#define             ESAL_GE_INT_DISABLE_BITS                ESAL_AR_INTERRUPTS_DISABLE_BITS
#define             ESAL_GE_INT_ENABLE_BITS                 ESAL_AR_INTERRUPTS_ENABLE_BITS

#define             NU_DISABLE_INTERRUPTS                   ESAL_GE_INT_DISABLE_BITS
#define             NU_ENABLE_INTERRUPTS                    ESAL_GE_INT_ENABLE_BITS

/* Ensure HUGE / FAR pointer type specified - required for
   some 16-bit architectures for pointer crossing page boundaries */
#ifndef             HUGE
#define             HUGE                                    ESAL_TS_HUGE_PTR_TYPE
#endif

#ifndef             FAR
#define             FAR                                     ESAL_TS_FAR_PTR_TYPE
#endif


/* Define macro to read OS timer count */
#define             NU_HW_TIMER_COUNT_READ()                ESAL_GE_TMR_OS_COUNT_READ()

#ifdef CFG_NU_OS_SVCS_PWR_CORE_ENABLE

/* Provide data structure for targets that vary during OP changes */
extern              UINT32                                  PMS_Single_Tick;
#define             NU_HW_TIMER_TICKS_PER_SEC              (PMS_Single_Tick * NU_PLUS_TICKS_PER_SEC)

#else

/* Define constant for number of hardware ticks per second */
#define             NU_HW_TIMER_TICKS_PER_SEC               (ESAL_GE_TMR_OS_CLOCK_RATE /    \
                                                             ESAL_GE_TMR_OS_CLOCK_PRESCALE)

#endif

/* Define the number of hardware timer ticks that occur for each Nucleus PLUS
   software tick */
#define             NU_HW_TIMER_TICKS_PER_SW_TICK           (NU_HW_TIMER_TICKS_PER_SEC /    \
                                                             NU_PLUS_TICKS_PER_SEC)

                                                             
/**********************************************************************/
/*                   COMMON NODE Definitions                          */
/**********************************************************************/
/* Define a common node data structure that can be included inside of 
   other system data structures.  */
   
typedef struct  CS_NODE_STRUCT
{
    struct CS_NODE_STRUCT  *cs_previous;
    struct CS_NODE_STRUCT  *cs_next;
    DATA_ELEMENT            cs_priority;

#if     PAD_1
    DATA_ELEMENT            cs_padding[PAD_1];
#endif

}  CS_NODE;


#if (NU_PLUS_INLINING == NU_FALSE)

/* Use declared functions */
VOID            NU_Place_On_List(CS_NODE **head, CS_NODE *new_node);
VOID            NU_Remove_From_List(CS_NODE **head, CS_NODE *node);

#else

/* Inlining enabled - use macros */
#define         NU_Place_On_List(head, new_node);                            \
                if (*((CS_NODE **) (head)))                                  \
                {                                                            \
                    ((CS_NODE *) (new_node)) -> cs_previous=                 \
                            (*((CS_NODE **) (head))) -> cs_previous;         \
                    (((CS_NODE *) (new_node)) -> cs_previous) -> cs_next =   \
                            (CS_NODE *) (new_node);                          \
                    ((CS_NODE *) (new_node)) -> cs_next =                    \
                            (*((CS_NODE **) (head)));                        \
                    (((CS_NODE *) (new_node)) -> cs_next) -> cs_previous =   \
                            ((CS_NODE *) (new_node));                        \
                }                                                            \
                else                                                         \
                {                                                            \
                    (*((CS_NODE **) (head))) = ((CS_NODE *) (new_node));     \
                    ((CS_NODE *) (new_node)) -> cs_previous =                \
                            ((CS_NODE *) (new_node));                        \
                    ((CS_NODE *) (new_node)) -> cs_next =                    \
                            ((CS_NODE *) (new_node));                        \
                }

#define         NU_Remove_From_List(head, node);                             \
                if (((CS_NODE *) (node)) -> cs_previous ==                   \
                                            ((CS_NODE *) (node)))            \
                {                                                            \
                    (*((CS_NODE **) (head))) =  NU_NULL;                     \
                }                                                            \
                else                                                         \
                {                                                            \
                    (((CS_NODE *) (node)) -> cs_previous) -> cs_next =       \
                                         ((CS_NODE *) (node)) -> cs_next;    \
                    (((CS_NODE *) (node)) -> cs_next) -> cs_previous =       \
                                     ((CS_NODE *) (node)) -> cs_previous;    \
                    if (((CS_NODE *) (node)) == *((CS_NODE **) (head)))      \
                        *((CS_NODE **) (head)) =                             \
                            ((CS_NODE *) (node)) -> cs_next;                 \
                }

#endif  /* NU_PLUS_INLINING == NU_FALSE */

/* This function is used regarless of inlining */
VOID            NU_Priority_Place_On_List(CS_NODE **head, CS_NODE *new_node);

/**********************************************************************/
/*                 TIMER MANAGEMENT Definitions                       */
/**********************************************************************/

/* Define the Timer Control Block data type.  */
typedef struct TM_TCB_STRUCT
{
    INT                 tm_timer_type;         /* Application/Task      */
    UNSIGNED            tm_remaining_time;     /* Remaining time        */
    VOID               *tm_information;        /* Information pointer   */
    struct TM_TCB_STRUCT
                       *tm_next_timer,         /* Next timer in list    */
                       *tm_previous_timer;     /* Previous timer in list*/
} TM_TCB;


/* Define Application's Timer Control Block data type.  */

typedef struct TM_APP_TCB_STRUCT
{
    CS_NODE             tm_created;            /* Node for linking to   */
                                               /*   created timer list  */
    UNSIGNED            tm_id;                 /* Internal TCB ID       */
    CHAR                tm_name[NU_MAX_NAME];  /* Timer name            */
    VOID  (*tm_expiration_routine)(UNSIGNED);  /* Expiration function   */
    UNSIGNED            tm_expiration_id;      /* Expiration ID         */
    BOOLEAN             tm_enabled;            /* Timer enabled flag    */
    BOOLEAN             tm_paused_status;      /* Pause status          */
#if     PAD_2
    DATA_ELEMENT        tm_padding[PAD_2];
#endif

    UNSIGNED            tm_expirations;        /* Number of expirations */
    UNSIGNED            tm_initial_time;       /* Initial time          */
    UNSIGNED            tm_reschedule_time;    /* Reschedule time       */
    TM_TCB              tm_actual_timer;       /* Actual timer internals*/
    UNSIGNED            tm_paused_time;        /* Pause remaining time  */
} TM_APP_TCB;

/* Target dependent functions.  */
VOID            TMCT_Timer_Interrupt(INT vector);

/**********************************************************************/
/*                  TASK CONTROL Definitions                          */
/**********************************************************************/

/* Define the Task Control Block data type.  */
typedef struct TC_TCB_STRUCT
{
    /* Standard thread information first.  This information is used by
       the target dependent portion of this component.  Changes made
       to this area of the structure can have undesirable side effects.  */

    CS_NODE             tc_created;            /* Node for linking to    */
                                               /*   created task list    */
    UNSIGNED            tc_id;                 /* Internal TCB ID        */
    CHAR                tc_name[NU_MAX_NAME];  /* Task name              */
    DATA_ELEMENT        tc_status;             /* Task status            */
    BOOLEAN             tc_delayed_suspend;    /* Delayed task suspension*/
    DATA_ELEMENT        tc_priority;           /* Task priority          */
    BOOLEAN             tc_preemption;         /* Task preemption enable */
    UNSIGNED            tc_scheduled;          /* Task scheduled count   */
    UNSIGNED            tc_cur_time_slice;     /* Current time slice     */
    VOID               *tc_stack_start;        /* Stack starting address */
    VOID               *tc_stack_end;          /* Stack ending address   */
    VOID               *tc_stack_pointer;      /* Task stack pointer     */
    UNSIGNED            tc_stack_size;         /* Task stack's size      */
    UNSIGNED            tc_stack_minimum;      /* Minimum stack size     */
    UNSIGNED            tc_grp_id;             /* Group id               */

    /* Process related structure members */
    VOID               *tc_process;            /* Pointer to process CB */
    CS_NODE             tc_proc_node;          /* Node element for process tracking */
    VOID               *tc_return_addr;        /* Return address of service call */
    VOID               *tc_saved_return_addr;  /* Previous return address of service call */

    VOID               *tc_saved_stack_ptr;    /* Previous stack pointer */
    UNSIGNED            tc_time_slice;         /* Task time slice value  */

    /* Information after this point is not used in the target dependent
       portion of this component.  Hence, changes in the following section
       should not impact assembly language routines.  */
    struct TC_TCB_STRUCT
                       *tc_ready_previous,     /* Previously ready TCB   */
                       *tc_ready_next;         /* next and previous ptrs */

    /* Task control information follows.  */

    UNSIGNED            tc_priority_group;     /* Priority group mask bit*/
    struct TC_TCB_STRUCT
                      **tc_priority_head;      /* Pointer to list head   */
    DATA_ELEMENT       *tc_sub_priority_ptr;   /* Pointer to sub-group   */
    DATA_ELEMENT        tc_sub_priority;       /* Mask of sub-group bit  */
    DATA_ELEMENT        tc_saved_status;       /* Previous task status   */
    BOOLEAN             tc_signal_active;      /* Signal active flag     */

    BOOLEAN             tc_auto_clean;         /* Terminate/Delete at    */
                                               /* task completion        */

                                               /* Task entry function    */
    VOID                (*tc_entry)(UNSIGNED, VOID *);
    UNSIGNED            tc_argc;               /* Optional task argument */
    VOID               *tc_argv;               /* Optional task argument */
    VOID                (*tc_cleanup) (VOID *);/* Clean-up routine       */
    VOID               *tc_cleanup_info;       /* Clean-up information   */

    /* Task timer information.  */
    INT                 tc_timer_active;       /* Active timer flag      */
    TM_TCB              tc_timer_control;      /* Timer control block    */

    /* Task signal control information.  */

    UNSIGNED            tc_signals;            /* Current signals        */
    UNSIGNED            tc_enabled_signals;    /* Enabled signals        */

    /* tc_saved_status and tc_signal_active are now defined above in an
       attempt to keep DATA_ELEMENT types together.  */

    /* Signal handling routine.  */
    VOID                (*tc_signal_handler) (UNSIGNED);

    /* Reserved words for the system and a single reserved word for the
       application.  */
    UNSIGNED            tc_system_reserved_1;  /* System reserved word   */
    UNSIGNED            tc_system_reserved_2;  /* System reserved word   */
    UNSIGNED            tc_system_reserved_3;  /* System reserved word   */
    UNSIGNED            tc_app_reserved_1;     /* Application reserved   */
    
    CS_NODE             *tc_semaphore_list;    /* Pointer to list of PI  */
                                               /* semaphores this task   */
                                               /* owns.                  */
    DATA_ELEMENT        tc_base_priority;      /* Base priority of task  */
                                               /* for PI semaphores      */
    DATA_ELEMENT        tc_semaphore_count;    /* Number of priority     */
                                               /* modifying resources    */
                                               /* that the task has      */
                                               /* acquired               */
    
    struct TC_TCB_STRUCT                       
                        *tc_grp_next;          /* Next app or sys task   */
    struct TC_TCB_STRUCT
                        *tc_grp_prev;          /* Last app or sys task   */
                        
    DATA_ELEMENT        tc_debug_suspend;      /* Debug suspension       */

} TC_TCB;


/* Define the High-Level Interrupt Service Routine Control Block data type.  */

typedef struct TC_HCB_STRUCT
{
    /* Standard thread information first.  This information is used by
       the target dependent portion of this component.  Changes made
       to this area of the structure can have undesirable side effects.  */

    CS_NODE             tc_created;            /* Node for linking to    */
                                               /*   created task list    */
    UNSIGNED            tc_id;                 /* Internal TCB ID        */
    CHAR                tc_name[NU_MAX_NAME];  /* HISR name              */
    DATA_ELEMENT        tc_not_used_1;         /* Not used field         */
    DATA_ELEMENT        tc_not_used_2;         /* Not used field         */
    DATA_ELEMENT        tc_priority;           /* HISR priority          */
    DATA_ELEMENT        tc_not_used_3;         /* Not used field         */
    UNSIGNED            tc_scheduled;          /* HISR scheduled count   */
    UNSIGNED            tc_cur_time_slice;     /* Not used in HISR       */
    VOID               *tc_stack_start;        /* Stack starting address */
    VOID               *tc_stack_end;          /* Stack ending address   */
    VOID               *tc_stack_pointer;      /* HISR stack pointer     */
    UNSIGNED            tc_stack_size;         /* HISR stack's size      */
    UNSIGNED            tc_stack_minimum;      /* Minimum stack size     */
    UNSIGNED            tc_grp_id;             /* Group id               */

    /* Process related structure members */
    VOID               *tc_process;            /* Pointer to process CB */

    struct TC_HCB_STRUCT
                       *tc_active_next;        /* Next activated HISR    */
    UNSIGNED            tc_activation_count;   /* Activation counter     */
    VOID                (*tc_entry)(VOID);     /* HISR entry function    */
    VOID               *tc_hisr_queue;         /* HISR Queue control
                                                  block pointer          */

    /* Information after this point is not used in the target dependent
       portion of this component.  Hence, changes in the following section
       should not impact assembly language routines.  */


    /* Reserved words for the system and a single reserved word for the
       application.  */
    UNSIGNED            tc_system_reserved_1;  /* System reserved word   */
    UNSIGNED            tc_system_reserved_2;  /* System reserved word   */
    UNSIGNED            tc_system_reserved_3;  /* System reserved word   */
    UNSIGNED            tc_app_reserved_1;     /* Application reserved   */

} TC_HCB;


/* Define externally referenced variables.   */
extern VOID * volatile      TCD_Current_Thread;
extern TC_TCB * volatile    TCD_Execute_Task;
extern TC_HCB * volatile    TCD_Execute_HISR;
extern TC_TCB * volatile    TCD_Current_App_Task;
extern TC_TCB              *TCD_App_Task_List;
extern UNSIGNED            TCD_Total_App_Tasks;

/**********************************************************************/
/*                  DYNAMIC MEMORY Definitions                        */
/**********************************************************************/

/* Define the Dynamic Pool Control Block data type.  */

typedef struct DM_PCB_STRUCT
{
    CS_NODE             dm_created;            /* Node for linking to    */
                                               /* created dynamic pools  */
    UNSIGNED            dm_id;                 /* Internal PCB ID        */
    CHAR                dm_name[NU_MAX_NAME];  /* Dynamic Pool name      */
    VOID               *dm_start_address;      /* Starting pool address  */
    UNSIGNED            dm_pool_size;          /* Size of pool           */
    UNSIGNED            dm_min_allocation;     /* Minimum allocate size  */
    UNSIGNED            dm_available;          /* Total available bytes  */
    struct DM_HEADER_STRUCT
                       *dm_memory_list;        /* Memory list            */
    BOOLEAN             dm_fifo_suspend;       /* Suspension type flag   */
#if     PAD_1
    DATA_ELEMENT        dm_padding[PAD_1];
#endif
    UNSIGNED            dm_tasks_waiting;      /* Number of waiting tasks*/
    struct DM_SUSPEND_STRUCT
                       *dm_suspension_list;    /* Suspension list        */
} DM_PCB;


/**********************************************************************/
/*                   EVENT GROUP Definitions                          */
/**********************************************************************/

/* Define the Event Group Control Block data type.  */
typedef struct EV_GCB_STRUCT
{
    CS_NODE             ev_created;            /* Node for linking to    */
                                               /*   created Events list  */
    UNSIGNED            ev_id;                 /* Internal EV ID         */
    CHAR                ev_name[NU_MAX_NAME];  /* Event group name       */
    UNSIGNED            ev_current_events;     /* Current event flags    */
    UNSIGNED            ev_tasks_waiting;      /* Number of waiting tasks*/
    struct EV_SUSPEND_STRUCT
                       *ev_suspension_list;    /* Suspension list        */
} EV_GCB;


/**********************************************************************/
/*                      QUEUE Definitions                             */
/**********************************************************************/

/* Define the Queue Control Block data type.  */
typedef struct QU_QCB_STRUCT
{
    CS_NODE             qu_created;            /* Node for linking to    */
                                               /*   created queue list   */
    UNSIGNED            qu_id;                 /* Internal QCB ID        */
    CHAR                qu_name[NU_MAX_NAME];  /* Queue name             */
    BOOLEAN             qu_fixed_size;         /* Fixed-size messages?   */
    BOOLEAN             qu_fifo_suspend;       /* Suspension type flag   */
#if     PAD_2
    DATA_ELEMENT        qu_padding[PAD_2];
#endif
    UNSIGNED            qu_queue_size;         /* Total size of queue    */
    UNSIGNED            qu_messages;           /* Messages in queue      */
    UNSIGNED            qu_message_size;       /* Size of each message   */
    UNSIGNED            qu_available;          /* Available words        */
    UNSIGNED_PTR        qu_start;              /* Start of queue area    */
    UNSIGNED_PTR        qu_end;                /* End of queue area + 1  */
    UNSIGNED_PTR        qu_read;               /* Read pointer           */
    UNSIGNED_PTR        qu_write;              /* Write pointer          */
    UNSIGNED            qu_tasks_waiting;      /* Number of waiting tasks*/
    struct QU_SUSPEND_STRUCT
                       *qu_urgent_list;        /* Urgent message suspend */
    struct QU_SUSPEND_STRUCT
                       *qu_suspension_list;    /* Suspension list        */
} QU_QCB;


/**********************************************************************/
/*                  SEMAPHORE Definitions                             */
/**********************************************************************/

/* Define the Semaphore Control Block data type.  */
typedef struct SM_SCB_STRUCT
{
    CS_NODE             sm_created;            /* Node for linking to    */
                                               /* created semaphore list */
    UNSIGNED            sm_id;                 /* Internal SCB ID        */
    CHAR                sm_name[NU_MAX_NAME];  /* Semaphore name         */
    UNSIGNED            sm_semaphore_count;    /* Counting semaphore     */
    OPTION              sm_suspend_type;       /* Suspension type        */
    BOOLEAN             sm_owner_killed;       /* Flag if owner killed   */
#if     PAD_2
    DATA_ELEMENT        sm_padding[PAD_2];
#endif
    TC_TCB             *sm_semaphore_owner;    /* Task that owns the     */
                                               /* semaphore              */
    UNSIGNED            sm_tasks_waiting;      /* Number of waiting      */
                                               /* tasks                  */
    CS_NODE             sm_semaphore_list;     /* Node for linking owned */
                                               /* semaphores             */                                              
    struct SM_SUSPEND_STRUCT
                       *sm_suspension_list;    /* Suspension list        */
} SM_SCB;


/**********************************************************************/
/*                      PLUS Definitions                              */
/**********************************************************************/

/* Define application data types to actual internal data structures */
typedef         TC_TCB                              NU_TASK;
typedef         TC_HCB                              NU_HISR;
typedef         DM_PCB                              NU_MEMORY_POOL;
typedef         TM_APP_TCB                          NU_TIMER;
typedef         UINT8                               NU_PROTECT;
typedef         QU_QCB                              NU_QUEUE;
typedef         SM_SCB                              NU_SEMAPHORE;
typedef         EV_GCB                              NU_EVENT_GROUP;

/* External declarations */
extern          INT                                 TCD_Interrupt_Level;
extern          volatile INT                        TMD_Timer_State;

/**********************************************************************/
/*             Other TIMER MANAGEMENT Definitions                     */
/**********************************************************************/

/* Externally reference global data */
extern volatile UNSIGNED    TMD_Timer;
extern volatile UNSIGNED    TMD_System_Clock;
extern volatile UNSIGNED    TMD_System_Clock_Upper;
extern volatile INT         TMD_Timer_State;
extern TC_TCB  *volatile    TMD_Time_Slice_Task;

/*********** START BACKWARDS COMPATIBILITY DEFINITIONS FOR NUCLEUS PLUS 2.0 **********/

/* NOTE:  The following definitions were created as part of
          Nucleus PLUS 2.0 to allow backwards compatibility
          of all Nucleus PLUS internal functions accessed
          by applications and any configuration settings modified
          by Nucleus PLUS 2.0                                            */

/* Hardware timer / clock related backwards compatibility */
#define             NU_Retrieve_Hardware_Clock(val)         val = NU_HW_TIMER_COUNT_READ()
#define             NU_PLUS_Ticks_Per_Second                NU_PLUS_TICKS_PER_SEC
#define             NU_HW_Ticks_Per_Second                  NU_HW_TIMER_TICKS_PER_SEC
#define             NU_HW_Ticks_Per_SW_Tick                 NU_HW_TIMER_TICKS_PER_SW_TICK

#if (ESAL_GE_TMR_OS_COUNT_DIR == ESAL_COUNT_DOWN)

#define             NU_COUNT_DOWN

#else

#undef              NU_COUNT_DOWN

#endif  /* ESAL_GE_TMR_OS_COUNT_DIR == ESAL_COUNT_DOWN */

/* Re-map old internal timer and thread control function names to new internal names */
#define         TCT_Current_Thread                  TCCT_Current_Thread
#define         CSC_Place_On_List                   NU_Place_On_List
#define         CSC_Remove_From_List                NU_Remove_From_List

/*********** END BACKWARDS COMPATIBILITY DEFINITIONS FOR NUCLEUS PLUS 2.0 **********/

extern INT TCD_Protect_Save;

/* Map the following common service API to control functions */

#ifndef NU_PROCESS
/* For kernel, use macros */
#define         NU_Protect(x)                       NU_UNUSED_PARAM(x); \
                                                    TCD_Protect_Save = NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS)
#define         NU_Unprotect()                      NU_Local_Control_Interrupts(TCD_Protect_Save)
#else
/* For processes, these functions are included in process user lib */
VOID            NU_Protect(NU_PROTECT *);
VOID            NU_Unprotect(VOID);
#endif

#define         NU_Local_Control_Interrupts         ESAL_GE_INT_Global_Set
#define         NU_Restore_Interrupts()             ESAL_GE_INT_Global_Set(TCD_Interrupt_Level)

#if (NU_STACK_CHECKING == NU_TRUE)
UNSIGNED        NU_Check_Stack(VOID);
#else
#define         NU_Check_Stack()                    0
#endif

/* Define task control functions.  */
STATUS          NU_Create_Task(NU_TASK *task, CHAR *name,
                               VOID (*task_entry)(UNSIGNED, VOID *), UNSIGNED argc,
                               VOID *argv, VOID *stack_address, UNSIGNED stack_size,
                               OPTION priority, UNSIGNED time_slice,
                               OPTION preempt, OPTION auto_start);
STATUS          NU_Delete_Task(NU_TASK *task);
STATUS          NU_Reset_Task(NU_TASK *task, UNSIGNED argc, VOID *argv);
STATUS          NU_Terminate_Task(NU_TASK *task);
STATUS          NU_Resume_Task(NU_TASK *task);
STATUS          NU_Suspend_Task(NU_TASK *task);
VOID            NU_Relinquish(VOID);
VOID            NU_Sleep(UNSIGNED ticks);
OPTION          NU_Change_Priority(NU_TASK *task, OPTION new_priority);
OPTION          NU_Change_Preemption(OPTION preempt);
UNSIGNED        NU_Change_Time_Slice(NU_TASK *task, UNSIGNED time_slice);
NU_TASK         *NU_Current_Task_Pointer(VOID);
UNSIGNED        NU_Established_Tasks(VOID);
STATUS          NU_Task_Information(NU_TASK *task, CHAR *name,
                                    DATA_ELEMENT *status, UNSIGNED *scheduled_count,
                                    OPTION *priority, OPTION *preempt,
                                    UNSIGNED *time_slice, VOID **stack_base,
                                    UNSIGNED *stack_size, UNSIGNED *minimum_stack);
UNSIGNED        NU_Task_Pointers(NU_TASK **pointer_list,
                                 UNSIGNED maximum_pointers);

STATUS          NU_Create_Auto_Clean_Task(NU_TASK **task_ptr, CHAR *name,
                                          VOID (*task_entry)(UNSIGNED, VOID *),
                                          UNSIGNED argc, VOID *argv,
                                          NU_MEMORY_POOL *pool_ptr, 
                                          UNSIGNED stack_size,
                                          OPTION priority, UNSIGNED time_slice,
                                          OPTION preempt, OPTION auto_start);

/* Define Queue management functions.  */
STATUS          NU_Create_Queue(NU_QUEUE *queue, CHAR *name,
                                VOID *start_address, UNSIGNED queue_size,
                                OPTION message_type, UNSIGNED message_size,
                                OPTION suspend_type);
STATUS          NU_Delete_Queue(NU_QUEUE *queue);
STATUS          NU_Reset_Queue(NU_QUEUE *queue);
STATUS          NU_Send_To_Front_Of_Queue(NU_QUEUE *queue, VOID *message,
                                          UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Send_To_Queue(NU_QUEUE *queue, VOID *message,
                                 UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Broadcast_To_Queue(NU_QUEUE *queue, VOID *message,
                                      UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Receive_From_Queue(NU_QUEUE *queue, VOID *message,
                                      UNSIGNED size, UNSIGNED *actual_size,
                                      UNSIGNED suspend);
UNSIGNED        NU_Established_Queues(VOID);
STATUS          NU_Queue_Information(NU_QUEUE *queue, CHAR *name,
                                     VOID **start_address, UNSIGNED *queue_size,
                                     UNSIGNED *available, UNSIGNED *messages,
                                     OPTION *message_type, UNSIGNED *message_size,
                                     OPTION *suspend_type, UNSIGNED *tasks_waiting,
                                     NU_TASK **first_task);
UNSIGNED        NU_Queue_Pointers(NU_QUEUE **pointer_list,
                                  UNSIGNED maximum_pointers);

/* Define Semaphore management functions.  */
STATUS          NU_Create_Semaphore(NU_SEMAPHORE *semaphore, CHAR *name,
                                    UNSIGNED initial_count, OPTION suspend_type);
STATUS          NU_Delete_Semaphore(NU_SEMAPHORE *semaphore);
STATUS          NU_Reset_Semaphore(NU_SEMAPHORE *semaphore,
                                   UNSIGNED initial_count);
STATUS          NU_Obtain_Semaphore(NU_SEMAPHORE *semaphore, UNSIGNED suspend);
STATUS          NU_Release_Semaphore(NU_SEMAPHORE *semaphore);
UNSIGNED        NU_Established_Semaphores(VOID);
STATUS          NU_Semaphore_Information(NU_SEMAPHORE *semaphore, CHAR *name,
                                         UNSIGNED *current_count, OPTION *suspend_type,
                                         UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        NU_Semaphore_Pointers(NU_SEMAPHORE **pointer_list,
                                      UNSIGNED maximum_pointers);
STATUS          NU_Get_Semaphore_Owner(NU_SEMAPHORE *semaphore_ptr, NU_TASK **task);                                      


/* Define Event Group management functions.  */
STATUS          NU_Create_Event_Group(NU_EVENT_GROUP *group, CHAR *name);
STATUS          NU_Delete_Event_Group(NU_EVENT_GROUP *group);
STATUS          NU_Set_Events(NU_EVENT_GROUP *group, UNSIGNED events,
                              OPTION operation);
STATUS          NU_Retrieve_Events(NU_EVENT_GROUP *group,
                                   UNSIGNED requested_flags, OPTION operation,
                                   UNSIGNED *retrieved_flags, UNSIGNED suspend);
UNSIGNED        NU_Established_Event_Groups(VOID);
STATUS          NU_Event_Group_Information(NU_EVENT_GROUP *group, CHAR *name,
                                           UNSIGNED *event_flags, UNSIGNED *tasks_waiting,
                                           NU_TASK **first_task);
UNSIGNED        NU_Event_Group_Pointers(NU_EVENT_GROUP **pointer_list,
                                        UNSIGNED maximum_pointers);       

/* Define Dynamic memory management functions.  */
STATUS          NU_Create_Memory_Pool(NU_MEMORY_POOL *pool, CHAR *name,
                                      VOID *start_address, UNSIGNED pool_size,
                                      UNSIGNED min_allocation, OPTION suspend_type);
STATUS          NU_Delete_Memory_Pool(NU_MEMORY_POOL *pool);

#define         NU_Allocate_Memory(pool_ptr, return_pointer, size, suspend)             \
                NU_Allocate_Aligned_Memory(pool_ptr, return_pointer, size, 0, suspend)

STATUS          NU_Add_Memory(NU_MEMORY_POOL *pool_ptr, VOID *memory_start_address,
                              UNSIGNED memory_size);
#define         NU_Reallocate_Memory(pool_ptr, memory_ptr, size, suspend)               \
                NU_Reallocate_Aligned_Memory(pool_ptr, memory_ptr, size, 0, suspend)

STATUS          NU_Reallocate_Aligned_Memory(NU_MEMORY_POOL *pool_ptr, VOID **return_pointer,
                                             UNSIGNED size, UNSIGNED alignment,
                                             UNSIGNED suspend);

STATUS          NU_Allocate_Aligned_Memory(NU_MEMORY_POOL *pool_ptr,
                                           VOID **return_pointer, UNSIGNED size,
                                           UNSIGNED alignment, UNSIGNED suspend);

STATUS          NU_Deallocate_Memory(VOID *memory);
UNSIGNED        NU_Established_Memory_Pools(VOID);
STATUS          NU_Memory_Pool_Information(NU_MEMORY_POOL *pool, CHAR *name,
                                           VOID **start_address, UNSIGNED *pool_size,
                                           UNSIGNED *min_allocation, UNSIGNED *available,
                                           OPTION *suspend_type, UNSIGNED *tasks_waiting,
                                           NU_TASK **first_task);
UNSIGNED        NU_Memory_Pool_Pointers(NU_MEMORY_POOL **pointer_list,
                                        UNSIGNED maximum_pointers);

/* Define Interrupt management functions.  */
INT             NU_Control_Interrupts(INT new_level);
STATUS          NU_Register_LISR(INT vector,
                                 VOID (*new_lisr)(INT),
                                 VOID (**old_lisr)(INT));
STATUS          NU_Activate_HISR(NU_HISR *hisr);
STATUS          NU_Create_HISR(NU_HISR *hisr, CHAR *name,
                               VOID (*hisr_entry)(VOID), OPTION priority,
                               VOID *stack_address, UNSIGNED stack_size);
STATUS          NU_Delete_HISR(NU_HISR *hisr);
NU_HISR         *NU_Current_HISR_Pointer(VOID);
UNSIGNED        NU_Established_HISRs(VOID);
STATUS          NU_HISR_Information(NU_HISR *hisr, CHAR *name,
                                    UNSIGNED *scheduled_count, DATA_ELEMENT *priority,
                                    VOID **stack_base, UNSIGNED *stack_size,
                                    UNSIGNED *minimum_stack);
UNSIGNED        NU_HISR_Pointers(NU_HISR **pointer_list,
                                 UNSIGNED maximum_pointers);
STATUS          NU_Create_HISR_Queue(NU_HISR * hisr_ptr, UINT queue_size,
                                     BOOLEAN overwrite_when_full);
STATUS          NU_Delete_HISR_Queue(NU_HISR * hisr_ptr);
STATUS          NU_Send_To_HISR_Queue(NU_HISR * hisr_ptr, VOID * data_ptr, UINT activate_hisr);
STATUS          NU_Receive_From_HISR_Queue(NU_HISR * hisr_ptr, VOID ** data_ptr);

/* Timer management functions.  */
STATUS          NU_Create_Timer(NU_TIMER *timer, CHAR *name, VOID(*expiration_routine)(UNSIGNED),
                                UNSIGNED id, UNSIGNED initial_time, UNSIGNED reschedule_time,
                                OPTION enable);
STATUS          NU_Delete_Timer(NU_TIMER *timer);
STATUS          NU_Reset_Timer(NU_TIMER *timer,
                               VOID (*expiration_routine)(UNSIGNED),
                               UNSIGNED initial_time, UNSIGNED reschedule_timer,
                               OPTION enable);
STATUS          NU_Control_Timer(NU_TIMER *timer, OPTION enable);
UNSIGNED        NU_Established_Timers(VOID);
STATUS          NU_Timer_Information(NU_TIMER *timer, CHAR *name,
                                     OPTION *enable, UNSIGNED *expirations, UNSIGNED *id,
                                     UNSIGNED *initial_time, UNSIGNED *reschedule_time);
STATUS          NU_Get_Remaining_Time(NU_TIMER *timer_ptr, UNSIGNED *remaining_time);
STATUS          NU_Pause_Timer(NU_TIMER *timer_ptr);
STATUS          NU_Resume_Timer(NU_TIMER *timer_ptr);
UNSIGNED        NU_Timer_Pointers(NU_TIMER **pointer_list,
                                  UNSIGNED maximum_pointers);

UINT64          NU_Get_Time_Stamp(VOID);

extern VOID    ESAL_CO_MEM_Region_Setup(INT region_num,
								UINT32 vrt_addr,
								UINT32 phy_addr,
								UINT32 size,
								ESAL_GE_MEMORY_TYPE mem_type,
								ESAL_GE_CACHE_TYPE cache_type,
								UINT32 access_type);

/* Determine if pointers / 32-bit values are accessible with a single instruction.
   If so, just reference the pointer / 32-bit value directly.  Otherwise, call
   the target dependent service.  */
#if ((NU_32BIT_ACCESS == 1) && (NU_PLUS_INLINING == 1) && !defined(CFG_NU_OS_KERN_PROCESS_ENABLE))

/* Macro definition for architectures supporting single instruction
   access to 32-bit values */
#define         NU_Retrieve_Clock()             TMD_System_Clock

#else

/* Define function prototype */
UNSIGNED        NU_Retrieve_Clock(VOID);

#endif  /* ((NU_32BIT_ACCESS == 1) && (NU_PLUS_INLINING == 1) && !defined(CFG_NU_OS_KERN_PROCESS_ENABLE)) */

#ifndef CFG_NU_OS_ARCH_SH_RENESAS_ENABLE
#include <time.h>
#endif
VOID            NU_Set_Clock(UNSIGNED new_value)     ESAL_TS_RTE_DEPRECATED;
VOID            NU_Set_Clock64(UINT64 new_value);
UINT64          NU_Retrieve_Clock64(VOID);
time_t          NU_Ticks_To_Time(UINT64 ticks);
UINT64          NU_Time_To_Ticks(time_t cal_time);

/* System memory functions */
STATUS          NU_System_Memory_Get(NU_MEMORY_POOL ** sys_pool_ptr, 
                                     NU_MEMORY_POOL ** usys_pool_ptr);                                      

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif 
