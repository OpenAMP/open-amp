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
*      eh_extr.h                                        
*
* COMPONENT
*
*      EH - Exception Handling
*
* DESCRIPTION
*
*      Interface to Exception Handling routines
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
*      eh_defs.h
*      ms_defs.h
*
*************************************************************************/

#ifndef EH_EXTR_H
#define EH_EXTR_H

#include "eh_defs.h"
#include "ms_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

STATUS EHC_Register_Exception_Handler(MS_MODULE *module_ptr,
                                      VOID(*exception_entry)(MS_MODULE *,EH_EXCEPTION *),
                                      VOID(**old_exception)(MS_MODULE *,EH_EXCEPTION *));

STATUS EHC_Set_Default_Exception_Handler(VOID(*exception_entry)(MS_MODULE *,EH_EXCEPTION *),
                                         VOID(**old_exception)(MS_MODULE *,EH_EXCEPTION *));

STATUS EHF_Get_Default_Exception_Handler(VOID(**exception_ptr)(MS_MODULE *,EH_EXCEPTION *));

STATUS EHFM_Module_Exception_Handler(MS_MODULE *module_ptr,
                                     VOID(**exception_ptr)(MS_MODULE *,EH_EXCEPTION *));

#ifdef __cplusplus
}
#endif


#endif /* #ifndef EH_EXTR_H */


