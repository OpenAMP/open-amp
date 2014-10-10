/*************************************************************************
*
*               Copyright Mentor Graphics Corporation 1999
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
**************************************************************************

**************************************************************************
*
* FILE NAME                                                
*
*      eh_defs.h                                        
*
* COMPONENT
*
*      EH - Exception Handling
*
* DESCRIPTION
*
*      Type definitions for Exception Handling
*
* DATA STRUCTURES
*
*      None
*
* PUBLIC FUNCTIONS
*
*      None
*
* PRIVATE FUNCTIONS
*
*      None
*
* DEPENDENCIES
*
*      None
*
*************************************************************************/

#ifndef EH_DEFS_H
#define EH_DEFS_H


#ifdef __cplusplus
extern "C" {
#endif

/* Exception control block identifier is 'EXCE' */
#define EH_EXCEPTION_ID 0x45584345UL

struct EH_EXCEPTION
{
    UNSIGNED            eh_id;              /* Internal exception ID */
    VOID               *eh_thread;          /* Offending thread */
    VOID               *eh_address;         /* Address where error occurred */
    INT                 eh_type;            /* Type of error that occurred */
};

#ifndef __cplusplus
typedef struct EH_EXCEPTION EH_EXCEPTION;
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef EH_DEFS_H */


