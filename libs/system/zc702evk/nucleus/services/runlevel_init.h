/***********************************************************************
*
*            Copyright 2010 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************/

/***********************************************************************
*
*  DESCRIPTION
*
*       This file contains external interfaces, defines, etc for the
*       run-level component
*
***********************************************************************/

#ifndef     RUNLEVEL_INIT_H
#define     RUNLEVEL_INIT_H

#ifdef      __cplusplus
extern      "C" { /* C declarations in C++     */
#endif


/* Maximum supported run-level value (limit is number of event group bits: 32-bits = run-levels 0-31) */
#define     NU_RUNLEVEL_MAX             31

/* External definitions */
#define     NU_RUNLEVEL_NOT_STARTED     1
#define     NU_RUNLEVEL_IN_PROGRESS     2

/* Define Runlevel component control values */
#define     RUNLEVEL_STOP                       0
#define     RUNLEVEL_START                      1
#define     RUNLEVEL_HIBERNATE                  2
#define     RUNLEVEL_RESUME                     3

/* External function prototypes */
STATUS      NU_RunLevel_Current(INT * runlevel);
STATUS      NU_RunLevel_Status(INT runlevel);
STATUS      NU_RunLevel_Complete_Wait(INT runlevel, UNSIGNED timeout);
STATUS      NU_RunLevel_Next_Start(VOID);
STATUS      NU_RunLevel_0_Init(const CHAR * compregpath);

#ifdef          __cplusplus

/* End of C declarations */
}

#endif      /* __cplusplus */

#endif      /* RUNLEVEL_INIT_H */

