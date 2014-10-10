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
*   FILE NAME
*
*       syslog_extern.h
*
*   DESCRIPTION
*
*       This file contains external interfaces, defines, etc for the
*       system logging component
*
*   DATA STRUCTURES
*
*       SYSLOG_HANDLE
*
*   DEPENDENCIES
*
*       device_manager.h
*
***********************************************************************/
#ifndef SYSLOG_EXTERN_H
#define SYSLOG_EXTERN_H

#ifdef CFG_NU_OS_SVCS_SYSLOG_ENABLE

#include  "kernel/dev_mgr.h"

/* Typedef for system logging handle */
typedef         DV_DEV_HANDLE               SYSLOG_HANDLE;

#else

/* Typedef for system logging handle */
typedef         INT32                       SYSLOG_HANDLE;

#endif  /* CFG_NU_OS_SVCS_SYSLOG_ENABLE */


/* Define system logging device class label */
#define         SYSLOGGER_CLASS_LABEL       {0xaf,0x77,0x94,0xed,0x2c,0x12,0x46,0xdd,0x9c,0xa6,0x87,0xab,0xad,0xe8,0x26,0xb7}
    
/* Define supported IOCTLs */
#define         SYSLOG_CMD_BASE             (DV_IOCTL0 + 1)
#define         SYSLOG_SET_NAME_CMD         1
#define         SYSLOG_SET_MASK_CMD         2

/* Define for invalid device handle */
#define         SYSLOG_INVALID_HANDLE       (0xFFFFFFFF)

#ifdef CFG_NU_OS_SVCS_SYSLOG_ENABLE

/* External function prototypes */
SYSLOG_HANDLE   SysLogOpen(const char * name);
VOID            SysLogSetMask(SYSLOG_HANDLE handle, UINT32 logmask);
VOID            SysLog(SYSLOG_HANDLE handle, const char * msg, UINT32 loglevel);
VOID            SysLogClose(SYSLOG_HANDLE handle);

#else

/* Define empty macros to allow use of System logging calls within code when component doesn't exist */
#define        SysLogOpen(x)                0
#define        SysLogSetMask(x,y)
#define        SysLog(x,y,z)
#define        SysLogClose(x)

#endif  /* CFG_NU_OS_SVCS_SYSLOG_ENABLE */

#endif  /* SYSLOG_EXTERN_H */
