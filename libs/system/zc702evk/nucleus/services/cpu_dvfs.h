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
*       cpu_dvfs.h
*
*   COMPONENT
*
*       CPU DVFS
*
*   DESCRIPTION
*
*       Contains data structures and IOCTLs for the DVFS component
*
*   DATA STRUCTURES
*
*       CPU_DVFS_OP
*       CPU_DVFS_GET_OP
*       CPU_DVFS_FROM_TO
*       CPU_DVFS_ADDITIONAL
*
*   DEPENDENCIES
*
*       nucleus.h
*
*************************************************************************/
#ifndef CPU_DVFS_H
#define CPU_DVFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Class Label */
#define CPU_DVFS_CLASS_LABEL          {0x30,0x4b,0xc2,0xbc,0x29,0xaa,0x49,0xe5,0xaa,0x69,0x36,0x36,0xaa,0xb5,0xac,0xc7}

/* CPU IOCTL offset codes */
#define CPU_DVFS_IOCTL_GET_OP_COUNT   0     /* Get the current number of operating points */
#define CPU_DVFS_IOCTL_GET_OP         1     /* Get a specific operating point with the PM_CPU_GET_OP structure */
#define CPU_DVFS_IOCTL_SET_OP         2     /* Set a specific operating point based on an index from 0..n-1 as derived from get op count */
#define CPU_DVFS_IOCTL_GET_CURRENT_OP 3     /* Get the index of the current operating point in use */
#define CPU_DVFS_IOCTL_FROM_TO        4     /* Get the amount of time to make a switch, time returned in ns */
#define CPU_DVFS_IOCTL_ADDITIONAL     5     /* If any extra information is available it will be returned using a structure defined by the cpu driver */
#define CPU_DVFS_IOCTL_SPECIFIC       6     /* If any extra information is available it will be returned using a structure defined by the cpu driver */

#define CPU_DVFS_IOCTL_TOTAL          7

/* This structure is passed in to IOCTL "Get OP" */
typedef struct CPU_DVFS_GET_OP_CB
{
    UINT8  pm_op_index;                     /* The OP index being requested by PM */
    UINT16 pm_voltage;                      /* Voltage in mHz */
#if PAD_3
    UINT8  pm_padding[PAD_3];
#endif
    UINT32 pm_frequency;                    /* Frequency in HZ */
} CPU_DVFS_GET_OP;

/* This structure is passed in to IOCTL "from to" */
typedef struct CPU_DVFS_FROM_TO_CB
{
    UINT8  pm_op_from;                      /* The OP index being starting from */
    UINT8  pm_op_to;                        /* The OP index switching to */
#if PAD_2
    UINT8  pm_padding[PAD_2];
#endif
    UINT32 pm_time;                         /* Required time to make the switch in nanoseconds */
} CPU_DVFS_FROM_TO;

/* This structure is passed in to IOCTL "additional info" */
typedef struct CPU_DVFS_ADDITIONAL_CB
{
    UINT8  pm_op_id;                        /* The OP index requested */
#if PAD_1
    UINT8  pm_padding[PAD_1];
#endif
    VOID  *pm_info;                         /* Pointer to CPU defined structure */
    UINT32 pm_size;                         /* Size of the information pointer */
} CPU_DVFS_ADDITIONAL;

/* This structure is passed in to IOCTL "additional info" */
typedef struct CPU_DVFS_SPECIFIC_CB
{
    UINT8  pm_op_id;                        /* The OP index requested */
#if PAD_1
    UINT8  pm_padding[PAD_1];
#endif
    CHAR  *pm_identifier;
    VOID  *pm_info;                         /* Pointer to CPU defined structure */
    UINT32 pm_size;                         /* Size of the information pointer */
} CPU_DVFS_SPECIFIC;

#ifdef __cplusplus
}
#endif

#endif /* CPU_DVFS_H */
