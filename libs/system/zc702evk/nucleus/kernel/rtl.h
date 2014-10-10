/*************************************************************************
*
*             Copyright 2010 Mentor Graphics Corporation
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
*       rtl.h
*
*   DESCRIPTION
*
*       This file contains the Nucleus run-time library functionality
*
*   DATA STRUCTURES
*
*       None
*
*   FUNCTIONS
*
*       None
*
*   DEPENDENCIES
*
*       string.h                            String functions
*
*************************************************************************/

#include        <string.h>

#ifndef RTL_H
#define RTL_H

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

/* This macro sets the Nucleus memory pool used for Nucleus malloc
   operations. */

#if (CFG_NU_OS_KERN_RTL_MALLOC_POOL == 0)
#define         RTL_MALLOC_POOL System_Memory
#endif /* CFG_NU_OS_KERN_RTL_MALLOC_POOL == 0 */

#if (CFG_NU_OS_KERN_RTL_MALLOC_POOL == 1)
#define         RTL_MALLOC_POOL Uncached_System_Memory
#endif /* CFG_NU_OS_KERN_RTL_MALLOC_POOL == 1 */

#if (CFG_NU_OS_KERN_RTL_MALLOC_POOL == 2)
#define         RTL_MALLOC_POOL NU_Malloc_User_Mem_Pool
#endif /* CFG_NU_OS_KERN_RTL_MALLOC_POOL == 2 */

#ifndef RTL_MALLOC_POOL
#error ERROR: No Nucleus memory pool for Nucleus malloc support.
#endif /* RTL_MALLOC_POOL */

/* Macros for time.h sub-routines */
#define RTL_SEC_PER_YEAR                 31536000
#define RTL_SEC_PER_DAY                  86400
#define RTL_SEC_PER_HOUR                 3600
#define RTL_SEC_PER_MIN                  60
#define RTL_EPOCH_YEAR                   1970

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif /* RTL_H */
