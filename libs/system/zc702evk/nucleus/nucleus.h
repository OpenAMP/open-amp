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
*       This file contains system constants common to both the
*       application and the actual Nucleus PLUS components.
*
***********************************************************************/

/* Check to see if this file has been included already.  */

#ifndef         NUCLEUS_H

#ifdef          __cplusplus
/* C declarations in C++     */
extern          "C" {
#endif
#define         NUCLEUS_H

/* Include auto-generated configuration file */
#include        "nucleus_gen_cfg.h"

/* Macro for generating a release version from major, minor, and patch version */
#define         NU_RELEASE_VERSION_MAKE(m,n,p)      ((m*10000)+(n*100)+p)

/* Define major, minor, and patch release numbers for Nucleus. */
#define         NUCLEUS_RELEASE_MAJOR_VERSION       CFG_NU_MAJOR_VERSION
#define         NUCLEUS_RELEASE_MINOR_VERSION       CFG_NU_MINOR_VERSION
#define         NUCLEUS_RELEASE_PATCH_VERSION       CFG_NU_PATCH_VERSION

/* Temporary macro to zero append the minor version, if it is less than 10. */
#if             (NUCLEUS_RELEASE_MINOR_VERSION < 10)
#define         ZERO_APPEND                         0
#else
#define         ZERO_APPEND
#endif          /* (NUCLEUS_RELEASE_MINOR_VERSION < 10) */

/* Temporary macros to convert a number to string. */
#define         STR_PASTE(x)                        #x
#define         STR(x)                              STR_PASTE(x)

/* Define Nucleus release string.  NOTE: this is the version number
   for the whole operating system. */
#define         NUCLEUS_RELEASE_STRING              "Nucleus " STR(NUCLEUS_RELEASE_MAJOR_VERSION) "." \
                                              STR(ZERO_APPEND) STR(NUCLEUS_RELEASE_MINOR_VERSION) "." \
                                                               STR(NUCLEUS_RELEASE_PATCH_VERSION)
/* Undefine the temporary macros. */
#undef          ZERO_APPEND
#undef          STR_PASTE
#undef          STR

/* Define Nucleus numbering */
#define         NUCLEUS_1_0                         NU_RELEASE_VERSION_MAKE(1,0,0)
#define         NUCLEUS_2_0                         NU_RELEASE_VERSION_MAKE(2,0,0)
#define         NUCLEUS_3_1                         NU_RELEASE_VERSION_MAKE(3,1,0)
#define         NUCLEUS_3_2                         NU_RELEASE_VERSION_MAKE(3,2,0)
#define         NUCLEUS_3_2_1                       NU_RELEASE_VERSION_MAKE(3,2,1)
#define         NUCLEUS_3_3_0                       NU_RELEASE_VERSION_MAKE(3,3,0)

/* Current version */
#define         NUCLEUS_VERSION                     NU_RELEASE_VERSION_MAKE(            \
                                                        NUCLEUS_RELEASE_MAJOR_VERSION,  \
                                                        NUCLEUS_RELEASE_MINOR_VERSION,  \
                                                        NUCLEUS_RELEASE_PATCH_VERSION)
                                                        
/* Define NULL value */
#define         NU_NULL                             0

/* Defines for TRUE / FALSE conditions */
#define         NU_FALSE                            0
#define         NU_TRUE                             1

/* Define standard data types.  These definitions allow software components
   to perform in the same manner on different target platforms.  */
typedef         signed int                          INT;
typedef         unsigned int                        UINT;
typedef         signed char                         INT8;
typedef         unsigned char                       UINT8;
typedef         signed short                        INT16;
typedef         unsigned short                      UINT16;
typedef         signed long                         INT32;
typedef         unsigned long                       UINT32;
#define         VOID                                void
typedef         unsigned long long                  UINT64;
typedef         signed long long                    INT64;

/* Define type used to cast void pointers when doing pointer math */
typedef         UINT8 *                             VOID_CAST;
/* Define register variable definition.  This define
   can be used to specify local variables and parameters
   that could be placed into registers to increase performance
   (speed and size) */
#define         NU_REG                              register

/* Define macro to eliminate warnings for unused parameters */
#define         NU_UNUSED_PARAM(parameter)          (VOID)parameter

/* Get address of structure using structure member and type */
#include        <stddef.h>
#define         NU_STRUCT_BASE(pointer, member, type)                           \
                        ((type *)((size_t)(pointer) - offsetof(type, member)))

/* Common types utilized within Nucleus code */
typedef         UINT32                              UNSIGNED;
typedef         INT32                               SIGNED;
typedef         UINT8                               DATA_ELEMENT;
typedef         UINT8                               OPTION;
typedef         UINT8                               BOOLEAN;
typedef         INT                                 STATUS;
typedef         UINT8                               UNSIGNED_CHAR;
typedef         char                                CHAR;
typedef         UINT                                UNSIGNED_INT;
typedef         UINT32 *                            UNSIGNED_PTR;
typedef         UINT8 *                             BYTE_PTR;


/* Define register defines.  R1, R2, R3, and R4 are used in the Nucleus PLUS
   source code in front of variables that are referenced often.  In some
   ports, defining them as "register" will improve performance.  */
#define         R1                                  NU_REG
#define         R2                                  NU_REG
#define         R3                                  NU_REG
#define         R4                                  NU_REG

/* Define the maximum object string name size in the system */
#define         NU_MAX_NAME                         8

/* Define system errors.  */
#define         NU_ERROR_CREATING_TIMER_HISR        1
#define         NU_ERROR_CREATING_TIMER_TASK        2
#define         NU_STACK_OVERFLOW                   3
#define         NU_UNHANDLED_INTERRUPT              4
#define         NU_NOT_IN_SUPERVISOR_MODE           5
#define         NU_NOT_ENOUGH_DTLBS                 6
#define         NU_NOT_ENOUGH_ITLBS                 7
#define         NU_STACK_UNDERFLOW                  8
#define         NU_UNHANDLED_EXCEPTION              9
#define         NU_RUNLEVEL_INIT_ERROR              10
#define         NU_MPU_CONFIG_ERROR                 11
#define         NU_INVALID_LOCK_USAGE               12

/* Define I/O driver constants.  */
#define         NU_IO_ERROR                         -1

/**********************************************************************/
/*           Nucleus Toolset specific macros                          */
/**********************************************************************/                                                      
/* Include architecture and toolset specific headers */  
#include        "arch.h"
#include        "toolset.h"
#include        "platform.h"
#include        "kernel/esal_extr.h"

/* Define number of accesses required to read or write a 32-bit value */
#define         NU_32BIT_ACCESS                     ESAL_AR_32BIT_ACCESS

/* Define number of accesses required to read or write a pointer */
#define         NU_PTR_ACCESS                       ESAL_AR_PTR_ACCESS

#define         NU_WEAK_REF                         ESAL_TS_WEAK_REF

/* Defines used to align / pad structure members of various Nucleus structures.  Certain
   architectures may require alignment */
#ifndef         ESAL_TS_PAD_1BYTE_ALIGN
#define         PAD_1                               0
#else
#define         PAD_1                               ESAL_TS_PAD_1BYTE_ALIGN
#endif  /* !ESAL_TS_PAD_1BYTE_ALIGN */

#ifndef         ESAL_TS_PAD_2BYTE_ALIGN
#define         PAD_2                               0
#else
#define         PAD_2                               ESAL_TS_PAD_2BYTE_ALIGN
#endif  /* !ESAL_TS_PAD_2BYTE_ALIGN */

#ifndef         ESAL_TS_PAD_3BYTE_ALIGN
#define         PAD_3                               0
#else
#define         PAD_3                               ESAL_TS_PAD_3BYTE_ALIGN
#endif  /* !ESAL_TS_PAD_3BYTE_ALIGN */

/* Common types utilized within Nucleus code - toolset specific */
#if (ESAL_TS_64BIT_SUPPORT == NU_TRUE)
typedef         UINT64                              OFFSET_T;
#else
typedef         UINT32                              OFFSET_T;
#endif /* (ESAL_TS_64BIT_SUPPORT == NU_TRUE) */


/* Check if debug agent support is enabled */
#ifdef CFG_NU_OS_SVCS_DBG_ENABLE

/* Include debug agent definitions */
#include        "services/dbg_extr.h"

#endif /* CFG_NU_OS_SVCS_DBG_ENABLE */

/* Include System Logging interface */
#include        "services/syslog_extern.h"
                   
#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif  /* !NUCLEUS_H */

