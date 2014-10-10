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
*       This file contains Nucleus PLUS configuration settings,
*       compile-time options, etc
*
************************************************************************/

/* Check to see if this file has been included already.  */

#ifndef PLUS_CFG
#define PLUS_CFG

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

/* DEFINE:      NU_POSIX_INCLUDED
   DEFAULT:     NU_FALSE
   DESCRIPTION: POSIX specific code changes are compiled into Nucleus PLUS when this
                define is set to NU_TRUE.  These POSIX related items will not be included
                when set to NU_FALSE.
   NOTE:        The Nucleus Posix software is required to utilize this option.
                The Nucleus PLUS library and application must be rebuilt after changing
                this define.            */
#ifndef         NU_POSIX_INCLUDED
#define         NU_POSIX_INCLUDED                   NU_FALSE
#endif

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif  /* !PLUS_CFG */
