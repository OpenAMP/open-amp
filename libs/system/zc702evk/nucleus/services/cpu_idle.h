/*************************************************************************
*
*            Copyright 2010 Mentor Graphics Corporation
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
*        cpu_idle.h
*
*   COMPONENT
*
*        CPU driver
*
*   DESCRIPTION
*
*        Contains definitions of the CPU idle component
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
#ifndef CPU_IDLE_H
#define CPU_IDLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_IDLE_CLASS_LABEL             {0xf2,0x07,0x01,0xf4,0x3f,0xb3,0x4e,0x5b,0x9e,0x99,0xf5,0xe8,0xbf,0xe7,0x6a,0x57}
#define CPU_DRAM_CTRL_CLASS_LABEL         {0x14,0x11,0xf7,0xdf,0x72,0x76,0x4c,0xf1,0x87,0xbc,0x77,0xdd,0x07,0xdc,0x16,0x55}

/* CPU Idle IOCTL offset codes */
#define CPU_IDLE_IOCTL_GET_IDLE     0
#define CPU_IDLE_IOCTL_GET_WAKEUP   1

/* CPU DRAM self refresh IOCTL offset codes */
#define CPU_DRAM_IOCTL_DISABLE_SELF_REFRESH    0
#define CPU_DRAM_IOCTL_ENABLE_SELF_REFRESH     1
#define CPU_DRAM_IOCTL_GET_SELF_REFRESH_STATUS 2

/* CPU Idle total number of ioctl */
#define CPU_IDLE_IOCTL_TOTAL        2
#define CPU_DRAM_IOCTL_TOTAL        3

/* flag to test tick suppression */
#define     TEST_TICK_SUPPRESS  NU_TRUE

#ifdef __cplusplus
}
#endif

#endif /* CPU_IDLE_H */
