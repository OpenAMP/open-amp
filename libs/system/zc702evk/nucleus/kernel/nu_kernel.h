/***********************************************************************
*
*            Copyright 2011 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************

************************************************************************
*
*  DESCRIPTION
*
*       This file contains kernel constants common to both the
*       application and the actual Nucleus PLUS kernel.
*
***********************************************************************/

/* Check to see if this file has been included already.  */

#ifndef         NU_KERNEL
#ifdef          __cplusplus
/* C declarations in C++     */
extern          "C" {
#endif
#define         NU_KERNEL


/**********************************************************************/
/*                    Service Paramaeters                             */
/**********************************************************************/
/* Define constants for use in service parameters.  */
#define         NU_AND                              2
#define         NU_AND_CONSUME                      3
#define         NU_DISABLE_TIMER                    4
#define         NU_ENABLE_TIMER                     5
#define         NU_FIFO                             6
#define         NU_FIXED_SIZE                       7
#define         NU_NO_PREEMPT                       8
#define         NU_NO_START                         9
#define         NU_NO_SUSPEND                       0
#define         NU_OR                               0
#define         NU_OR_CONSUME                       1
#define         NU_PREEMPT                          10
#define         NU_PRIORITY                         11
#define         NU_START                            12
#define         NU_SUSPEND                          0xFFFFFFFFUL
#define         NU_VARIABLE_SIZE                    13
#define         NU_PRIORITY_INHERIT                 14

/* Define service completion status constants.  */
#define         NU_SUCCESS                          0
#define         NU_END_OF_LOG                       -1
#define         NU_GROUP_DELETED                    -2
#define         NU_INVALID_DELETE                   -3
#define         NU_INVALID_DRIVER                   -4
#define         NU_INVALID_ENABLE                   -5
#define         NU_INVALID_ENTRY                    -6
#define         NU_INVALID_FUNCTION                 -7
#define         NU_INVALID_GROUP                    -8
#define         NU_INVALID_HISR                     -9
#define         NU_INVALID_MAILBOX                  -10
#define         NU_INVALID_MEMORY                   -11
#define         NU_INVALID_MESSAGE                  -12
#define         NU_INVALID_OPERATION                -13
#define         NU_INVALID_PIPE                     -14
#define         NU_INVALID_POINTER                  -15
#define         NU_INVALID_POOL                     -16
#define         NU_INVALID_PREEMPT                  -17
#define         NU_INVALID_PRIORITY                 -18
#define         NU_INVALID_QUEUE                    -19
#define         NU_INVALID_RESUME                   -20
#define         NU_INVALID_SEMAPHORE                -21
#define         NU_INVALID_SIZE                     -22
#define         NU_INVALID_START                    -23
#define         NU_INVALID_SUSPEND                  -24
#define         NU_INVALID_TASK                     -25
#define         NU_INVALID_TIMER                    -26
#define         NU_INVALID_VECTOR                   -27
#define         NU_MAILBOX_DELETED                  -28
#define         NU_MAILBOX_EMPTY                    -29
#define         NU_MAILBOX_FULL                     -30
#define         NU_MAILBOX_RESET                    -31
#define         NU_NO_MEMORY                        -32
#define         NU_NO_MORE_LISRS                    -33
#define         NU_NO_PARTITION                     -34
#define         NU_NOT_DISABLED                     -35
#define         NU_NOT_PRESENT                      -36
#define         NU_NOT_REGISTERED                   -37
#define         NU_NOT_TERMINATED                   -38
#define         NU_PIPE_DELETED                     -39
#define         NU_PIPE_EMPTY                       -40
#define         NU_PIPE_FULL                        -41
#define         NU_PIPE_RESET                       -42
#define         NU_POOL_DELETED                     -43
#define         NU_QUEUE_DELETED                    -44
#define         NU_QUEUE_EMPTY                      -45
#define         NU_QUEUE_FULL                       -46
#define         NU_QUEUE_RESET                      -47
#define         NU_SEMAPHORE_DELETED                -48
#define         NU_SEMAPHORE_RESET                  -49
#define         NU_TIMEOUT                          -50
#define         NU_UNAVAILABLE                      -51
#define         NU_INVALID_DESCRIPTION              -52
#define         NU_INVALID_REGION                   -53
#define         NU_MEMORY_CORRUPT                   -54
#define         NU_INVALID_DEBUG_ALLOCATION         -55
#define         NU_EMPTY_DEBUG_ALLOCATION_LIST      -56
#define         NU_HISR_ACTIVATION_COUNT_ERROR      -57
#define         NU_ZC_INVALID_PTR                   -58
#define         NU_ZC_INVALID_BUF_ID                -59
#define         NU_ZC_BUF_SEG_MISMATCH              -60
#define         NU_ZC_INVALID_SEG_OFFSET            -61
#define         NU_ZC_BUF_EMPTY                     -62
#define         NU_ZC_INSUFFICIENT_MEMORY           -63
#define         NU_ZC_INIT_ALREADY_COMPLETE         -64
#define         NU_EN_INVALID_INPUT_PARAMS          -65
#define         NU_EN_ALREADY_LISTENING             -66
#define         NU_EN_NO_AVAIL_LISTEN_ENTRY         -67
#define         NU_EN_NO_ACTIVE_LISTENERS           -68
#define         NU_EN_LISTENER_NOT_FOUND            -69
#define         NU_INVALID_RESUME_TIMER             -70
#define         NU_INVALID_PAUSE_TIMER              -71
#define         NU_TIMER_PAUSED                     -72
#define         NU_INVALID_COUNT                    -73
#define         NU_SEMAPHORE_INVALID_OWNER          -74
#define         NU_NO_WAITING_TASKS                 -75
#define         NU_SEMAPHORE_ALREADY_OWNED          -76
#define         NU_EQM_INVALID_EVENT_SIZE           -77
#define         NU_EQM_INVALID_HANDLE               -78
#define         NU_EQM_EVENT_EXPIRED                -79
#define         NU_EQM_EVENT_WITHOUT_DATA           -80
#define         NU_EQM_INVALID_INPUT                -81
#define         NU_NOT_ALIGNED                      -82
#define         NU_SEMAPHORE_COUNT_ROLLOVER         -83
#define         NU_INVALID_PROCESS                  -84
#define         NU_INVALID_STATE                    -85
#define         NU_PROCESS_IN_TRANSITION            -86
#define         NU_SEMAPHORE_OWNER_DEAD             -87
#define         NU_SYMBOLS_IN_USE                   -88
#define         NU_INVALID_MEMORY_REGION            -89
#define         NU_MEMORY_IS_SHARED                 -90
#define         NU_MEMORY_OVERLAPS                  -91
#define         NU_INVALID_OPTIONS                  -92

/**********************************************************************/
/*                 Processes Configuration                            */
/**********************************************************************/

#include        "kernel/proc_extern.h"

/* Legacy macros */
#define         NU_SUPERVISOR_MODE()
#define         NU_USER_MODE()
#define         NU_SUPERV_USER_VARIABLES
#define         NU_SUPERVISOR_MODE_ISR()
#define         NU_USER_MODE_ISR()
#define         NU_BIND_TASK_TO_KERNEL(task)    NU_SUCCESS
#define         NU_BIND_HISR_TO_KERNEL(hisr)    NU_SUCCESS

/**********************************************************************/
/*                          Kernel Support                            */
/**********************************************************************/

/* Define the basic data structure templates.  */
#include        "kernel/plus_core.h"
#include        "kernel/dev_mgr.h"
#include        "kernel/eqm.h"
#include        "kernel/rtl_extr.h"
#include        "kernel/esal_extr.h"

#ifdef CFG_NU_OS_KERN_PLUS_SUPPLEMENT_ENABLE
#include        "kernel/plus_supplement.h"
#endif  /* CFG_NU_OS_KERN_PLUS_SUPPLEMENT_ENABLE */

/* Define Nucleus PLUS system interface */
VOID            Application_Initialize(NU_MEMORY_POOL*, NU_MEMORY_POOL*);
NU_WEAK_REF(VOID Pre_Kernel_Init_Hook(NU_MEMORY_POOL*, NU_MEMORY_POOL*));

/**********************************************************************/
/*                    Support Definitions                             */
/**********************************************************************/
/* Include Nucleus PLUS configuration settings */
#include        "kernel/plus_cfg.h"


#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif  /* !NU_KERNEL */
