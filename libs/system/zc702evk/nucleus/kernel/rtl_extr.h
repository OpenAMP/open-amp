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
*       rtl_extr.h
*
*   DESCRIPTION
*
*       This file contains the Nucleus run-time library functionality
*       external interface.
*
*   DATA STRUCTURES
*
*       None
*
*   FUNCTIONS
*
*       RTL_malloc
*       RTL_calloc
*       RTL_realloc
*       RTL_free
*
*   DEPENDENCIES
*
*       None
*
*************************************************************************/

#ifndef RTL_EXTR_H
#define RTL_EXTR_H

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

/* Nucleus malloc functions */

void *  RTL_malloc (size_t size);
void *  RTL_calloc (size_t nmemb, size_t size);
void *  RTL_realloc (void * ptr, size_t size);
void    RTL_free (void * ptr);

/* API to seed random number generator */

VOID NU_RTL_Rand_Seed(VOID);

/* Nucleus epoch function used by gettimeofday */
UINT64 RTL_Calc_Time_Since_Epoch(struct tm *time_ptr);

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif /* RTL_EXTR_H */
