/*************************************************************************
*
*            Copyright 2012 Mentor Graphics Corporation
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
*        cpu_selfrefresh.h
*
*   COMPONENT
*
*        CPU driver
*
*   DESCRIPTION
*
*        Contains definitions of the CPU self-refresh component
*
*   DATA STRUCTURES
*
*        None
*
*   DEPENDENCIES
*
*        None
*
*************************************************************************/
#ifndef CPU_SELFREFRESH_H
#define CPU_SELFREFRESH_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_SELFREFRESH_CLASS_LABEL          {0x05,0x32,0xbd,0x5f,0x32,0xc6,0x42,0x0f,0x80,0x0d,0x6b,0xfe,0xc9,0xbc,0xc1,0x92}

/* Self Refresh IOCTL offset codes */
#define CPU_SELFREFRESH_IOCTL_INIT           0     /*  */

#define CPU_SELFREFRESH_IOCTL_TOTAL          1

/* Self-Refresh Function type */
typedef VOID (*SELF_REFRESH_FUNC)(VOID);

/* Self-Refresh control block */
typedef struct CPU_SELF_REFRESH_CB_STRUCT
{
    BOOLEAN                 self_refresh;
    SELF_REFRESH_FUNC       selfrefresh_enter_func;
    SELF_REFRESH_FUNC       selfrefresh_exit_func;

} CPU_SELF_REFRESH_CB;


#ifdef __cplusplus
}
#endif

#endif /* CPU_SELFREFRESH_H */
