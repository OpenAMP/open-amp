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
*   DESCRIPTION
*
*       This file contains external data structures and definitions of
*       all PLUS Supplementary components.
*
***********************************************************************/

#ifndef PLUS_SUPPLEMENT_H
#define PLUS_SUPPLEMENT_H

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

/**********************************************************************/
/*                  Test Configuration Settings                       */
/**********************************************************************/

/* DEFINE:      NU_TIME_TEST1MIN_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to perform the minimum code / data size tests that are
                part of the Nucleus PLUS timing test.  Setting this define to NU_TRUE will
                allow the minimal sized PLUS code and data to be built.
   NOTE:        The Nucleus PLUS library and timing test must be rebuilt after changing
                this define.            */
#ifndef         NU_TIME_TEST1MIN_ENABLE
#define         NU_TIME_TEST1MIN_ENABLE             CFG_NU_OS_KERN_PLUS_SUPPLEMENT_TIME_TEST1MIN
#endif

/* DEFINE:      NU_TIME_TEST1MAX_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to perform the maximum code / data size tests that are
                part of the Nucleus PLUS timing test.  Setting this define to NU_TRUE will
                allow the maximum sized PLUS code and data to be built.
   NOTE:        The Nucleus PLUS library and timing test must be rebuilt after changing
                this define.            */
#ifndef         NU_TIME_TEST1MAX_ENABLE
#define         NU_TIME_TEST1MAX_ENABLE             CFG_NU_OS_KERN_PLUS_SUPPLEMENT_TIME_TEST1MAX
#endif

/* DEFINE:      NU_TIME_TEST2_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to include code used for the Nucleus PLUS timing
                test #2.  Setting this define to NU_TRUE will include the necessary
                timing related code.
   NOTE:        The Nucleus PLUS library and timing test must be rebuilt after changing
                this define.            */
#ifndef         NU_TIME_TEST2_ENABLE
#define         NU_TIME_TEST2_ENABLE                CFG_NU_OS_KERN_PLUS_SUPPLEMENT_TIME_TEST2
#endif

/* DEFINE:      NU_TIME_TEST3_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to include code used for the Nucleus PLUS timing
                test #3.  Setting this define to NU_TRUE will include the necessary
                timing related code.
   NOTE:        The Nucleus PLUS library and timing test must be rebuilt after changing
                this define.            */
#ifndef         NU_TIME_TEST3_ENABLE
#define         NU_TIME_TEST3_ENABLE                CFG_NU_OS_KERN_PLUS_SUPPLEMENT_TIME_TEST3
#endif

/* DEFINE:      NU_STATIC_TEST_ENABLE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define allows visibility of static data / functions during debug
                sessions by changing their scope from static to global when enabled.
   NOTE:        The Nucleus PLUS library, all middleware, and application must be rebuilt
                after changing this define.            */
#ifndef         NU_STATIC_TEST_ENABLE
#define         NU_STATIC_TEST_ENABLE               CFG_NU_OS_KERN_PLUS_SUPPLEMENT_STATIC_TEST
#endif

/**********************************************************************/
/*                 Other Configuration Settings                       */
/**********************************************************************/
/* DEFINE:      NU_PLUS_OBJECT_LISTS_INCLUDE
   DEFAULT:     NU_FALSE
   DESCRIPTION: This define is used to include ALL of the Nucleus PLUS created object lists
                within the executable image of Nucleus PLUS.  Setting this define to NU_TRUE
                will explicitly include each of the object list pointers during
                initialization.  Setting this define to NU_FALSE will only include the
                object lists required for the given application.
   NOTE:        This define may need to be set to NU_TRUE to allow kernel awareness to
                correctly function on certain third party debuggers.. Some kernel aware
                debuggers are not able to display any list of OS objects unless all lists are
                in the image.
   NOTE:        The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_PLUS_OBJECT_LISTS_INCLUDE
#define         NU_PLUS_OBJECT_LISTS_INCLUDE        CFG_NU_OS_KERN_PLUS_SUPPLEMENT_PLUS_OBJECT_LISTS
#endif

/**********************************************************************/
/*                    Support Definitions                             */
/**********************************************************************/

/* Define for "static" variables / data used to allow testing
   of internal functions using external drivers / stub functions */
#if (NU_STATIC_TEST_ENABLE == NU_TRUE)

/* Define STATIC as nothing to make internal functions / variables
   globally visible */
#define             STATIC

#else

/* Define STATIC as static to make internal functions / variables
   have file level scope */
#define             STATIC                      static

#endif  /* NU_STATIC_TEST_ENABLE == NU_TRUE */

/* Check if multiple timing tests being configured */
#if ((NU_TIME_TEST1MIN_ENABLE + NU_TIME_TEST1MAX_ENABLE +    \
      NU_TIME_TEST2_ENABLE + NU_TIME_TEST3_ENABLE) > 1)

/* Show an error - only 1 timing test can be configured */
#error          Only 1 timing test may be configured

#endif

/* Check if timing test 1 (minimum code / data) is enabled */
#if (NU_TIME_TEST1MIN_ENABLE == NU_TRUE)

/* Ensure all configuration settings build minimal code / data */
#undef  NU_ERROR_CHECKING
#define NU_ERROR_CHECKING               NU_FALSE
#undef  NU_PLUS_INLINING
#define NU_PLUS_INLINING                NU_FALSE
#undef  NU_MIN_RAM_ENABLED
#define NU_MIN_RAM_ENABLED              NU_TRUE
#undef  NU_PLUS_OBJECT_LISTS_INCLUDE
#define NU_PLUS_OBJECT_LISTS_INCLUDE    NU_FALSE

#endif  /* NU_TIME_TEST1MIN_ENABLE == NU_TRUE */

/* Check if timing test 1 (maximum code / data) is enabled */
#if (NU_TIME_TEST1MAX_ENABLE == NU_TRUE)

/* Ensure all configuration settings build maximum code / data */
#undef  NU_ERROR_CHECKING
#define NU_ERROR_CHECKING               NU_TRUE
#undef  NU_PLUS_INLINING
#define NU_PLUS_INLINING                NU_TRUE
#undef  NU_MIN_RAM_ENABLED
#define NU_MIN_RAM_ENABLED              NU_FALSE
#undef  NU_PLUS_OBJECT_LISTS_INCLUDE
#define NU_PLUS_OBJECT_LISTS_INCLUDE    NU_TRUE

#endif  /* NU_TIME_TEST1MAX_ENABLE == NU_TRUE */

/* Check if timing test 2 (API timing) or timing test 3 (interrupt latency)
   is enabled */
#if (NU_TIME_TEST2_ENABLE == NU_TRUE) || (NU_TIME_TEST3_ENABLE == NU_TRUE)

/* Ensure all configuration settings build for speed */
#undef  NU_ERROR_CHECKING
#define NU_ERROR_CHECKING               NU_FALSE
#undef  NU_PLUS_INLINING
#define NU_PLUS_INLINING                NU_TRUE
#undef  NU_MIN_RAM_ENABLED
#define NU_MIN_RAM_ENABLED              NU_FALSE

#endif  /* NU_TIME_TEST2_ENABLE == NU_TRUE */


/**********************************************************************/
/*                EVENT NOTIFICATIONS Definitions                     */
/**********************************************************************/

/* Maximum number of listeners */
#define EN_MAX_LISTEN_CNT   10

/* Minimum number of notification messages per pipe */
#define EN_MIN_MSG_CNT      4

/* Maximum length of notification message */
#define EN_MAX_MSG_LEN      255

/**********************************************************************/
/*                     MAILBOX Definitions                            */
/**********************************************************************/
#define         MB_MESSAGE_SIZE         4

/* Define the Mailbox Control Block data type.  */
typedef struct MB_MCB_STRUCT
{
    CS_NODE             mb_created;            /* Node for linking to    */
                                               /* created mailbox list   */
    UNSIGNED            mb_id;                 /* Internal MCB ID        */
    CHAR                mb_name[NU_MAX_NAME];  /* Mailbox name           */
    BOOLEAN             mb_message_present;    /* Message present flag   */
    BOOLEAN             mb_fifo_suspend;       /* Suspension type flag   */
#if     PAD_2
    DATA_ELEMENT        mb_padding[PAD_2];
#endif
    UNSIGNED            mb_tasks_waiting;      /* Number of waiting tasks*/
    UNSIGNED                                   /* Message area           */
                        mb_message_area[MB_MESSAGE_SIZE];
    struct MB_SUSPEND_STRUCT
                       *mb_suspension_list;    /* Suspension list        */
} MB_MCB;


/**********************************************************************/
/*                       PIPE Definitions                             */
/**********************************************************************/

/* Define the Pipe Control Block data type.  */
typedef struct PI_PCB_STRUCT
{
    CS_NODE             pi_created;            /* Node for linking to    */
                                               /*   created pipe list    */
    UNSIGNED            pi_id;                 /* Internal PCB ID        */
    CHAR                pi_name[NU_MAX_NAME];  /* Pipe name              */
    BOOLEAN             pi_fixed_size;         /* Fixed-size messages?   */
    BOOLEAN             pi_fifo_suspend;       /* Suspension type flag   */
#if     PAD_2
    DATA_ELEMENT        pi_padding[PAD_2];
#endif
    UNSIGNED            pi_pipe_size;          /* Total size of pipe     */
    UNSIGNED            pi_messages;           /* Messages in pipe       */
    UNSIGNED            pi_message_size;       /* Size of each message   */
    UNSIGNED            pi_available;          /* Available bytes        */
    BYTE_PTR            pi_start;              /* Start of pipe area     */
    BYTE_PTR            pi_end;                /* End of pipe area + 1   */
    BYTE_PTR            pi_read;               /* Read pointer           */
    BYTE_PTR            pi_write;              /* Write pointer          */
    UNSIGNED            pi_tasks_waiting;      /* Number of waiting tasks*/
    struct PI_SUSPEND_STRUCT
                       *pi_urgent_list;        /* Urgent message suspend */
    struct PI_SUSPEND_STRUCT
                       *pi_suspension_list;    /* Suspension list        */
} PI_PCB;

/**********************************************************************/
/*                 PARTITION MEMORY Definitions                       */
/**********************************************************************/

/* Define the Partition Pool Control Block data type.  */
typedef struct PM_PCB_STRUCT
{
    CS_NODE             pm_created;            /* Node for linking to    */
                                               /* created partition list */
    UNSIGNED            pm_id;                 /* Internal PCB ID        */
    CHAR                pm_name[NU_MAX_NAME];  /* Partition Pool name    */
    VOID               *pm_start_address;      /* Starting pool address  */
    UNSIGNED            pm_pool_size;          /* Size of pool           */
    UNSIGNED            pm_partition_size;     /* Size of each partition */
    UNSIGNED            pm_available;          /* Available partitions   */
    UNSIGNED            pm_allocated;          /* Allocated partitions   */
    struct PM_HEADER_STRUCT
                       *pm_available_list;     /* Available list         */
    BOOLEAN             pm_fifo_suspend;       /* Suspension type flag   */
#if     PAD_1
    DATA_ELEMENT        pm_padding[PAD_1];
#endif
    UNSIGNED            pm_tasks_waiting;      /* Number of waiting tasks*/
    struct PM_SUSPEND_STRUCT
                       *pm_suspension_list;    /* Suspension list        */
} PM_PCB;

/**********************************************************************/
/*                      PLUS Definitions                              */
/**********************************************************************/
typedef         MB_MCB                              NU_MAILBOX;
typedef         PI_PCB                              NU_PIPE;
typedef         PM_PCB                              NU_PARTITION_POOL;

#define         NU_Release_Information()            PLUS_RELEASE_STRING

#if (CFG_NU_OS_KERN_PLUS_SUPPLEMENT_EVT_NOTIFY == NU_TRUE)

#define         ENC_Notification_Send               NU_Notification_Send

#endif /* CFG_NU_OS_KERN_PLUS_SUPPLEMENT_EVT_NOTIFY == NU_TRUE */

/* Define Mailbox management functions.  */
STATUS          NU_Create_Mailbox(NU_MAILBOX *mailbox, CHAR *name,
                                  OPTION suspend_type);
STATUS          NU_Delete_Mailbox(NU_MAILBOX *mailbox);
STATUS          NU_Reset_Mailbox(NU_MAILBOX *mailbox);
STATUS          NU_Send_To_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                   UNSIGNED suspend);
STATUS          NU_Broadcast_To_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                        UNSIGNED suspend);
STATUS          NU_Receive_From_Mailbox(NU_MAILBOX *mailbox, VOID *message,
                                        UNSIGNED suspend);
UNSIGNED        NU_Established_Mailboxes(VOID);
STATUS          NU_Mailbox_Information(NU_MAILBOX *mailbox, CHAR *name,
                  OPTION *suspend_type, OPTION *message_present,
                  UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        NU_Mailbox_Pointers(NU_MAILBOX **pointer_list,
                                    UNSIGNED maximum_pointers);

/* Define Pipe management functions.  */
STATUS          NU_Create_Pipe(NU_PIPE *pipe, CHAR *name,
                               VOID *start_address, UNSIGNED pipe_size,
                               OPTION message_type, UNSIGNED message_size,
                               OPTION suspend_type);
STATUS          NU_Delete_Pipe(NU_PIPE *pipe);
STATUS          NU_Reset_Pipe(NU_PIPE *pipe);
STATUS          NU_Send_To_Front_Of_Pipe(NU_PIPE *pipe, VOID *message,
                                         UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Send_To_Pipe(NU_PIPE *pipe, VOID *message,
                                UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Broadcast_To_Pipe(NU_PIPE *pipe, VOID *message,
                                     UNSIGNED size, UNSIGNED suspend);
STATUS          NU_Receive_From_Pipe(NU_PIPE *pipe, VOID *message,
                                     UNSIGNED size, UNSIGNED *actual_size,
                                     UNSIGNED suspend);
UNSIGNED        NU_Established_Pipes(VOID);
STATUS          NU_Pipe_Information(NU_PIPE *pipe, CHAR *name,
                                    VOID **start_address, UNSIGNED *pipe_size,
                                    UNSIGNED *available, UNSIGNED *messages,
                                    OPTION *message_type, UNSIGNED *message_size,
                                    OPTION *suspend_type, UNSIGNED *tasks_waiting,
                                    NU_TASK **first_task);
UNSIGNED        NU_Pipe_Pointers(NU_PIPE **pointer_list,
                                 UNSIGNED maximum_pointers);

/* Define Signal processing functions.  */
UNSIGNED        NU_Control_Signals(UNSIGNED signal_enable_mask);
UNSIGNED        NU_Receive_Signals(VOID);
STATUS          NU_Register_Signal_Handler(VOID (*signal_handler)(UNSIGNED));
STATUS          NU_Send_Signals(NU_TASK *task, UNSIGNED signal_mask);

/* Define Partition memory management functions.  */
STATUS          NU_Create_Partition_Pool(NU_PARTITION_POOL *pool, CHAR *name,
                                         VOID *start_address, UNSIGNED pool_size,
                                         UNSIGNED partition_size, OPTION suspend_type);
STATUS          NU_Delete_Partition_Pool(NU_PARTITION_POOL *pool);
STATUS          NU_Allocate_Partition(NU_PARTITION_POOL *pool,
                                      VOID **return_pointer, UNSIGNED suspend);
STATUS          NU_Deallocate_Partition(VOID *partition);
UNSIGNED        NU_Established_Partition_Pools(VOID);
STATUS          NU_Partition_Pool_Information(NU_PARTITION_POOL *pool,
                                              CHAR *name,
                                              VOID **start_address,
                                              UNSIGNED *pool_size,
                                              UNSIGNED *partition_size,
                                              UNSIGNED *available,
                                              UNSIGNED *allocated,
                                              OPTION *suspend_type,
                                              UNSIGNED *tasks_waiting,
                                              NU_TASK **first_task);
UNSIGNED        NU_Partition_Pool_Pointers(NU_PARTITION_POOL **pointer_list,
                                           UNSIGNED maximum_pointers);

/* Development support functions.  */
VOID            NU_Get_Release_Version(UINT* major, UINT* minor);

#if (CFG_NU_OS_KERN_PLUS_SUPPLEMENT_EVT_NOTIFY == NU_TRUE)

/* Event notification functions.  */
VOID            *NU_Notification_Queue_Create (VOID) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Listen_Start (VOID* handle, DV_DEV_ID dev_id,
                                       UINT32 type, UINT32 type_mask) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Send (DV_DEV_ID dev_id, UINT32 type, VOID* msg, UINT8 msg_len) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Get (VOID *handle, DV_DEV_ID *dev_id_ptr, UINT32 *type_ptr,
                                     VOID **msg_ptr, UINT8 *msg_len_ptr, UNSIGNED suspend) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Listen_Stop (VOID* handle, DV_DEV_ID dev_id) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Queue_Delete (VOID* handle) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Register (DV_DEV_LABEL label, DV_DEV_ID* sender_id) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Unregister (DV_DEV_ID sender_id) ESAL_TS_RTE_DEPRECATED;
STATUS          NU_Notification_Find_Sender (DV_DEV_LABEL label_list[], UINT32 label_cnt,
                                             DV_DEV_ID sender_id_list[], UINT32* sender_id_cnt) ESAL_TS_RTE_DEPRECATED;

#endif /* CFG_NU_OS_KERN_PLUS_SUPPLEMENT_EVT_NOTIFY == NU_TRUE */

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif
