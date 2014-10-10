/***********************************************************************
*
*            Copyright 1993 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************

************************************************************************
*
*   DESCRIPTION
*
*       This file contains data structure definitions and constants for
*       the component that controls the various threads of execution in
*       system.  Threads include tasks, HISRs, signal handlers, etc.
*
***********************************************************************/

/* Check to see if the file has been included already.  */

#ifndef THREAD_CONTROL_H
#define THREAD_CONTROL_H

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

/* Define constants local to this component.  */

#define         TC_TASK_ID              0x5441534bUL
#define         TC_HISR_ID              0x48495352UL
#define         TC_PRIORITIES           CFG_NU_OS_KERN_PLUS_CORE_NUM_TASK_PRIORITIES
#define         TC_HISR_PRIORITIES      3
#define         TC_MAX_GROUPS           (TC_PRIORITIES/8)
#define         TC_HIGHEST_MASK         0x000000FFUL
#define         TC_NEXT_HIGHEST_MASK    0x0000FF00UL
#define         TC_NEXT_LOWEST_MASK     0x00FF0000UL
#define         TC_LOWEST_MASK          0xFF000000UL
#define         TC_MAX_HISR_ACTIVATIONS 0xFFFFFFFFUL
#define         TC_NO_THREAD_WAITING    0
#define         TC_THREAD_WAITING       1
#define         TC_GRP_ID_SYS           0
#define         TC_GRP_ID_APP           1


/* Core error checking functions.  */

STATUS          TCCE_Validate_Resume(OPTION resume_type, NU_TASK *task_ptr);
INT             TCCE_Suspend_Error(VOID);

/* Core processing functions.  */

STATUS          TCC_Resume_Task(NU_TASK *task_ptr, OPTION suspend_type);
STATUS          TCC_Debug_Resume_Service(NU_TASK *task_ptr);
VOID            TCC_Suspend_Task(NU_TASK *task_ptr, OPTION suspend_type,
                                 VOID (*cleanup)(VOID *),
                                 VOID *information, UNSIGNED timeout);
STATUS          TCC_Debug_Suspend_Service(NU_TASK *task_ptr);
VOID            TCC_Task_Timeout(NU_TASK *task_ptr);
VOID            TCC_Time_Slice(NU_TASK *task_ptr);

/* Core functions with target dependencies */
VOID            TCCT_Schedule(VOID);

extern BOOLEAN  TCD_Schedule_Lock;
#if (CFG_NU_OS_KERN_PLUS_CORE_DEBUG_SCHED_LOCK == NU_TRUE)
extern VOID     ERC_System_Error(INT error_code);
#define         TCCT_Schedule_Lock()                                            \
                {                                                               \
                    /* Ensure lock not set (if set, invalid usage / nesting) */ \
                    if (TCD_Schedule_Lock != NU_FALSE)                          \
                    {                                                           \
                        /* System error */                                      \
                        ERC_System_Error(NU_INVALID_LOCK_USAGE);                \
                    }                                                           \
                                                                                \
                    TCD_Schedule_Lock = NU_TRUE;                                \
                    ESAL_GE_RTE_COMPILE_MEM_BARRIER();                          \
                }
#else
#define         TCCT_Schedule_Lock()                                            \
                {                                                               \
                    TCD_Schedule_Lock = NU_TRUE;                                \
                    ESAL_GE_RTE_COMPILE_MEM_BARRIER();                          \
                }
#endif  /* CFG_NU_OS_KERN_PLUS_CORE_DEBUG_SCHED_LOCK == NU_TRUE */

VOID            TCCT_Schedule_Unlock(VOID);

VOID            TCCT_Control_To_System(VOID);

/* OS State Functions */
UNSIGNED        TCS_Task_Group_ID(NU_TASK *task_ptr);
UNSIGNED        TCS_Change_Group_ID(NU_TASK *task_ptr, UNSIGNED group_id);
NU_TASK        *TCC_Current_Application_Task_Pointer(VOID);
UNSIGNED        TCF_Established_Application_Tasks(VOID);
UNSIGNED        TCF_Application_Task_Pointers(NU_TASK ** pointer_list,
                                              UNSIGNED maximum_pointers);

#define TCC_Application_Task_Add(task);                                         \
                task->tc_grp_id = TC_GRP_ID_APP;                                \
                if(TCD_App_Task_List == NU_NULL)                                \
                {                                                               \
                    TCD_App_Task_List = task;                                   \
                    task->tc_grp_next = task;                                   \
                    task->tc_grp_prev = task;                                   \
                }                                                               \
                else                                                            \
                {                                                               \
                    task->tc_grp_next = TCD_App_Task_List;                      \
                    task->tc_grp_prev = TCD_App_Task_List->tc_grp_prev;         \
                    TCD_App_Task_List->tc_grp_prev->tc_grp_next = task;         \
                    TCD_App_Task_List->tc_grp_prev = task;                      \
                }                                                               \
                TCD_Total_App_Tasks++;

#define TCC_Application_Task_Remove(task);                                      \
                if(task->tc_grp_next == task)                                   \
                {                                                               \
                    TCD_App_Task_List = NU_NULL;                                \
                }                                                               \
                else                                                            \
                {                                                               \
                    if(task == TCD_App_Task_List)                               \
                    {                                                           \
                        TCD_App_Task_List = task->tc_grp_next;                  \
                    }                                                           \
                    task->tc_grp_next->tc_grp_prev = task->tc_grp_prev;         \
                    task->tc_grp_prev->tc_grp_next = task->tc_grp_next;         \
                }                                                               \
                TCD_Total_App_Tasks--;

#define TCC_Current_App_Task_Set(task_ptr);                                                 \
                                    /* Check if an application task */                      \
                                    if(task_ptr->tc_grp_id == TC_GRP_ID_APP)                \
                                    {                                                       \
                                        /* Set this task as the current application task */ \
                                        TCD_Current_App_Task = task_ptr;                    \
                                    }

/* Determine if pointers are accessible with a single instruction.  If so,
   just reference the pointer directly.  Otherwise, call the target dependent
   service */

#if (NU_PTR_ACCESS == 1)

/* Macro definitions for architectures supporting single instruction
   access to pointers */
#define         TCCT_Current_Thread()               TCD_Current_Thread
#define         TCCT_Set_Execute_Task(task)         TCD_Execute_Task =  task

#else

VOID            *TCCT_Current_Thread(VOID);
VOID            TCCT_Set_Execute_Task(TC_TCB *task);

#endif /* NU_PTR_ACCESS == 1 */

#define         TCC_Task_Priority(task)                                         \
                    ((TC_TCB *) (task)) -> tc_priority

/* Define macro, for internal components, to access current HISR pointer */

#define         TCC_CURRENT_HISR_PTR                                            \
                    ((TC_HCB *)TCD_Current_Thread)

/* Define interrupt locking / unlocking macros based on the interrupt locking
   method used */
#if (NU_GLOBAL_INT_LOCKING == NU_TRUE)

/* The following macro sets the interrupt lock-out level to the globally defined
   level (TCD_Interrupt_Level).  This macro is used in the scheduler and within
   the HISR shell routine. */
#define         TCC_INTERRUPTS_GLOBAL_ENABLE()                                  \
                        ESAL_GE_INT_BITS_SET(TCD_Interrupt_Level)

/* The following macro locks out interrupts without saving the state before
   locking-out.  It is used to guard critical sections of code and is always
   paired with the macro TCC_INTERRUPTS_RESTORE defined below. */
#define         TCC_INTERRUPTS_DISABLE()                                        \
                        ESAL_GE_INT_FAST_ALL_DISABLE()

/* The following macro sets the interrupt lock-out level to the globally defined
   level (TCD_Interrupt_Level).  This macro is used at the end of critical
   sections where a context switch may have occurred.  This macro is always
   paired with the macro TCC_INTERRUPTS_ENABLE defined above. */
#define         TCC_INTERRUPTS_RESTORE()                                        \
                        ESAL_GE_INT_BITS_SET(TCD_Interrupt_Level)

#else

/* The following macro is used to enable interrupts globally.  This macro is used
   in the scheduler and within the HISR shell routine. */
#define         TCC_INTERRUPTS_GLOBAL_ENABLE()                                  \
                        ESAL_GE_INT_FAST_ALL_ENABLE()

/* The following macro locks out interrupts and saves the state before
   locking-out.  It is used to guard critical sections of code and is always
   paired with the macro TCC_INTERRUPTS_RESTORE defined below. */
#define         TCC_INTERRUPTS_DISABLE()                                        \
                        ESAL_GE_INT_CONTROL_VARS                                \
                        ESAL_GE_INT_ALL_DISABLE()

/* The following macro sets the interrupt lock-out level to entry level saved
   by TCC_INTERRUPTS_LOCK.  This macro is used at the end of critical
   sections where a context switch may have occurred.  This macro is always
   paired with the macro TCC_INTERRUPTS_LOCK defined above. */
#define         TCC_INTERRUPTS_RESTORE()                                        \
                        ESAL_GE_INT_ALL_RESTORE()

#endif  /* NU_GLOBAL_INT_LOCKING == NU_TRUE */

/* Nucleus PLUS Timing Test 2 macros / external declarations */
#if (NU_TIME_TEST2_ENABLE == NU_TRUE)

/* Include timing test header file */
#include        "test2_defs.h"

#else

/* Define macro to do nothing */
#define         NU_PLUS_TIMING_TEST2()

#endif  /* (NU_TIME_TEST2_ENABLE == NU_TRUE) */

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif
