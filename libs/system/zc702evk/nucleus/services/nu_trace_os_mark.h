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

#ifndef _OS_MARK_H_
#define _OS_MARK_H_

#ifdef          __cplusplus
extern          "C" {   /* C declarations in C++     */
#endif

#include        "nucleus.h"
#include        "services/nu_trace.h"

/* Build configuration that enables or disables kernel markers */
#if (CFG_NU_OS_SVCS_TRACE_CORE_TRACE_SUPPORT == NU_TRUE)

extern unsigned int    idle_flag;

/*******************/
/* Event ID MACROs */
/*******************/

/* Event ID definitions for various kernel markers */
#define KRN_EID_BASE                    0x0

/* IRQs */
#define IRQ_ENABLE_EID                  (KRN_EID_BASE + 1)
#define IRQ_DSABLE_EID                  (KRN_EID_BASE + 2)

/* LISRs */
#define LISR_REGED_EID                  (KRN_EID_BASE + 3)
#define LISR_ENTRY_EID                  (KRN_EID_BASE + 4)
#define LISR_EXITT_EID                  (KRN_EID_BASE + 5)

/* HISRs */
#define HISR_CREAT_EID                  (KRN_EID_BASE + 6)
#define HISR_DELET_EID                  (KRN_EID_BASE + 7)
#define HISR_ACTIV_EID                  (KRN_EID_BASE + 8)
#define HISR_RUNIG_EID                  (KRN_EID_BASE + 9)
#define HISR_STOPD_EID                  (KRN_EID_BASE + 10)

/* Tasks */
#define TASK_CREAT_EID                  (KRN_EID_BASE + 11)
#define TASK_RESET_EID                  (KRN_EID_BASE + 12)
#define TASK_READY_EID                  (KRN_EID_BASE + 13)
#define TASK_RUNIG_EID                  (KRN_EID_BASE + 14)
#define TASK_SSUSP_EID                  (KRN_EID_BASE + 15)
#define TASK_TERMT_EID                  (KRN_EID_BASE + 16)
#define TASK_DELET_EID                  (KRN_EID_BASE + 17)
#define TASK_CHPRE_EID                  (KRN_EID_BASE + 18)
#define TASK_CHPRO_EID                  (KRN_EID_BASE + 19)
#define TASK_CHSLC_EID                  (KRN_EID_BASE + 20)

/* Idle */
#define IDLE_ENTRY_EID                  (KRN_EID_BASE + 21)
#define IDLE_EXIT_EID                   (KRN_EID_BASE + 22)

/* Dynamic memory */
#define MEM_CREATE_EID                  (KRN_EID_BASE + 23)
#define MEM_DELETE_EID                  (KRN_EID_BASE + 24)
#define MEM_ADD_EID                     (KRN_EID_BASE + 25)
#define DMEM_ALLOC_EID                  (KRN_EID_BASE + 26)
#define DMEM_DALLO_EID                  (KRN_EID_BASE + 27)
#define DMEM_RALLO_EID                  (KRN_EID_BASE + 28)

/* Partitioned memory */
#define PMEM_CREAT_EID                  (KRN_EID_BASE + 29)
#define PMEM_DELET_EID                  (KRN_EID_BASE + 30)
#define PMEM_ALLOC_EID                  (KRN_EID_BASE + 31)
#define PMEM_DEALL_EID                  (KRN_EID_BASE + 32)

/* Semaphores */
#define SEMA_CREAT_EID                  (KRN_EID_BASE + 33)
#define SEMA_RESET_EID                  (KRN_EID_BASE + 34)
#define SEMA_OBTAI_EID                  (KRN_EID_BASE + 35)
#define SEMA_RELSE_EID                  (KRN_EID_BASE + 36)
#define SEMA_DELET_EID                  (KRN_EID_BASE + 37)

/* Events */
#define EVEN_CREAT_EID                  (KRN_EID_BASE + 38)
#define EVEN_SET_EID                    (KRN_EID_BASE + 39)
#define EVEN_RETRV_EID                  (KRN_EID_BASE + 40)
#define EVEN_DELET_EID                  (KRN_EID_BASE + 41)

/* Queues */
#define QUEU_CREAT_EID                  (KRN_EID_BASE + 42)
#define QUEU_RESET_EID                  (KRN_EID_BASE + 43)
#define QUEU_BRBND_EID                  (KRN_EID_BASE + 44)
#define QUEU_SEND_EID                   (KRN_EID_BASE + 45)
#define QUEU_SD2FT_EID                  (KRN_EID_BASE + 46)
#define QUEU_RECVE_EID                  (KRN_EID_BASE + 47)
#define QUEU_DELET_EID                  (KRN_EID_BASE + 48)

/* Pipes */
#define PIPE_CREAT_EID                  (KRN_EID_BASE + 49)
#define PIPE_RESET_EID                  (KRN_EID_BASE + 50)
#define PIPE_BRBND_EID                  (KRN_EID_BASE + 51)
#define PIPE_SEND_EID                   (KRN_EID_BASE + 52)
#define PIPE_SD2FT_EID                  (KRN_EID_BASE + 53)
#define PIPE_RECVE_EID                  (KRN_EID_BASE + 54)
#define PIPE_DELET_EID                  (KRN_EID_BASE + 55)

/* Mailbox */
#define MBOX_CREAT_EID                  (KRN_EID_BASE + 56)
#define MBOX_RESET_EID                  (KRN_EID_BASE + 57)
#define MBOX_BRBND_EID                  (KRN_EID_BASE + 58)
#define MBOX_SEND_EID                   (KRN_EID_BASE + 59)
#define MBOX_RECVE_EID                  (KRN_EID_BASE + 60)
#define MBOX_DELET_EID                  (KRN_EID_BASE + 61)

/* Signals */
#define SGNL_REGSR_EID                  (KRN_EID_BASE + 62)
#define SGNL_CNTRL_EID                  (KRN_EID_BASE + 63)
#define SGNL_SEND_EID                   (KRN_EID_BASE + 64)
#define SGNL_RECVE_EID                  (KRN_EID_BASE + 65)
#define SGNL_RUNIN_EID                  (KRN_EID_BASE + 66)
#define SGNL_STOPD_EID                  (KRN_EID_BASE + 67)

/* Timers */
#define TIME_CREAT_EID                  (KRN_EID_BASE + 68)
#define TIME_DELET_EID                  (KRN_EID_BASE + 69)
#define TIME_RESET_EID                  (KRN_EID_BASE + 70)
#define TIME_CNTRL_EID                  (KRN_EID_BASE + 71)
#define TIME_PAUSE_EID                  (KRN_EID_BASE + 72)
#define TIME_RESUM_EID                  (KRN_EID_BASE + 73)
#define TIME_RUNIN_EID                  (KRN_EID_BASE + 74)
#define TIME_STOPD_EID                  (KRN_EID_BASE + 75)

/* Power Services */
#define PWR_OP_INFO_EID                 (KRN_EID_BASE + 76)
#define PWR_OP_TRANS_EID                (KRN_EID_BASE + 77)
#define PWR_HIB_ENTER_EID               (KRN_EID_BASE + 78)
#define PWR_HIB_EXIT_EID                (KRN_EID_BASE + 79)
#define PWR_HIB_EXIT_OP_EID             (KRN_EID_BASE + 80)
#define PWR_SYS_CNT_EID                 (KRN_EID_BASE + 81)
#define PWR_SYS_MAP_EID                 (KRN_EID_BASE + 82)
#define PWR_SYS_UNMAP_EID               (KRN_EID_BASE + 83)
#define PWR_SYS_TRANS_EID               (KRN_EID_BASE + 84)
#define PWR_DEV_TRANS_EID               (KRN_EID_BASE + 85)
#define PWR_WD_CREATE_EID               (KRN_EID_BASE + 86)
#define PWR_WD_ACTIVE_EID               (KRN_EID_BASE + 87)
#define PWR_WD_EXPIRE_EID               (KRN_EID_BASE + 88)
#define PWR_WD_DELETE_EID               (KRN_EID_BASE + 89)
#define PWR_TICK_SUP_EID                (KRN_EID_BASE + 90)
#define PWR_DEV_NAME_EID                (KRN_EID_BASE + 91)
#define PWR_OP_TL_START_EID             (KRN_EID_BASE + 92)
#define PWR_OP_TL_STOP_EID              (KRN_EID_BASE + 93)
#define PWR_HIB_TL_START_EID            (KRN_EID_BASE + 94)
#define PWR_HIB_TL_STOP_EID             (KRN_EID_BASE + 95)
#define PWR_SYS_TL_START_EID            (KRN_EID_BASE + 96)
#define PWR_SYS_TL_STOP_EID             (KRN_EID_BASE + 97)
#define PWR_DEV_TL_START_EID            (KRN_EID_BASE + 98)
#define PWR_DEV_TL_STOP_EID             (KRN_EID_BASE + 99)

/* Nucleus Storage */
#define STOR_PHY_DEV_LIST_EID           (KRN_EID_BASE + 100)
#define STOR_PHY_DEV_REMOVAL_EID        (KRN_EID_BASE + 101)
#define STOR_LOG_DEV_REMOVAL_EID        (KRN_EID_BASE + 102)
#define STOR_LOG_DEV_INFO_EID           (KRN_EID_BASE + 103)
#define STOR_DIR_CREATE_INFO_EID        (KRN_EID_BASE + 104)
#define STOR_DIR_CREATE_STAT_EID        (KRN_EID_BASE + 105)
#define STOR_DIR_DEL_STAT_EID           (KRN_EID_BASE + 106)
#define STOR_FILE_OPEN_INFO_EID         (KRN_EID_BASE + 107)
#define STOR_FILE_OPEN_STAT_EID         (KRN_EID_BASE + 108)
#define STOR_FILE_CLOSE_STAT_EID        (KRN_EID_BASE + 109)
#define STOR_FILE_READ_START_EID        (KRN_EID_BASE + 110)
#define STOR_FILE_READ_STOP_EID         (KRN_EID_BASE + 111)
#define STOR_FILE_WRITE_START_EID       (KRN_EID_BASE + 112)
#define STOR_FILE_WRITE_STOP_EID        (KRN_EID_BASE + 113)
#define STOR_FILE_SEEK_START_EID        (KRN_EID_BASE + 114)
#define STOR_FILE_SEEK_STOP_EID         (KRN_EID_BASE + 115)
#define STOR_FILE_FLUSH_START_EID       (KRN_EID_BASE + 116)
#define STOR_FILE_FLUSH_STOP_EID        (KRN_EID_BASE + 117)
#define STOR_FILE_TRUNC_START_EID       (KRN_EID_BASE + 118)
#define STOR_FILE_TRUNC_STOP_EID        (KRN_EID_BASE + 119)
#define STOR_FILE_RENAME_STAT_EID       (KRN_EID_BASE + 120)
#define STOR_FILE_DEL_STAT_EID          (KRN_EID_BASE + 121)

/* Nucleus Networking */
#define NET_STACK_INIT_STATUS_EID       (KRN_EID_BASE + 122)
#define NET_DEV_LIST_EID                (KRN_EID_BASE + 123)
#define NET_DEV_REMOVAL_EID             (KRN_EID_BASE + 124)
#define NET_DEV_IPv4_IP_EID             (KRN_EID_BASE + 125)
#define NET_DEV_IPv4_DHCP_IP_EID	    (KRN_EID_BASE + 126)
#define NET_DEV_IPv6_IP_EID             (KRN_EID_BASE + 127)
#define NET_DEV_REMOVE_IP_EID           (KRN_EID_BASE + 128)
#define NET_DEV_DETACH_IP_EID		    (KRN_EID_BASE + 129)
#define NET_DEV_LINK_UP_EID		        (KRN_EID_BASE + 130)
#define NET_DEV_LINK_DOWN_EID		    (KRN_EID_BASE + 131)
#define NET_DEV_UP_STATUS_EID		    (KRN_EID_BASE + 132)
#define NET_SOCK_LIST_EID		        (KRN_EID_BASE + 133)
#define NET_SOCK_STATUS_EID		        (KRN_EID_BASE + 134)
#define NET_SOCK_ENQ_EID                (KRN_EID_BASE + 135)
#define NET_SOCK_TX_START_EID           (KRN_EID_BASE + 136)
#define NET_SOCK_TX_STOP_EID            (KRN_EID_BASE + 137)
#define NET_SOCK_RX_EID                 (KRN_EID_BASE + 138)
#define NET_ADD_ROUTE_EID               (KRN_EID_BASE + 139)
#define NET_ADD_ROUTE6_EID              (KRN_EID_BASE + 140)
#define NET_DEL_ROUTE_EID               (KRN_EID_BASE + 141)
#define NET_DEL_ROUTE6_EID              (KRN_EID_BASE + 142)
#define NET_BUFF_USAGE_EID              (KRN_EID_BASE + 143)
#define NET_DEV_TX_LAT_START_EID        (KRN_EID_BASE + 144)
#define NET_DEV_TX_LAT_STOP_EID         (KRN_EID_BASE + 145)
#define NET_DEV_RX_ACT_EID              (KRN_EID_BASE + 146)
#define NET_DEV_TRANSQ_LEN_EID          (KRN_EID_BASE + 147)
#define NET_DEV_REMOVE_IP6_EID          (KRN_EID_BASE + 148)

/* Trace status definitions */
#define OBJ_ACTION_SUCCESS              0
#define OBJ_BLKD_CTXT                   1
#define OBJ_UNBLKD_CTXT                 2

#if (CFG_NU_OS_SVCS_TRACE_CORE_TRACK_TRACE_OVERHEAD == NU_TRUE)
void    Log_pCU64U64(unsigned short evt_id, char* evt_str, unsigned long long u64_1, unsigned long long u64_2);
void    Log_U64U64I(unsigned short evt_id, unsigned long long u64_1, unsigned long long u64_2, int status);

#define LATENCY_IED                     0xFF
#define LATENCY_START_IED               0x2FF
#define LATENCY_FINISH_IED              0x3FF
#define TIMER_LOAD_VAL                  0x1FF

/* Define max timer count. */
#define     MAX_TIMER_COUNT                                                 \
            ESAL_GE_TMR_COUNT_CALC(ESAL_GE_TMR_OS_CLOCK_RATE,               \
                                   ESAL_GE_TMR_OS_CLOCK_PRESCALE,           \
                                   NU_PLUS_TICKS_PER_SEC)                   \

/* This macro invoke trace log if global mask is enabled for the kernel object requested */
#define APPLY_MASK(_msk_bit_, _eid_, _trace_fn_)                            \
{                                                                           \
    unsigned long long  _start;                                             \
    unsigned long long  _finish;                                            \
                                                                            \
    if(_msk_bit_ & Gbl_Trace_Mask)                                          \
    {                                                                       \
        _start = NU_Get_Time_Stamp();                                       \
        _trace_fn_;                                                         \
        _finish = NU_Get_Time_Stamp();                                      \
        Log_U64U64I(LATENCY_IED, _finish, _start, _eid_);                   \
    }                                                                       \
}

#else

/* This macro invoke trace log if global mask is enabled for the kernel object requested */
#define APPLY_MASK(_msk_bit_, _eid_, _trace_fn_)                            \
{                                                                           \
    NU_UNUSED_PARAM(_eid_);                                                 \
                                                                            \
    if(_msk_bit_ & Gbl_Trace_Mask)                                          \
    {                                                                       \
        _trace_fn_;                                                         \
    }                                                                       \
}

#endif

/* Size definitions for various trace signatures */
#define I_SIZE                          sizeof(int)
#define UL_SIZE                         sizeof(unsigned long)
#define UC_SIZE                         sizeof(unsigned char)
#define pV_SIZE                         sizeof(void*)
#define US_SIZE                         sizeof(unsigned short)
#define IS_SIZE                         sizeof(signed short)
#define IL_SIZE                         sizeof(signed long)

#define TH_SIZE                         sizeof(TRACE_MARK_HDR)
#define TH_I_SIZE                       (TH_SIZE + sizeof(int))
#define TH_pVI_SIZE                     (TH_SIZE + sizeof(void*) + sizeof(int))
#define TH_pVU8I_SIZE                   (TH_SIZE + sizeof(void*) + sizeof(unsigned char) + sizeof(int))
#define TH_pVU32I_SIZE                  (TH_SIZE + sizeof(void*) + sizeof(unsigned long) + sizeof(int))
#define TH_pVU32U8I_SIZE                (TH_SIZE + sizeof(void*) + sizeof(unsigned long) + \
                                         sizeof(unsigned char) + sizeof(int))
#define TH_pVpVU32I_SIZE                (TH_SIZE + sizeof(void*) + sizeof(void*) +          \
                                         sizeof(unsigned long) + sizeof(int))
#define TH_pVpVU32U8I_SIZE              (TH_SIZE + sizeof(void*) + sizeof(void*) +          \
                                         sizeof(unsigned long) + sizeof(unsigned char) + sizeof(int))
#define TH_pVpVpVU32U8I_SIZE            (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned char) + sizeof(int))
#define TH_pVpVU32U32U8I_SIZE           (TH_SIZE + sizeof(void*) + sizeof(void*) +          \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned char) + sizeof(int))
#define TH_pVU32U32U32U8I_SIZE          (TH_SIZE + sizeof(void*) + sizeof(unsigned long) +  \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned char) + sizeof(int))
#define TH_pVpVU32U32U8U8I_SIZE         (TH_SIZE + sizeof(void*) + sizeof(void*) +  \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned char) + sizeof(unsigned char) + sizeof(int))
#define TH_pVpVU32U32U32U8I_SIZE        (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(unsigned long) +\
                                         sizeof(unsigned long) + sizeof(unsigned long) + sizeof(unsigned char) + \
                                         sizeof(int))
#define TH_pVpVpVU32U32U8U8I_SIZE       (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned long) +     \
                                         sizeof(unsigned char) + sizeof(unsigned char) + sizeof(int))
#define TH_pVpVU32U32U32I_SIZE          (TH_SIZE + sizeof(void*) + sizeof(void*) +          \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned long) + sizeof(int))
#define TH_pVpVpVU32U32U8U8U8I_SIZE     (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned char) + sizeof(unsigned char) +    \
                                         sizeof(unsigned char) + sizeof(int))
#define TH_pVpVpVU32U32U32I_SIZE        (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned long) +           \
                                         sizeof(unsigned long) + sizeof(int))
#define TH_pVpVpVU32U32U32U32I_SIZE     (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(int))
#define TH_pVpVpVU32U32U32U32U32I_SIZE  (TH_SIZE + sizeof(void*) + sizeof(void*) + sizeof(void*) + \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned long) + sizeof(unsigned long) +    \
                                         sizeof(unsigned long) + sizeof(int))
#define TH_I32U32U8I_SIZE               (TH_SIZE + sizeof(signed long) + sizeof(unsigned long) + \
                                         sizeof(unsigned char) + sizeof(int))
#define TH_U32U8I32_SIZE                (TH_SIZE + sizeof(unsigned long) + sizeof(unsigned char) + \
                                         sizeof(signed long))
#define TH_U8U32U32I_SIZE               (TH_SIZE + sizeof(unsigned char) + sizeof(unsigned long) + \
                                         sizeof(unsigned long) + sizeof(int))
#define TH_U16U16_SIZE                  (TH_SIZE + sizeof(unsigned short) + sizeof(unsigned short))
#define TH_U16U16I_SIZE                 (TH_SIZE + sizeof(unsigned short) + sizeof(unsigned short) + sizeof(int))
#define TH_U8U32I_SIZE                  (TH_SIZE + sizeof(unsigned char) + sizeof(unsigned long) + sizeof(int))
#define TH_U8I_SIZE                     (TH_SIZE + sizeof(unsigned char) + sizeof(int))
#define TH_I16I_SIZE                    (TH_SIZE + sizeof(signed short) + sizeof(int))
#define TH_U32U32I_SIZE                 (TH_SIZE + sizeof(unsigned long) + sizeof(unsigned long) + sizeof(int))
#define TH_U32I_SIZE                    (TH_SIZE + sizeof(unsigned long) + sizeof(int))


/* Internal function prototypes for kernel markers */
void    Log_V(unsigned short evt_id);
void    Log_I(unsigned short evt_id, int irq_num);
void    Log_II(unsigned short evt_id, int irq_num, int status);
void    Log_U32(unsigned short evt_id, unsigned long u32);
void    Log_U32I(unsigned short evt_id, unsigned long u32, int status);
void    Log_pV(unsigned short evt_id, void* pV);
void    Log_pVU32(unsigned short evt_id, void* pV, unsigned long u32);
void    Log_pVI(unsigned short evt_id, void* pV, int status);
void    Log_pVpV(unsigned short evt_id, void* pV1, void* pV2);
void    Log_pVpVI(unsigned short evt_id, void* pV1, void* pV2, int status);
void    Log_pVpCI(unsigned short evt_id, void* pV, char* pC, int status);
void    Log_pVU8I(unsigned short evt_id, void* pV, unsigned char u8, int status);
void    Log_pVU32I(unsigned short evt_id, void* pV, unsigned long u32, int status);
void    Log_pVU32U8I(unsigned short evt_id, void* pV, unsigned long u32, unsigned char u8, int status);
void    Log_pVpCU32I(unsigned short evt_id, void* pV, char* pC, unsigned long u32, int status);
void    Log_pVU8U8(unsigned short evt_id, void* pV, unsigned char u8_1, unsigned char u8_2);
void    Log_pVU32U32(unsigned short evt_id, void* pV, unsigned long u32_1, unsigned long u32_2);
void    Log_pVpCU32U8I(unsigned short evt_id, void* pV, char* pC, unsigned long u32, unsigned char u8, int status);
void    Log_pVpVU8I(unsigned short evt_id, void* pV1, void* pV2, unsigned char u8, int status);
void    Log_pVpCU8I(unsigned short evt_id, void* pV, char* pC, unsigned char u8, int status);
void    Log_pVpVU32I(unsigned short evt_id, void* pV1, void* pV2, unsigned long u32, int status);
void    Log_pVpVpCU32I(unsigned short evt_id, void* pV1, void* pV2, char* pC, unsigned long u32, int status);
void    Log_pVpVU32U8I(unsigned short evt_id, void* pV1, void* pV2, unsigned long u32, unsigned char u8, int status);
void    Log_pVpVU32U32I(unsigned short evt_id, void* pV1, void* pV2, unsigned long u32_1, unsigned long u32_2, int status);
void    Log_pVpVpCU32U8I(unsigned short evt_id, void* pV1, void* pV2, char* pC, unsigned long u32, unsigned char u8, int status);
void    Log_pVpVpVU32I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, int status);
void    Log_pVpVpVU32U32I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, unsigned long u32_2, int status);
void    Log_pVpVpVpCU32U8I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, char* pC, unsigned long u32, unsigned char u8, int status);
void    Log_pVpVpCU32U32U8I(unsigned short evt_id, void* pV1, void* pV3, char* pC, unsigned long u32_1,
                            unsigned long u32_2, unsigned char u8, int status);
void    Log_pVpVU32U32U32I(unsigned short evt_id, void* pV1, void* pV2, unsigned long u32_1, unsigned long u32_2, unsigned long u32_3, int status);
void    Log_pVpVU32U32U8I(unsigned short evt_id, void* pV1, void* pV2, unsigned long u32_1, unsigned long u32_2, unsigned char u8, int status);
void    Log_pVU32U32U32U8I(unsigned short evt_id, void* pV, unsigned long u32_1, unsigned long u32_2, unsigned long u32_3,
                           unsigned char u8, int status);
void    Log_pVpVpCU32U32U32U8I(unsigned short evt_id, void* pV1, void* pV2, char* pC,unsigned long u32_1, unsigned long u32_2,
                               unsigned long u32_3, unsigned char u8, int status);
void    Log_pVpVpVU32U32U8U8I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, unsigned long u32_2,
                              unsigned char u8_1, unsigned char u8_2, int status);
void    Log_pVpVpCU32U32U8U8I(unsigned short evt_id, void* pV1, void* pV2, char* pC, unsigned long u32_1, unsigned long u32_2,
                              unsigned char u8_1, unsigned char u8_2, int status);
void    Log_pVpVpVpCU32U32U8U8U8I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, char* pC1, unsigned long u32_1,
                                  unsigned long u32_2, unsigned char u8_1, unsigned char u8_2, unsigned char u8_3, int status);
void    Log_pVpVpVU32U32U32I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, unsigned long u32_2,
                             unsigned long u32_3, int status);
void    Log_pVpVpVU32U32U32U32I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, unsigned long u32_2,
                                unsigned long u32_3, unsigned long u32_4, int status);
void    Log_pVpVpVU32U32U32U32U32I(unsigned short evt_id, void* pV1, void* pV2, void* pV3, unsigned long u32_1, unsigned long u32_2,
                                   unsigned long u32_3, unsigned long u32_4, unsigned long u32_5, int status);
void    Log_U8U8U16U8U32(unsigned short evt_id, unsigned char u8_1, unsigned char u8_2, unsigned short u16, unsigned char u8_3, unsigned long u32);
void    Log_U8I(unsigned short evt_id, unsigned char u8, int status);
void    Log_U8(unsigned short evt_id, unsigned char u8);
void    Log_I32U32U8I(unsigned short evt_id, signed long i32, unsigned long u32, unsigned char u8,int status);
void    Log_I32U32I(unsigned short evt_id, signed long i32, unsigned long u32, int status);
void    Log_I32I(unsigned short evt_id, signed long i32, int status);
void    Log_I32II(unsigned short evt_id, signed long i32, int fd, int status);
void    Log_I32I16I(unsigned short evt_id, signed long i32, signed short i16, int fd);
void    Log_I32II32(unsigned short evt_id, signed long i32_1, int fd, signed long i32_2);
void    Log_pCU32U8I32(unsigned short evt_id, char* pC, unsigned long u32, unsigned char u8, signed long i32);
void    Log_I32(unsigned short evt_id, signed long i32);
void    Log_pC(unsigned short evt_id, char* pC);
void    Log_pCI(unsigned short evt_id, char* pC, int status);
void    Log_pCpCpCU8U32U32I(unsigned short evt_id, char* pC1, char* pC2, char* pC3,
                            unsigned char u8, unsigned long u32_1, unsigned long u32_2, int status);
void    Log_pCpCI(unsigned short evt_id, char* pC1, char* pC2, int fd);
void    Log_pCpCpC(unsigned short evt_id, char* pC1, char* pC2, char* pC3);
void    Log_pCpCpCU16U16I(unsigned short evt_id, char* pC1, char* pC2, char* pC3, unsigned short u16_1, unsigned short u16_2, int fd);
void    Log_pCU16U16I(unsigned short evt_id, char* pC, unsigned short u16_1, unsigned short u16_2, int fd);
void    Log_I32I16II32(unsigned short evt_id, signed long i32_1, signed short i16, int fd, signed long i32_2);
void    Log_pCpU8U8U32I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8, unsigned char u8,
                                     unsigned long u32, int status, unsigned char pu8_array_size);
void    Log_pCpU8pU8I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8_1,
                                   unsigned char* pU8_2, int status, unsigned char pu8_array_size);
void    Log_pCpU8pU8pU8I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8_1,
                                      unsigned char* pU8_2, unsigned char* pU8_3, int status, unsigned char pu8_array_size);
void    Log_pCpU8I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8, int status, unsigned char pu8_array_size);
void    Log_pCpU8U8I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8, unsigned char u8,
                                  int status, unsigned char u8_array_size);
void    Log_pCpU8pU8U8I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8_1, unsigned char* pU8_2,
                                     unsigned char u8, int status, unsigned char u8_array_size);
void    Log_pCpU8I16I_pU8ArraySize(unsigned short evt_id, char* pC, unsigned char* pU8, signed short i16, int status, unsigned char pu8_array_size);
void    Log_pCU8I(unsigned short evt_id, char* pC, unsigned char u8, int status);
void    Log_I16I16I16I(unsigned short evt_id, signed short i16_1, signed short i16_2, signed short i16_3, int status);
void    Log_U16II(unsigned short evt_id, unsigned short u16, int sd, int status);
void    Log_U32U32I(unsigned short evt_id, unsigned long u32_1, unsigned long u32_2, int sd);
void    Log_U16II32(unsigned short evt_id, unsigned short u16, int sd, signed long i32);
void    Log_pU8pU8I_pU8ArraySize(unsigned short evt_id, unsigned char* pU8_1, unsigned char* pU8_2, int status, unsigned char pu8_array_size);
void    Log_U16(unsigned short evt_id, unsigned short u16);
void    Log_pCU32U32I(unsigned short evt_id, char* pC, unsigned long u32_1, unsigned long u32_2, int status);
void    Log_pCU32I(unsigned short evt_id, char* pC, unsigned long u32, int status);
void    Log_pCU32(unsigned short evt_id, char* pC, unsigned long u32);


/* Build configuration that enables or disables kernel markers */
#if (CFG_NU_OS_SVCS_TRACE_CORE_PC_HOTSPOT_SUPPORT == NU_TRUE)

extern void                                 *Trace_PC_Sample;

/********************************/
/* Nucleus PC Hot Spot Analysis */
/********************************/
#define T_PC_SAMPLE()                       Log_pV(0xFFFF, Trace_PC_Sample)

#else

/********************************/
/* Nucleus PC Hot Spot Analysis */
/********************************/
#define T_PC_SAMPLE()

#endif /* (CFG_NU_OS_SVCS_TRACE_CORE_PC_HOTSPOT_SUPPORT == NU_TRUE) */

/*********/
/* LISRs */
/*********/
#define    T_IRQ_ENABLED(irq_num, status)   APPLY_MASK(NU_TRACE_LISRS, IRQ_ENABLE_EID,        \
                                            Log_II(IRQ_ENABLE_EID, irq_num, status))
#define    T_IRQ_DISABLED(irq_num, status)  APPLY_MASK(NU_TRACE_LISRS, IRQ_DSABLE_EID,        \
                                            Log_II(IRQ_DSABLE_EID, irq_num, status))
#define    T_LISR_REGISTERED(irq_num, status) APPLY_MASK(NU_TRACE_LISRS, LISR_REGED_EID,      \
                                            Log_II(LISR_REGED_EID, irq_num, status))
#define    T_LISR_ENTRY(irq_num)            APPLY_MASK(NU_TRACE_LISRS, LISR_ENTRY_EID,        \
                                            Log_I(LISR_ENTRY_EID, irq_num))
#define    T_LISR_EXIT(irq_num)             APPLY_MASK(NU_TRACE_LISRS, LISR_EXITT_EID,        \
                                            Log_I(LISR_EXITT_EID, irq_num))
/*********/
/* HISRs */
/*********/
#define    T_HISR_CREATED(p_hcb, p_hisr_entry, p_stack, p_hisr_name, p_size, priority, status)\
                                            APPLY_MASK(NU_TRACE_HISRS, HISR_CREAT_EID,        \
                                            Log_pVpVpVpCU32U8I(HISR_CREAT_EID, p_hcb,         \
                                            p_hisr_entry, p_stack, p_hisr_name, p_size,       \
                                            priority, status))
#define    T_HISR_DELETED(p_hcb, status)    APPLY_MASK(NU_TRACE_HISRS, HISR_DELET_EID,        \
                                            Log_pVI(HISR_DELET_EID, p_hcb, status))
#define    T_HISR_ACTIVATED(p_hcb, status)  APPLY_MASK(NU_TRACE_HISRS, HISR_ACTIV_EID,        \
                                            Log_pVI(HISR_ACTIV_EID, p_hcb, status))
#define    T_HISR_RUNNING(p_hcb)            APPLY_MASK(NU_TRACE_HISRS, HISR_RUNIG_EID,        \
                                            Log_pV(HISR_RUNIG_EID, p_hcb))
#define    T_HISR_STOPPED(p_hcb)            APPLY_MASK(NU_TRACE_HISRS, HISR_STOPD_EID,        \
                                            Log_pV(HISR_STOPD_EID, p_hcb))
/*********/
/* Tasks */
/*********/
#define    T_TASK_CREATED(p_tcb, p_task, p_stk, p_name, stk_size, time_slice, priority,        \
                                            preempt_opt, strt_opt, status)                     \
                                            APPLY_MASK(NU_TRACE_TASKS, TASK_CREAT_EID,         \
                                            Log_pVpVpVpCU32U32U8U8U8I(TASK_CREAT_EID,          \
                                            p_tcb, p_task, p_stk, p_name, stk_size,            \
                                            time_slice, priority, preempt_opt, strt_opt, status))
#define    T_TASK_RESET(p_tcb, status)      APPLY_MASK(NU_TRACE_TASKS, TASK_RESET_EID,         \
                                            Log_pVI(TASK_RESET_EID, p_tcb, status))
#define    T_TASK_READY(p_tcb)              APPLY_MASK(NU_TRACE_TASKS, TASK_READY_EID,         \
                                            Log_pV(TASK_READY_EID, p_tcb))
#define    T_TASK_RUNNING(p_tcb)            APPLY_MASK(NU_TRACE_TASKS, TASK_RUNIG_EID,         \
                                            Log_pV(TASK_RUNIG_EID, p_tcb))
#define    T_TASK_SUSPEND(p_tcb, sus_type)  APPLY_MASK(NU_TRACE_TASKS, TASK_SSUSP_EID,         \
                                            Log_pVU32(TASK_SSUSP_EID, p_tcb, sus_type))
#define    T_TASK_TERMINATED(p_tcb, status) APPLY_MASK(NU_TRACE_TASKS, TASK_TERMT_EID,         \
                                            Log_pVI(TASK_TERMT_EID, p_tcb, status))
#define    T_TASK_DELETED(p_tcb, status)    APPLY_MASK(NU_TRACE_TASKS, TASK_DELET_EID,         \
                                            Log_pVI(TASK_DELET_EID, p_tcb, status))
#define    T_TASK_CHG_PREMPTION(p_tcb, new_prempt_state, old_prempt_state)                     \
                                            APPLY_MASK(NU_TRACE_TASKS, TASK_CHPRE_EID,         \
                                            Log_pVU8U8(TASK_CHPRE_EID, p_tcb,                  \
                                            new_prempt_state, old_prempt_state))
#define    T_TASK_CHG_PRIORITY(p_tcb, new_priority, old_priority)                              \
                                            APPLY_MASK(NU_TRACE_TASKS, TASK_CHPRO_EID,         \
                                            Log_pVU8U8(TASK_CHPRO_EID, p_tcb,                  \
                                            new_priority, old_priority))
#define    T_TASK_CHG_TIMESLICE(p_tcb, new_timeslice, old_timeslice)                           \
                                            APPLY_MASK(NU_TRACE_TASKS, TASK_CHSLC_EID,         \
                                            Log_pVU32U32(TASK_CHSLC_EID, p_tcb,                \
                                            new_timeslice, old_timeslice))

/******************/
/* Dynamic memory */
/******************/
#define    T_MEM_POOL_CREATE(p_mpcb, p_start_add, p_name, size, available_size, min_size,       \
                                            sus_typ, status)                                    \
                                            APPLY_MASK(NU_TRACE_DMEM, MEM_CREATE_EID,           \
                                            Log_pVpVpCU32U32U32U8I(MEM_CREATE_EID,              \
                                            p_mpcb, p_start_add, p_name, size, available_size,  \
                                            min_size, sus_typ, status))
#define    T_MEM_POOL_DELETE(p_mpcb, status) APPLY_MASK(NU_TRACE_DMEM, MEM_DELETE_EID,          \
                                            Log_pVI(MEM_DELETE_EID, p_mpcb, status))
#define    T_MEM_ADD(p_mpcb, p_start_add, p_caller, total_size, available_size, added_size,     \
                                            status)                                             \
                                            APPLY_MASK(NU_TRACE_DMEM, MEM_ADD_EID,              \
                                            Log_pVpVpVU32U32U32I(MEM_ADD_EID, p_mpcb,           \
                                            p_start_add, p_caller, total_size, available_size,  \
                                            added_size, status))
#define    T_DMEM_ALLOCATE(p_mpcb, p_start, p_caller, total_size, available_size,               \
                                            size, sus_type, status)                             \
                                            APPLY_MASK(NU_TRACE_DMEM, DMEM_ALLOC_EID,           \
                                            Log_pVpVpVU32U32U32U32I(DMEM_ALLOC_EID,             \
                                            p_mpcb, p_start, p_caller, total_size,              \
                                            available_size, size, sus_type, status))
#define    T_DMEM_DEALLOCATE(p_mpcb, p_start, p_caller, total_size, available_size, status)     \
                                            APPLY_MASK(NU_TRACE_DMEM, DMEM_DALLO_EID,           \
                                            Log_pVpVpVU32U32I(DMEM_DALLO_EID, p_mpcb,           \
                                            p_start, p_caller, total_size, available_size, status))
#define    T_DMEM_REALLOCATE(p_mpcb, p_orig_mem, p_caller, total_size, available_size, new_size,\
                                            susp_type, status)                                  \
                                            APPLY_MASK(NU_TRACE_DMEM, DMEM_RALLO_EID,           \
                                            Log_pVpVpVU32U32U32U32I(DMEM_RALLO_EID,             \
                                            p_mpcb, p_orig_mem, p_caller, total_size,           \
                                            available_size, new_size, susp_type, status))

/********************/
/* Partition memory */
/********************/
#define    T_PMEM_CREATE(p_pmpcb, p_start_add, p_name, p_avail, p_size, sus_type, status)       \
                                            APPLY_MASK(NU_TRACE_PMEM, PMEM_CREAT_EID,           \
                                            Log_pVpVpCU32U32U8I(PMEM_CREAT_EID,                 \
                                            p_pmpcb, p_start_add, p_name, p_avail, p_size,      \
                                            sus_type, status))
#define    T_PMEM_ALLOCATE(p_pmpcb, p_partition, p_caller, poolsize, partition_size,            \
                                            available_size, allocated_size, susp_type, status)  \
                                            APPLY_MASK(NU_TRACE_PMEM, PMEM_ALLOC_EID,           \
                                            Log_pVpVpVU32U32U32U32U32I(PMEM_ALLOC_EID,          \
                                            p_pmpcb, p_partition, p_caller, poolsize,           \
                                            partition_size, available_size, allocated_size,     \
                                            susp_type, status))
#define    T_PMEM_DEALLOCATE(p_pmpcb, p_partition, p_caller, poolsize, partition_size,          \
                                            available_size, allocated_size, status)             \
                                            APPLY_MASK(NU_TRACE_PMEM, PMEM_DEALL_EID,           \
                                            Log_pVpVpVU32U32U32U32I(PMEM_DEALL_EID,             \
                                            p_pmpcb, p_partition, p_caller, poolsize,           \
                                            partition_size, available_size, allocated_size, status))
#define    T_PMEM_DELETE(p_pmpcb, status)   APPLY_MASK(NU_TRACE_PMEM, PMEM_DELET_EID,           \
                                            Log_pVI(PMEM_DELET_EID, p_pmpcb, status))

/**************/
/* Semaphores */
/**************/
#define    T_SEM_CREATE(p_scb, p_name, ini_count, susp_type, status)                           \
                                            APPLY_MASK(NU_TRACE_SEMAPHORE, SEMA_CREAT_EID,     \
                                            Log_pVpCU32U8I(SEMA_CREAT_EID,                     \
                                            p_scb, p_name, ini_count, susp_type, status))
#define    T_SEM_DELETE(p_pcb, p_tcb, status)                                                  \
                                            APPLY_MASK(NU_TRACE_SEMAPHORE, SEMA_DELET_EID,     \
                                            Log_pVpVI(SEMA_DELET_EID, p_pcb, p_tcb, status))
#define    T_SEM_RESET(p_pcb, p_tcb, init_count, status)                                       \
                                            APPLY_MASK(NU_TRACE_SEMAPHORE, SEMA_RESET_EID,     \
                                            Log_pVpVU32I(SEMA_RESET_EID, p_pcb,                \
                                            p_tcb, init_count, status))
#define    T_SEM_OBTAIN(p_pcb, p_tcb, suspend, status)                                         \
                                            APPLY_MASK(NU_TRACE_SEMAPHORE, SEMA_OBTAI_EID,     \
                                            Log_pVpVU32I(SEMA_OBTAI_EID, p_pcb,                \
                                            p_tcb, suspend, status))
#define    T_SEM_RELEASE(p_pcb, p_tcb, status)APPLY_MASK(NU_TRACE_SEMAPHORE, SEMA_RELSE_EID,   \
                                            Log_pVpVI(SEMA_RELSE_EID, p_pcb, p_tcb, status))

/****************/
/* Event groups */
/****************/
#define    T_EVT_GRP_CREATE(p_evt_grp, p_name, status)                                         \
                                            APPLY_MASK(NU_TRACE_EVENT, EVEN_CREAT_EID,         \
                                            Log_pVpCI(EVEN_CREAT_EID, p_evt_grp,               \
                                            p_name, status))
#define    T_EVT_SET(p_evt_grp, evt_flags, operation, status)                                  \
                                            APPLY_MASK(NU_TRACE_EVENT, EVEN_SET_EID,           \
                                            Log_pVU32U8I(EVEN_SET_EID, p_evt_grp,              \
                                            evt_flags, operation, status))
#define    T_EVT_RETRIEVE(p_evt_grp, req_evt, ret_events, suspend, operation, status)          \
                                            APPLY_MASK(NU_TRACE_EVENT, EVEN_RETRV_EID,         \
                                            Log_pVU32U32U32U8I(EVEN_RETRV_EID,                 \
                                            p_evt_grp, req_evt, ret_events, suspend,           \
                                            operation, status))
#define    T_EVT_GRP_DELETE(p_evt_grp, status) APPLY_MASK(NU_TRACE_EVENT, EVEN_DELET_EID,      \
                                            Log_pVI(EVEN_DELET_EID, p_evt_grp, status))

/**********/
/* Queues */
/**********/
#define    T_Q_CREATE(p_qcb, p_start, p_name, q_size, msg_size, msg_type, susp_type, status)   \
                                            APPLY_MASK(NU_TRACE_QUEUE, QUEU_CREAT_EID,         \
                                            Log_pVpVpCU32U32U8U8I(QUEU_CREAT_EID,              \
                                            p_qcb, p_start, p_name, q_size, msg_size,          \
                                            msg_type, susp_type, status))
#define    T_Q_RESET(p_qcb, status)         APPLY_MASK(NU_TRACE_QUEUE, QUEU_RESET_EID,         \
                                            Log_pVI(QUEU_RESET_EID, p_qcb, status))
#define    T_Q_BCAST(p_qcb, p_msg, size, susp, status)                                         \
                                            APPLY_MASK(NU_TRACE_QUEUE, QUEU_BRBND_EID,         \
                                            Log_pVpVU32U32I(QUEU_BRBND_EID, p_qcb,             \
                                            p_msg, size, susp, status))
#define    T_Q_SEND(p_qcb, p_msg, size, susp, status)                                          \
                                            APPLY_MASK(NU_TRACE_QUEUE, QUEU_SEND_EID,          \
                                            Log_pVpVU32U32I(QUEU_SEND_EID, p_qcb,              \
                                            p_msg, size, susp, status))
#define    T_Q_SEND2FRONT(p_qcb, p_msg, size, susp, status)                                    \
                                            APPLY_MASK(NU_TRACE_QUEUE, QUEU_SD2FT_EID,         \
                                            Log_pVpVU32U32I(QUEU_SD2FT_EID, p_qcb,             \
                                            p_msg, size, susp, status))
#define    T_Q_RECV(p_qcb, p_msg, q_size, a_size, susp, status)                                \
                                            APPLY_MASK(NU_TRACE_QUEUE, QUEU_RECVE_EID,         \
                                            Log_pVpVU32U32U32I(QUEU_RECVE_EID, p_qcb,          \
                                            p_msg, q_size, a_size, susp, status))
#define    T_Q_DELETE(p_qcb, status)        APPLY_MASK(NU_TRACE_QUEUE, QUEU_DELET_EID,         \
                                            Log_pVI(QUEU_DELET_EID, p_qcb, status))

/*********/
/* Pipes */
/*********/
#define    T_PIPE_CREATE(p_pcb, p_start, p_name, pip_size, msg_size, msg_type, susp, status)   \
                                            APPLY_MASK(NU_TRACE_PIPE, PIPE_CREAT_EID,          \
                                            Log_pVpVpCU32U32U8U8I(PIPE_CREAT_EID,              \
                                            p_pcb, p_start, p_name, pip_size, msg_size,        \
                                            msg_type, susp, status))
#define    T_PIPE_RESET(p_pcb, status)      APPLY_MASK(NU_TRACE_PIPE, PIPE_RESET_EID,          \
                                            Log_pVI(PIPE_RESET_EID, p_pcb, status))
#define    T_PIPE_BCAST(p_pcb, p_msg, size, suspend, status)                                   \
                                            APPLY_MASK(NU_TRACE_PIPE, PIPE_BRBND_EID,          \
                                            Log_pVpVU32U32I(PIPE_BRBND_EID,                    \
                                            p_pcb, p_msg, size, suspend, status))
#define    T_PIPE_SEND(p_pcb, p_msg, size, suspend, status)                                    \
                                            APPLY_MASK(NU_TRACE_PIPE, PIPE_SEND_EID,           \
                                            Log_pVpVU32U32I(PIPE_SEND_EID, p_pcb,              \
                                            p_msg, size, suspend, status))
#define    T_PIPE_SEND2FRONT(p_pcb, p_msg, size, suspend, status)                              \
                                            APPLY_MASK(NU_TRACE_PIPE, PIPE_SD2FT_EID,          \
                                            Log_pVpVU32U32I(PIPE_SD2FT_EID, p_pcb,             \
                                            p_msg, size, suspend, status))
#define    T_PIPE_RECV(p_pcb, p_msg, p_size, a_size, suspend, status)                          \
                                            APPLY_MASK(NU_TRACE_PIPE, PIPE_RECVE_EID,          \
                                            Log_pVpVU32U32U32I(PIPE_RECVE_EID, p_pcb,          \
                                            p_msg, p_size, a_size, suspend, status))
#define    T_PIPE_DELETE(p_pcb, status)     APPLY_MASK(NU_TRACE_PIPE, PIPE_DELET_EID,          \
                                            Log_pVI(PIPE_DELET_EID, p_pcb, status))

/***********/
/* Mailbox */
/***********/
#define    T_MBOX_CREATE(p_mcb, p_name, susp_type, status)                                     \
                                            APPLY_MASK(NU_TRACE_MAILBOX, MBOX_CREAT_EID,       \
                                            Log_pVpCU8I(MBOX_CREAT_EID,                        \
                                            p_mcb, p_name, susp_type, status))
#define    T_MBOX_RESET(p_mcb, status)      APPLY_MASK(NU_TRACE_MAILBOX, MBOX_RESET_EID,       \
                                            Log_pVI(MBOX_RESET_EID, p_mcb, status))
#define    T_MBOX_BCAST(p_mcb, p_msg, susp_type, status)                                       \
                                            APPLY_MASK(NU_TRACE_MAILBOX, MBOX_BRBND_EID,       \
                                            Log_pVpVU32I(MBOX_BRBND_EID, p_mcb,                \
                                            p_msg, susp_type, status))
#define    T_MBOX_SEND(p_mcb, p_msg, susp_type, status)                                        \
                                            APPLY_MASK(NU_TRACE_MAILBOX, MBOX_SEND_EID,        \
                                            Log_pVpVU32I(MBOX_SEND_EID, p_mcb,                 \
                                            p_msg, susp_type, status))
#define    T_MBOX_RECV(p_mcb, p_msg, susp_type, status)                                        \
                                            APPLY_MASK(NU_TRACE_MAILBOX, MBOX_RECVE_EID,       \
                                            Log_pVpVU32I(MBOX_RECVE_EID, p_mcb,                \
                                            p_msg, susp_type, status))
#define    T_MBOX_DELETE(p_mcb, status)     APPLY_MASK(NU_TRACE_MAILBOX, MBOX_DELET_EID,       \
                                            Log_pVI(MBOX_DELET_EID, p_mcb, status))

/***********/
/* Signals */
/***********/
#define    T_SIG_REG_HANDLER(p_tcb, p_sig_hdlr, status)                                        \
                                            APPLY_MASK(NU_TRACE_SIGNAL, SGNL_REGSR_EID,        \
                                            Log_pVpVI(SGNL_REGSR_EID, p_tcb, p_sig_hdlr,       \
                                            status))
#define    T_SIG_CONTROL(p_tcb, enbl_signal_mask, old_signal_mask)                             \
                                            APPLY_MASK(NU_TRACE_SIGNAL, SGNL_CNTRL_EID,        \
                                            Log_pVU32U32(SGNL_CNTRL_EID,                       \
                                            p_tcb, enbl_signal_mask, old_signal_mask))
#define    T_SIG_SEND(p_tcb, signals, status)  APPLY_MASK(NU_TRACE_SIGNAL, SGNL_SEND_EID,      \
                                            Log_pVU32I(SGNL_SEND_EID, p_tcb,                   \
                                            signals, status))
#define    T_SIG_RECV(p_tcb, signals)       APPLY_MASK(NU_TRACE_SIGNAL, SGNL_RECVE_EID,        \
                                            Log_pVU32(SGNL_RECVE_EID, p_tcb, signals))
#define    T_SIG_HANDLER_RUNNING(p_tcb, p_sig_hdlr)     APPLY_MASK(NU_TRACE_SIGNAL, SGNL_RUNIN_EID,\
                                            Log_pVpV(SGNL_RUNIN_EID, p_tcb, p_sig_hdlr))
#define    T_SIG_HANDLER_STOPPED(p_tcb, p_sig_hdlr)     APPLY_MASK(NU_TRACE_SIGNAL, SGNL_STOPD_EID,\
                                            Log_pVpV(SGNL_STOPD_EID, p_tcb, p_sig_hdlr))


/**********/
/* Timers */
/**********/
#define    T_TIMER_CREATE(p_tcb, p_exp_routine, p_name, id, init_time,                         \
                                            resched_time, enable, status)                      \
                                            APPLY_MASK(NU_TRACE_TIMER, TIME_CREAT_EID,         \
                                            Log_pVpVpCU32U32U32U8I(TIME_CREAT_EID,             \
                                            p_tcb, p_exp_routine, p_name, id, init_time,       \
                                            resched_time, enable, status))
#define    T_TIMER_DELETE(p_tcb, status)    APPLY_MASK(NU_TRACE_TIMER, TIME_DELET_EID,         \
                                            Log_pVI(TIME_DELET_EID, p_tcb, status))
#define    T_TIMER_RESET(p_tcb, p_exp_routine, init_time, resched_time, enable, status)        \
                                            APPLY_MASK(NU_TRACE_TIMER, TIME_RESET_EID,         \
                                            Log_pVpVU32U32U8I(TIME_RESET_EID,                  \
                                            p_tcb, p_exp_routine, init_time,                   \
                                            resched_time, enable, status))
#define    T_TIMER_CONTROL(p_tcb, enable, status)                                              \
                                            APPLY_MASK(NU_TRACE_TIMER, TIME_CNTRL_EID,         \
                                            Log_pVU8I(TIME_CNTRL_EID, p_tcb, enable, status))
#define    T_TIMER_PAUSE(p_tcb, status)     APPLY_MASK(NU_TRACE_TIMER, TIME_PAUSE_EID,         \
                                            Log_pVI(TIME_PAUSE_EID, p_tcb, status))
#define    T_TIMER_RESUME(p_tcb, status)    APPLY_MASK(NU_TRACE_TIMER, TIME_RESUM_EID,         \
                                            Log_pVI(TIME_RESUM_EID, p_tcb, status))
#define    T_TIMER_EXP_ROUTIN_RUNNING(p_tcb, id)   APPLY_MASK(NU_TRACE_TIMER, TIME_RUNIN_EID,  \
                                            Log_pVU32(TIME_RUNIN_EID, p_tcb, id))
#define    T_TIMER_EXP_ROUTIN_STOPPED(p_tcb, id)   APPLY_MASK(NU_TRACE_TIMER, TIME_STOPD_EID,  \
                                            Log_pVU32(TIME_STOPD_EID, p_tcb, id))


/********/
/* Idle */
/********/
#define    T_IDLE_ENTRY()                   idle_flag = 1; APPLY_MASK(NU_TRACE_CPU_STATE, IDLE_ENTRY_EID, Log_U32(IDLE_ENTRY_EID,0))
#define    T_IDLE_EXIT()                    if(idle_flag == 1) {idle_flag = 0; APPLY_MASK(NU_TRACE_CPU_STATE, IDLE_EXIT_EID, Log_U32(IDLE_EXIT_EID,0))}

/******************/
/* Power Services */
/******************/
#define T_OP_INFO(op_id, volt_id, volt_val, freq_id, freq_val)                                \
                                            APPLY_MASK(NU_TRACE_PMS_INFO, PWR_OP_INFO_EID,    \
                                            Log_U8U8U16U8U32(PWR_OP_INFO_EID, op_id,          \
                                            volt_id, volt_val, freq_id, freq_val))
#define T_OP_TRANS(op_id, status)           APPLY_MASK(NU_TRACE_PMS_INFO, PWR_OP_TRANS_EID,   \
                                            Log_U8I(PWR_OP_TRANS_EID, op_id, status))
#define T_HIB_ENTER(level, status)          APPLY_MASK(NU_TRACE_PMS_INFO, PWR_HIB_ENTER_EID,  \
                                            Log_U8I(PWR_HIB_ENTER_EID, level, status))
#define T_HIB_EXIT(boot_state, status)      APPLY_MASK(NU_TRACE_PMS_INFO, PWR_HIB_EXIT_EID,   \
                                            Log_U32I(PWR_HIB_EXIT_EID, boot_state, status))
#define T_HIB_EXIT_OP(op_id, status)        APPLY_MASK(NU_TRACE_PMS_INFO, PWR_HIB_EXIT_OP_EID,\
                                            Log_U8I(PWR_HIB_EXIT_OP_EID, op_id, status))
#define T_SYS_COUNT(state_count, status)    APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_CNT_EID,    \
                                            Log_U8I(PWR_SYS_CNT_EID, state_count,             \
                                            status))
#define T_SYS_TRANS(state, status)          APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_TRANS_EID,  \
                                            Log_U8I(PWR_SYS_TRANS_EID, state, status))
#define T_SYS_MAP(dev_id, dev_state, sys_state, status)                                       \
                                            APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_MAP_EID,    \
                                            Log_I32U32U8I(PWR_SYS_MAP_EID, dev_id,            \
                                            dev_state, sys_state, status))
#define T_SYS_UNMAP(dev_id, status)         APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_UNMAP_EID,  \
                                            Log_I32I(PWR_SYS_UNMAP_EID, dev_id, status))
#define T_DEV_TRANS(dev_id, state, status)  APPLY_MASK(NU_TRACE_PMS_INFO, PWR_DEV_TRANS_EID,  \
                                            Log_I32U32I(PWR_DEV_TRANS_EID, dev_id,            \
                                            state, status))
#define T_WD_CREATE(wd, status)             APPLY_MASK(NU_TRACE_PMS_INFO, PWR_WD_CREATE_EID,  \
                                            Log_pVI(PWR_WD_CREATE_EID, wd, status))
#define T_WD_ACTIVE(wd)                     APPLY_MASK(NU_TRACE_PMS_INFO, PWR_WD_ACTIVE_EID,  \
                                            Log_pV(PWR_WD_ACTIVE_EID, wd))
#define T_WD_EXPIRE(wd)                     APPLY_MASK(NU_TRACE_PMS_INFO, PWR_WD_EXPIRE_EID,  \
                                            Log_pV(PWR_WD_EXPIRE_EID, wd))
#define T_WD_DELETE(wd, status)             APPLY_MASK(NU_TRACE_PMS_INFO, PWR_WD_DELETE_EID,  \
                                            Log_pVI(PWR_WD_DELETE_EID, wd, status))
#define T_TICK_SUPPRESS(flag)               APPLY_MASK(NU_TRACE_PMS_INFO, PWR_TICK_SUP_EID,   \
                                            Log_U8(PWR_TICK_SUP_EID, flag))
#define T_DEV_NAME(key, state, state_count, dev_id)                                           \
                                            APPLY_MASK(NU_TRACE_PMS_INFO, PWR_DEV_NAME_EID,   \
                                            Log_pCU32U8I32(PWR_DEV_NAME_EID, key, state,      \
                                            state_count, dev_id))

/* Power Services Transition Latency */
#define T_DEV_TRANS_START(dev_id)           APPLY_MASK(NU_TRACE_PMS_INFO,                       \
                                            PWR_DEV_TL_START_EID,                               \
                                            Log_I32(PWR_DEV_TL_START_EID, dev_id))
#define T_DEV_TRANS_STOP(dev_id)            APPLY_MASK(NU_TRACE_PMS_INFO,                       \
                                            PWR_DEV_TL_STOP_EID,                                \
                                            Log_I32(PWR_DEV_TL_STOP_EID, dev_id))
#define T_OP_TL_START()                     APPLY_MASK(NU_TRACE_PMS_INFO, PWR_OP_TL_START_EID,  \
                                            Log_V(PWR_OP_TL_START_EID))
#define T_OP_TL_STOP()                      APPLY_MASK(NU_TRACE_PMS_INFO, PWR_OP_TL_STOP_EID,   \
                                            Log_V(PWR_OP_TL_STOP_EID))
#define T_HIB_TL_START()                    APPLY_MASK(NU_TRACE_PMS_INFO, PWR_HIB_TL_START_EID, \
                                            Log_V(PWR_HIB_TL_START_EID))
#define T_HIB_TL_STOP()                     APPLY_MASK(NU_TRACE_PMS_INFO, PWR_HIB_TL_STOP_EID,  \
                                            Log_V(PWR_HIB_TL_STOP_EID, dev_id))
#define T_SYS_TL_START()                    APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_TL_START_EID, \
                                            Log_V(PWR_SYS_TL_START_EID))
#define T_SYS_TL_STOP()                     APPLY_MASK(NU_TRACE_PMS_INFO, PWR_SYS_TL_STOP_EID,  \
                                            Log_V(PWR_SYS_TL_STOP_EID))

/*******************/
/* Nucleus Storage */
/*******************/
#define T_PHY_DEV_LIST(dev_name)            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_PHY_DEV_LIST_EID,   \
                                            Log_pC(STOR_PHY_DEV_LIST_EID, dev_name))
#define T_PHY_DEV_REMOVAL(dev_name, status) APPLY_MASK(NU_TRACE_STOR_INFO, STOR_PHY_DEV_REMOVAL_EID,   \
                                            Log_pCI(STOR_PHY_DEV_REMOVAL_EID, dev_name, status))
#define T_LOG_DEV_REMOVAL(dev_name, status) APPLY_MASK(NU_TRACE_STOR_INFO, STOR_LOG_DEV_REMOVAL_EID,   \
                                            Log_pCI(STOR_LOG_DEV_REMOVAL_EID, dev_name, status))
#define T_LOG_DEV_INFO(dev_name, fs, mp, auto_fmt, sect_start, sect_end, status)                       \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_LOG_DEV_INFO_EID,      \
                                            Log_pCpCpCU8U32U32I(STOR_LOG_DEV_INFO_EID,                 \
                                            dev_name, fs, mp, auto_fmt, sect_start, sect_end, status))
#define T_DIR_CREATE_INFO(mp, dev_name, dir_name)                                                      \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_DIR_CREATE_INFO_EID,   \
                                            Log_pCpCpC(STOR_DIR_CREATE_INFO_EID, mp, dev_name,         \
                                            dir_name))
#define T_DIR_CREATE_STAT(dir_name, status) APPLY_MASK(NU_TRACE_STOR_INFO, STOR_DIR_CREATE_STAT_EID,   \
                                            Log_pCI(STOR_DIR_CREATE_STAT_EID, dir_name, status))
#define T_DIR_DEL_STAT(dir_name, status)    APPLY_MASK(NU_TRACE_STOR_INFO, STOR_DIR_DEL_STAT_EID,      \
                                            Log_pCI(STOR_DIR_DEL_STAT_EID, dir_name, status))
#define T_FILE_OPEN_INFO(mp, dev_name, file_name, flag, mode, vfs_fd)                                  \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_OPEN_INFO_EID,    \
                                            Log_pCpCpCU16U16I(STOR_FILE_OPEN_INFO_EID, mp,             \
                                            dev_name, file_name, flag, mode, vfs_fd))
#define T_FILE_OPEN_STAT(file_name, flag, mode, vfs_fd)                                                \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_OPEN_STAT_EID,    \
                                            Log_pCU16U16I(STOR_FILE_OPEN_STAT_EID, file_name,          \
                                            flag, mode, vfs_fd))
#define T_FILE_CLOSE_STAT(vfs_fd, status)   APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_CLOSE_STAT_EID,   \
                                            Log_II(STOR_FILE_CLOSE_STAT_EID, vfs_fd, status))
#define T_FILE_READ_START(byte_count, vfs_fd)                                                          \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_READ_START_EID,   \
                                            Log_I32I(STOR_FILE_READ_START_EID, byte_count, vfs_fd))
#define T_FILE_READ_STOP(byte_count, vfs_fd, ret_val)                                                  \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_READ_STOP_EID,    \
                                            Log_I32II32(STOR_FILE_READ_STOP_EID, byte_count,           \
                                            vfs_fd, ret_val))
#define T_FILE_WRITE_START(byte_count, vfs_fd)                                                         \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_WRITE_START_EID,  \
                                            Log_I32I(STOR_FILE_WRITE_START_EID, byte_count, vfs_fd))
#define T_FILE_WRITE_STOP(byte_count, vfs_fd, ret_val)                                                 \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_WRITE_STOP_EID,   \
                                            Log_I32II32(STOR_FILE_WRITE_STOP_EID, byte_count,          \
                                            vfs_fd, ret_val))
#define T_FILE_SEEK_START(offset_bytes, origin, vfs_fd)                                                \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_SEEK_START_EID,   \
                                            Log_I32I16I(STOR_FILE_SEEK_START_EID,                      \
                                            offset_bytes, origin, vfs_fd))
#define T_FILE_SEEK_STOP(offset_bytes, origin, vfs_fd, ret_val)                                        \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_SEEK_STOP_EID,    \
                                            Log_I32I16II32(STOR_FILE_SEEK_STOP_EID,                    \
                                            offset_bytes, origin, vfs_fd, ret_val))
#define T_FILE_FLUSH_START(vfs_fd)          APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_FLUSH_START_EID,  \
                                            Log_I(STOR_FILE_FLUSH_START_EID, vfs_fd))
#define T_FILE_FLUSH_STOP(vfs_fd, status)   APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_FLUSH_STOP_EID,   \
                                            Log_II(STOR_FILE_FLUSH_STOP_EID, vfs_fd, status))
#define T_FILE_TRUNC_START(offset_bytes, vfs_fd)                                                       \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_TRUNC_START_EID,  \
                                            Log_I32I(STOR_FILE_TRUNC_START_EID, offset_bytes, vfs_fd))
#define T_FILE_TRUNC_STOP(offset_bytes, vfs_fd, status)                                                \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_TRUNC_STOP_EID,   \
                                            Log_I32II(STOR_FILE_TRUNC_STOP_EID, offset_bytes, vfs_fd,  \
                                            status))
#define T_FILE_RENAME_STAT(file_name, newname, status)                                                 \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_RENAME_STAT_EID,  \
                                            Log_pCpCI(STOR_FILE_RENAME_STAT_EID, file_name,            \
                                            newname, status))
#define T_FILE_DEL_STAT(file_name, status)                                                             \
                                            APPLY_MASK(NU_TRACE_STOR_INFO, STOR_FILE_DEL_STAT_EID,     \
                                            Log_pCI(STOR_FILE_DEL_STAT_EID, file_name, status))

/**********************/
/* Nucleus Networking */
/**********************/
#define T_NET_INIT_STAT(status)             APPLY_MASK(NU_TRACE_NET_INFO, NET_STACK_INIT_STATUS_EID,   \
                                            Log_I(NET_STACK_INIT_STATUS_EID, status))
#define T_DEV_LIST(dev_name, macaddr, type, flags, status, size)        							   \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_LIST_EID,            \
                                            Log_pCpU8U8U32I_pU8ArraySize(NET_DEV_LIST_EID, dev_name,   \
                                            macaddr, type, flags, status, size))
#define T_DEV_REMOVAL(dev_name, status)     APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_REMOVAL_EID,  	   \
                                            Log_pCI(NET_DEV_REMOVAL_EID, dev_name, status))
#define T_DEV_IPv4_IP(dev_name, ip, subnet, status, size)										       \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_IPv4_IP_EID,  	   \
                                            Log_pCpU8pU8I_pU8ArraySize(NET_DEV_IPv4_IP_EID, dev_name,  \
                                            ip, subnet, status, size))
#define T_DEV_IPv4_DHCP_IP(dev_name, ip, status, size)												   \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_IPv4_DHCP_IP_EID,    \
                                            Log_pCpU8I_pU8ArraySize(NET_DEV_IPv4_DHCP_IP_EID, dev_name,\
                                            ip, status, size))
#define T_DEV_IPv6_IP(dev_name, ip, prefix_len, status, size)                                          \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_IPv6_IP_EID,         \
                                            Log_pCpU8U8I_pU8ArraySize(NET_DEV_IPv6_IP_EID, dev_name,   \
                                            ip, prefix_len, status, size))
#define T_REMOVE_IP(dev_name, ip, family, status, size)                                                 \
{                                                                                                       \
    if (family == NU_FAMILY_IP6)                                                                        \
    {                                                                                                   \
        APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_REMOVE_IP6_EID,                                           \
                   Log_pCpU8I16I_pU8ArraySize(NET_DEV_REMOVE_IP6_EID, dev_name,                         \
                                              ip, family, status, size));                               \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_REMOVE_IP_EID,                                            \
                   Log_pCpU8I16I_pU8ArraySize(NET_DEV_REMOVE_IP_EID, dev_name,                          \
                                              ip, family, status, size));                               \
    }                                                                                                   \
}
#define T_DETACH_IP(dev_name, status)       APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_DETACH_IP_EID,       \
                                            Log_pCI(NET_DEV_DETACH_IP_EID, dev_name, status))
#define T_DEV_LINK_UP(dev_name, status)     APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_LINK_UP_EID,         \
                                            Log_pCI(NET_DEV_LINK_UP_EID, dev_name, status))
#define T_DEV_LINK_DOWN(dev_name, status)   APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_LINK_DOWN_EID,       \
                                            Log_pCI(NET_DEV_LINK_DOWN_EID, dev_name, status))
#define T_DEV_UP_STATUS(dev_name, status)   APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_UP_STATUS_EID,       \
                                            Log_pCI(NET_DEV_UP_STATUS_EID, dev_name, status))
#define T_SOCK_LIST(family, type, protocol, sd)                                                        \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_LIST_EID,           \
                                            Log_I16I16I16I(NET_SOCK_LIST_EID, family, type, protocol, sd))
#define T_SOCK_STATUS(state, sd, status)    APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_STATUS_EID,         \
                                            Log_U16II(NET_SOCK_STATUS_EID, state, sd, status))
#define T_SOCK_ENQ(packets, total_bytes, sd)  APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_ENQ_EID,          \
                                            Log_U32U32I(NET_SOCK_ENQ_EID, packets, total_bytes, sd))
#define T_SOCK_TX_LAT_START(sd)             APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_TX_START_EID,       \
                                            Log_I(NET_SOCK_TX_START_EID, sd))                                            
#define T_SOCK_TX_LAT_STOP(bytes, sd, status) APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_TX_STOP_EID,      \
                                            Log_U16II32(NET_SOCK_TX_STOP_EID, bytes, sd, status))
#define T_SOCK_RX(bytes, sd, status)        APPLY_MASK(NU_TRACE_NET_INFO, NET_SOCK_RX_EID,             \
                                            Log_U16II32(NET_SOCK_RX_EID, bytes, sd, status))
#define T_ADD_ROUTE(dev_name, dest, subnet, gateway, status, size)                                     \
{                                                                                                      \
    UINT8               t_gateway[IP_ADDR_LEN];                                                        \
    PUT32(t_gateway, 0, gateway);                                                                      \
    APPLY_MASK(NU_TRACE_NET_INFO, NET_ADD_ROUTE_EID,                                                   \
               Log_pCpU8pU8pU8I_pU8ArraySize(NET_ADD_ROUTE_EID, dev_name,                              \
                                             dest, subnet, t_gateway, status, size));                  \
                                                                                                       \
}
#define T_ADD_ROUTE6(dev_name, dest, next_hop, prefix_len, status, size)                               \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_ADD_ROUTE6_EID,           \
                                            Log_pCpU8pU8U8I_pU8ArraySize(NET_ADD_ROUTE6_EID, dev_name,  \
                                            dest, next_hop, prefix_len, status, size))
#define T_DEL_ROUTE(dest, next_hop, status, size)                                                      \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEL_ROUTE_EID,           \
                                            Log_pU8pU8I_pU8ArraySize(NET_DEL_ROUTE_EID, dest, next_hop, status, size))
#define T_DEL_ROUTE6(dest, next_hop, status, size)                                                     \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEL_ROUTE6_EID,          \
                                            Log_pU8pU8I_pU8ArraySize(NET_DEL_ROUTE6_EID, dest, next_hop, status, size))
#define T_BUFF_USAGE(buff_count)            APPLY_MASK(NU_TRACE_NET_INFO, NET_BUFF_USAGE_EID,          \
                                            Log_U16(NET_BUFF_USAGE_EID, buff_count))
#define T_DEV_TX_LAT_START(dev_name)        APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_TX_LAT_START_EID,    \
                                            Log_pC(NET_DEV_TX_LAT_START_EID, dev_name))
#define T_DEV_TX_LAT_STOP(dev_name, data_len, bytes_written, status)                                   \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_TX_LAT_STOP_EID,     \
                                            Log_pCU32U32I(NET_DEV_TX_LAT_STOP_EID, dev_name, data_len, \
                                            bytes_written, status))
#define T_DEV_RX_ACT(dev_name, data_len, status)                                                       \
                                            APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_RX_ACT_EID,          \
                                            Log_pCU32I(NET_DEV_RX_ACT_EID, dev_name, data_len, status))
#define T_DEV_TRANSQ_LEN(dev_name, q_len)   APPLY_MASK(NU_TRACE_NET_INFO, NET_DEV_TRANSQ_LEN_EID,      \
                                            Log_pCU32(NET_DEV_TRANSQ_LEN_EID, dev_name, q_len))

#else

/********************************/
/* Nucleus PC Hot Spot Analysis */
/********************************/
#define T_PC_SAMPLE()

/*********/
/* LISRs */
/*********/
#define T_IRQ_ENABLED(irq_num, status)
#define T_IRQ_DISABLED(irq_num, status)
#define T_LISR_REGISTERED(irq_num, status)
#define T_LISR_ENTRY(irq_num)
#define T_LISR_EXIT(irq_num)

/*********/
/* HISRs */
/*********/
#define T_HISR_CREATED(p_hcb, p_hisr_entry, p_stack, p_hisr_name, p_size, priority, status)
#define T_HISR_DELETED(p_hcb, status)
#define T_HISR_ACTIVATED(p_hcb, status)
#define T_HISR_RUNNING(p_hcb)
#define T_HISR_STOPPED(p_hcb)

/*********/
/* Tasks */
/*********/
#define T_TASK_CREATED(p_tcb, p_task, p_stk, p_name, stk_size, time_slice, priority,        \
        preempt_opt, strt_opt, status)
#define T_TASK_RESET(p_tcb, status)
#define T_TASK_READY(p_tcb)
#define T_TASK_RUNNING(p_tcb)
#define T_TASK_SUSPEND(p_tcb, sus_type)
#define T_TASK_TERMINATED(p_tcb, status)
#define T_TASK_DELETED(p_tcb, status)
#define T_TASK_CHG_PREMPTION(p_tcb, new_prempt_state, old_prempt_state)
#define T_TASK_CHG_PRIORITY(p_tcb, new_priority, old_priority)
#define T_TASK_CHG_TIMESLICE(p_tcb, new_timeslice, old_timeslice)

/****************/
/* Memory pools */
/****************/
#define T_MEM_POOL_CREATE(p_mpcb, p_start_add, p_name, size, available_size, min_size, sus_typ, status)
#define T_MEM_POOL_DELETE(p_mpcb, status)
#define T_MEM_ADD(p_mpcb, p_start_add, p_caller, total_size, available_size, added_size, status)

/******************/
/* Dynamic memory */
/******************/
#define T_DMEM_ALLOCATE(p_mpcb, p_start, p_caller, toatl_size, available_size, size, sus_type, status)
#define T_DMEM_DEALLOCATE(p_mpcb, p_start, p_caller, total_size, available_size, status)
#define T_DMEM_REALLOCATE(p_mpcb, p_orig_mem, p_caller, total_size, available_size, new_size, susp_type, status)

/********************/
/* Partition memory */
/********************/
#define T_PMEM_CREATE(p_pmpcb, p_start_add, p_name, p_avail, p_size, sus_type, status)
#define T_PMEM_ALLOCATE(p_pmpcb, p_partition, p_caller, p_poolsize, partition_size, available_size,\
                        allocated_size, susp_type, status)
#define T_PMEM_DEALLOCATE(p_pmpcb, p_partition, p_caller, p_poolsize, partition_size, available_size,       \
                          allocated_size, status)
#define T_PMEM_DELETE(p_pmpcb, status)

/**************/
/* Semaphores */
/**************/
#define T_SEM_CREATE(p_scb, p_name, ini_count, susp_type, status)
#define T_SEM_DELETE(p_pcb, p_tcb, status)
#define T_SEM_RESET(p_pcb, p_tcb, init_count, status)
#define T_SEM_OBTAIN(p_pcb, p_tcb, suspend, status)
#define T_SEM_RELEASE(p_pcb, p_tcb, status)

/***********/
/* Mailbox */
/***********/
#define T_MBOX_CREATE(p_mcb, p_name, susp_type, status)
#define T_MBOX_RESET(p_mcb, status)
#define T_MBOX_BCAST(p_mcb, p_msg, susp_type, status)
#define T_MBOX_SEND(p_mcb, p_msg, susp_type, status)
#define T_MBOX_RECV(p_mcb, p_msg, susp_type, status)
#define T_MBOX_DELETE(p_mcb, status)

/**********/
/* Queues */
/**********/
#define T_Q_CREATE(p_qcb, p_start, p_name, q_size, msg_size, msg_type, susp_type, status)
#define T_Q_RESET(p_qcb, status)
#define T_Q_BCAST(p_qcb, p_msg, size, susp, status)
#define T_Q_SEND(p_qcb, p_msg, size, susp, status)
#define T_Q_SEND2FRONT(p_qcb, p_msg, size, susp, status)
#define T_Q_RECV(p_qcb, p_msg, q_size, a_size, susp, status)
#define T_Q_DELETE(p_qcb, status)

/*********/
/* Pipes */
/*********/
#define T_PIPE_CREATE(p_pcb, p_start, p_name, pip_size, msg_size, msg_type, susp, status)
#define T_PIPE_RESET(p_pcb, status)
#define T_PIPE_BCAST(p_pcb, p_msg, size, suspend, status)
#define T_PIPE_SEND(p_pcb, p_msg, size, suspend, status)
#define T_PIPE_SEND2FRONT(p_pcb, p_msg, size, suspend, status)
#define T_PIPE_RECV(p_pcb, p_msg, p_size, a_size, suspend, status)
#define T_PIPE_DELETE(p_pcb, status)

/****************/
/* Event groups */
/****************/
#define T_EVT_GRP_CREATE(p_evt_grp, p_name, status)
#define T_EVT_SET(p_evt_grp, evt_flags, operation, status)
#define T_EVT_RETRIEVE(p_evt_grp, req_evt, ret_events, suspend, operation, status)
#define T_EVT_GRP_DELETE(p_evt_grp, status)

/***********/
/* Signals */
/***********/
#define T_SIG_REG_HANDLER(p_tcb, p_sig_hdlr, status)
#define T_SIG_CONTROL(p_tcb, enbl_signal_mask, old_signal_mask)
#define T_SIG_SEND(p_tcb, signals, status)
#define T_SIG_RECV(p_tcb, signals)
#define T_SIG_HANDLER_RUNNING(p_tcb, p_sig_hdlr)
#define T_SIG_HANDLER_STOPPED(p_tcb, p_sig_hdlr)

/**********/
/* Timers */
/**********/
#define T_TIMER_CREATE(p_tcb, p_exp_routine, p_name, id, init_time,                         \
                       resched_time, enable, status)
#define T_TIMER_DELETE(p_tcb, status)
#define T_TIMER_RESET(p_tcb, p_exp_routine, init_time, resched_time, enable, status)
#define T_TIMER_CONTROL(p_tcb, enable, status)
#define T_TIMER_PAUSE(p_tcb, status)
#define T_TIMER_RESUME(p_tcb, status)
#define T_TIMER_EXP_ROUTIN_RUNNING(p_tcb, id)
#define T_TIMER_EXP_ROUTIN_STOPPED(p_tcb, id)

/********/
/* Idle */
/********/
#define T_IDLE_ENTRY()
#define T_IDLE_EXIT()

/******************/
/* Power Services */
/******************/
#define T_OP_INFO(op_id, volt_id, volt_val, freq_id, freq_val)
#define T_OP_TRANS(op_id, status)
#define T_HIB_ENTER(level, status)
#define T_HIB_EXIT(state, status)
#define T_HIB_EXIT_OP(op_id, status)
#define T_HIB_BOOT(boot_state)
#define T_SYS_COUNT(state_count, status)
#define T_SYS_TRANS(state, status)
#define T_SYS_MAP(dev_id, dev_state, sys_state, status)
#define T_SYS_UNMAP(dev_id, status)
#define T_DEV_TRANS(dev_id, state, status)
#define T_WD_CREATE(wd, status)
#define T_WD_ACTIVE(wd)
#define T_WD_EXPIRE(wd)
#define T_WD_DELETE(wd, status)
#define T_TICK_SUPPRESS(flag)
#define T_DEV_NAME(key, state, state_count, dev_id)
/* Power Services Transition Latency */
#define T_DEV_TRANS_START(dev_id)
#define T_DEV_TRANS_STOP(dev_id)
#define T_OP_TL_START()
#define T_OP_TL_STOP()
#define T_HIB_TL_START()
#define T_HIB_TL_STOP()
#define T_SYS_TL_START()
#define T_SYS_TL_STOP()

/*******************/
/* Nucleus Storage */
/*******************/
#define T_PHY_DEV_LIST(dev_name)
#define T_PHY_DEV_REMOVAL(dev_name, status)
#define T_LOG_DEV_REMOVAL(dev_name, status)
#define T_LOG_DEV_INFO(dev_name, fs, mp, auto_fmt, sect_start, sect_end, status)
#define T_DIR_CREATE_INFO(mp, dev_name, dir_name)
#define T_DIR_CREATE_STAT(dir_name, status)
#define T_DIR_DEL_STAT(dir_name, status)
#define T_FILE_OPEN_INFO(mp, dev_name, file_name, flag, mode, vfs_fd)
#define T_FILE_OPEN_STAT(file_name, flag, mode, vfs_fd)
#define T_FILE_CLOSE_STAT(vfs_fd, status)
#define T_FILE_READ_START(byte_count, vfs_fd)
#define T_FILE_READ_STOP(byte_count, vfs_fd, ret_val)
#define T_FILE_WRITE_START(byte_count, vfs_fd)
#define T_FILE_WRITE_STOP(byte_count, vfs_fd, ret_val)
#define T_FILE_SEEK_START(offset_bytes, origin, vfs_fd)
#define T_FILE_SEEK_STOP(offset_bytes, origin, vfs_fd, ret_val)
#define T_FILE_FLUSH_START(vfs_fd)
#define T_FILE_FLUSH_STOP(vfs_fd, status)
#define T_FILE_TRUNC_START(offset_bytes, vfs_fd)
#define T_FILE_TRUNC_STOP(offset_bytes, vfs_fd, status)
#define T_FILE_RENAME_STAT(file_name, newname, status)
#define T_FILE_DEL_STAT(file_name, status)

/**********************/
/* Nucleus Networking */
/**********************/
#define T_NET_INIT_STAT(status)
#define T_DEV_LIST(dev_name, macaddr, type, flags, status, size)
#define T_DEV_REMOVAL(dev_name, status)
#define T_DEV_IPv4_IP(dev_name, ip, subnet, status, size)
#define T_DEV_IPv4_DHCP_IP(dev_name, ip, status, size)
#define T_DEV_IPv6_IP(dev_name, ip, prefix_len, status, size)
#define T_REMOVE_IP(dev_name, ip, family, status, size)
#define T_DETACH_IP(dev_name, status)
#define T_DEV_LINK_UP(dev_name, status)
#define T_DEV_LINK_DOWN(dev_name, status)
#define T_DEV_UP_STATUS(dev_name, status)
#define T_SOCK_LIST(family, type, protocol, sd)
#define T_SOCK_STATUS(state, sd, status)
#define T_SOCK_ENQ(packets, total_bytes, sd)
#define T_SOCK_TX_LAT_START(sd)
#define T_SOCK_TX_LAT_STOP(bytes, sd, status)
#define T_SOCK_RX(bytes, sd, status)
#define T_ADD_ROUTE(dev_name, dest, subnet, gateway, status, size)
#define T_ADD_ROUTE6(dev_name, dest, next_hop, prefix_len, status, size)
#define T_DEL_ROUTE(dest, next_hop, status, size)
#define T_DEL_ROUTE6(dest, next_hop, status, size)
#define T_BUFF_USAGE(buff_count)
#define T_DEV_TX_LAT_START(dev_name)
#define T_DEV_TX_LAT_STOP(dev_name, data_len, bytes_written, status)
#define T_DEV_RX_ACT(dev_name, data_len, status)
#define T_DEV_TRANSQ_LEN(dev_name, q_len)

#endif /* (CFG_NU_OS_SVCS_TRACE_CORE_TRACE_SUPPORT == NU_TRUE) */

#ifdef          __cplusplus
}   /* End of C declarations */
#endif  /* __cplusplus */

#endif /* _OS_MARK_H_ */
