/************************************************************************
*
*                  Copyright 2003 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS THE
* PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS SUBJECT
* TO LICENSE TERMS.
*
*************************************************************************

*************************************************************************
* FILE NAME
*
*       semaphore.h
*
* COMPONENT
*
*		SM - Semaphore.
*
* DESCRIPTION
*
*		This file contains the declaration of various routines about
*		semaphores.
*
* DATA STRUCTURES
*
*		None
*
* DEPENDENCIES
*
*		"fcntl.h"							Contains the requests and
*											arguments for use by the
*											functions fcntl( ) and
*											open( ).
*
************************************************************************/
#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#if (_POSIX_SEMAPHORES != -1)

#include "services/fcntl.h"

/* Semaphore mode of operations */
#ifndef POSIX_FLAG_PROCESS_PRIVATE
#define POSIX_FLAG_PROCESS_PRIVATE          0
#endif
#ifndef POSIX_FLAG_PROCESS_SHARED
#define POSIX_FLAG_PROCESS_SHARED           1
#endif

typedef int sem_t;

#define SEM_FAILED              ((sem_t *) -1)

/* Function Declarations.  */

#ifdef __cplusplus
extern "C" {
#endif

int sem_close(sem_t *);

int sem_destroy(sem_t *);

int sem_getvalue(sem_t *, int *);

int sem_init(sem_t *, int, unsigned);

sem_t*  sem_open(const char *,
                 int
                 ,...                       /* mode_t mode,
                                               unsigned value  */
                 );

int sem_post(sem_t *);

int sem_trywait(sem_t *);

#if(_POSIX_TIMEOUTS != -1)

int sem_timedwait(sem_t* sem, const struct timespec* abs_timeout);

#endif

int sem_unlink(const char *);

int sem_wait(sem_t *);

#ifdef __cplusplus
}
#endif

#endif /*  _POSIX_SEMAPHORES  */

#endif /*  __SEMAPHORE_H_  */




