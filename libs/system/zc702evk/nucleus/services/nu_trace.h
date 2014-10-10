/***********************************************************************
*
*            Copyright 2012 Mentor Graphics Corporation
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
*       TRACE components.
*
***********************************************************************/

#ifndef _NU_TRACE_H_
#define _NU_TRACE_H_

#ifdef          __cplusplus
extern          "C" {   /* C declarations in C++     */
#endif

#include <stdarg.h>

/* Mask definitions for each kernel object */
#define NU_TRACE_ALL                    0xFFFFFFFF
#define NU_TRACE_APP                    (1<<0x0)
#define NU_TRACE_KERN                   (~NU_TRACE_APP)    /* Enable all kernel hooks */
#define NU_TRACE_LISRS                  (1<<0x1)
#define NU_TRACE_HISRS                  (1<<0x2)
#define NU_TRACE_TASKS                  (1<<0x3)
#define NU_TRACE_DMEM                   (1<<0x4)
#define NU_TRACE_PMEM                   (1<<0x5)
#define NU_TRACE_MAILBOX                (1<<0x6)
#define NU_TRACE_QUEUE                  (1<<0x7)
#define NU_TRACE_PIPE                   (1<<0x8)
#define NU_TRACE_SEMAPHORE              (1<<0x9)
#define NU_TRACE_EVENT                  (1<<0xA)
#define NU_TRACE_SIGNAL                 (1<<0xB)
#define NU_TRACE_TIMER                  (1<<0xC)
#define NU_TRACE_CPU_STATE              (1<<0xD)
#define NU_TRACE_PMS_INFO               (1<<0xE)
#define NU_TRACE_STOR_INFO              (1<<0XF)
#define NU_TRACE_NET_INFO               (1<<0X10)

#define NU_TRACE_KERN_INFO              (NU_TRACE_LISRS | NU_TRACE_HISRS | NU_TRACE_TASKS |     \
                                         NU_TRACE_DMEM | NU_TRACE_PMEM | NU_TRACE_MAILBOX |     \
                                         NU_TRACE_QUEUE | NU_TRACE_PIPE | NU_TRACE_SEMAPHORE |  \
                                         NU_TRACE_EVENT | NU_TRACE_SIGNAL | NU_TRACE_TIMER |    \
                                         NU_TRACE_CPU_STATE)

/* Definitions for trace communications */
#define NU_TRACE_TRANSMIT_ALL           0xFFFFFFFF

/* Error definitions */
#define NU_TRACE_INIT_ERROR             -1
#define NU_TRACE_ALREADY_INITIALIZED    (NU_TRACE_INIT_ERROR - 1)
#define NU_TRACE_NOT_INITIALIZED        (NU_TRACE_ALREADY_INITIALIZED - 1)
#define NU_TRACE_BUFF_FULL              (NU_TRACE_NOT_INITIALIZED - 1)
#define NU_TRACE_BUFF_EMPTY             (NU_TRACE_BUFF_FULL - 1)
#define NU_TRACE_INVALID_ARGS           (NU_TRACE_BUFF_EMPTY - 1)
#define NU_TRACE_COMMS_ERROR            (NU_TRACE_INVALID_ARGS -1)
#define NU_TRACE_COMMS_NOT_INITIALIZED  (NU_TRACE_COMMS_ERROR - 1)
#define NU_TRACE_COMMS_ALREADY_STARTED  (NU_TRACE_COMMS_NOT_INITIALIZED - 1)
#define NU_TRACE_COMMS_NOT_STARTED      (NU_TRACE_COMMS_ALREADY_STARTED - 1)
#define NU_TRACE_KERN_NOT_AVAILABLE     (NU_TRACE_COMMS_NOT_STARTED - 1)
#define NU_TRACE_ERROR_CODE_DELIMITER   (NU_TRACE_KERN_NOT_AVAILABLE - 1)

#ifdef CFG_NU_OS_SVCS_TRACE_ENABLE

/* This macro invoke trace log if global mask is enabled for the kernel object requested */
extern unsigned long                    Gbl_Trace_Mask;

#if ((CFG_NU_OS_SVCS_TRACE_CORE_TRACE_SUPPORT == NU_TRUE) && (CFG_NU_OS_SVCS_TRACE_CORE_TRACK_TRACE_OVERHEAD == NU_TRUE))

/* Trace log function prototype */
extern void    Log_pCU64U64(unsigned short evt_id, char* pC, unsigned long long u64_1, unsigned long long u64_2);
extern UINT64  NU_Get_Time_Stamp();

/* API latency trace event ID */
#define TRACE_APPLY_MASK(_msk_bit_, _evt_str_, _trace_fn_)                      \
{                                                                               \
    unsigned long long   _start;                                                \
    unsigned long long  _finish;                                                \
                                                                                \
    if(_msk_bit_ & Gbl_Trace_Mask)                                              \
    {                                                                           \
        _start = NU_Get_Time_Stamp();                                           \
        _trace_fn_;                                                             \
        _finish = NU_Get_Time_Stamp();                                          \
        Log_pCU64U64(0xFE, _evt_str_, _finish, _start);                         \
    }                                                                           \
}
#else
#define TRACE_APPLY_MASK(_msk_bit_, _evt_str_, _trace_fn_)                      \
{                                                                               \
    if(_msk_bit_ & Gbl_Trace_Mask)                                              \
    {                                                                           \
        _trace_fn_;                                                             \
    }                                                                           \
}
#endif /* ((CFG_NU_OS_SVCS_TRACE_CORE_TRACE_SUPPORT == NU_TRUE) && (CFG_NU_OS_SVCS_TRACE_CORE_TRACK_TRACE_OVERHEAD == NU_TRUE)) */

/* User API interface */
STATUS  NU_Trace_Initialize(VOID);
STATUS  NU_Trace_Deinitialize(VOID);
STATUS  NU_Trace_Arm(UINT32 mask);
STATUS  NU_Trace_Disarm(UINT32 mask);
UINT32  NU_Trace_Get_Mask(VOID);

/* External definitions */
extern void Trace_Mark_I32(char* evt_str, signed int i32_val);
extern void Trace_Mark_U32(char* evt_str, unsigned int u32_val);
extern void Trace_Mark_Float(char* evt_str, float float_val);
extern void Trace_Mark_String(char* evt_str, char* str_val);
extern void Trace_Mark(char *event_type, char* format, ...);

#define NU_Trace_Mark_I32(evt_str, i32_val)        TRACE_APPLY_MASK(NU_TRACE_APP, evt_str, Trace_Mark_I32(evt_str, i32_val))
#define NU_Trace_Mark_U32(evt_str, u32_val)        TRACE_APPLY_MASK(NU_TRACE_APP, evt_str, Trace_Mark_U32(evt_str, u32_val))
#define NU_Trace_Mark_Float(evt_str, float_val)    TRACE_APPLY_MASK(NU_TRACE_APP, evt_str, Trace_Mark_Float(evt_str, float_val))
#define NU_Trace_Mark_String(evt_str, str_val)     TRACE_APPLY_MASK(NU_TRACE_APP, evt_str, Trace_Mark_String(evt_str, str_val))
#define NU_Trace_Mark                              Trace_Mark

#if(defined(CFG_NU_OS_SVCS_TRACE_COMMS_ENABLE) && (CFG_NU_OS_SVCS_TRACE_COMMS_CHANNEL != DBG_INTERFACE))

STATUS  Trace_Comms_Start(OPTION comms_tsk_priority, UINT32 comms_tx_period);
STATUS  Trace_Comms_Stop(VOID);
STATUS  Trace_Comms_Flush(VOID);
STATUS  Trace_Comms_Transmit_N_Packets(UINT32 num_packets, UINT32* num_pkts_transmitted);

#define	NU_Trace_Comms_Start                Trace_Comms_Start
#define	NU_Trace_Comms_Stop                 Trace_Comms_Stop
#define	NU_Trace_Comms_Flush                Trace_Comms_Flush
#define	NU_Trace_Comms_Transmit_N_Packets   Trace_Comms_Transmit_N_Packets

#else

#define NU_Trace_Comms_Start
#define NU_Trace_Comms_Stop
#define NU_Trace_Comms_Flush
#define NU_Trace_Comms_Transmit_N_Packets

#endif /* (defined(CFG_NU_OS_SVCS_TRACE_COMMS_ENABLE) && (CFG_NU_OS_SVCS_TRACE_COMMS_CHANNEL != DBG_INTERFACE)) */

#else

/* Dummy definitions to avoid build errors when Trace is disabled at build-time */
#define  NU_Trace_Initialize()                      NU_SUCCESS
#define  NU_Trace_Deinitialize()                    NU_SUCCESS
#define  NU_Trace_Arm(mask)                         NU_SUCCESS
#define  NU_Trace_Disarm(mask)                      NU_SUCCESS
#define  NU_Trace_Get_Mask()                        0

#define NU_Trace_Mark_I32(evt_str, i32_val)
#define NU_Trace_Mark_U32(evt_str, u32_val)
#define NU_Trace_Mark_Float(evt_str, float_val)
#define NU_Trace_Mark_String(evt_str, str_val)
#define NU_Trace_Mark

#endif /* CFG_NU_OS_SVCS_TRACE_ENABLE */

#ifdef          __cplusplus
}   /* End of C declarations */
#endif  /* __cplusplus */

#endif /* _NU_TRACE_H_ */
