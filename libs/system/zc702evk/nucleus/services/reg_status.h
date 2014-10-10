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
*       reg_status.h
*
*   COMPONENT
*
*       Registry
*
*   DESCRIPTION
*
*       This file defines the error codes used by the registry API.
*
*   DATA STRUCTURES
*
*       None
*
*	DEPENDENCIES
*
*       None.
*
*************************************************************************/

#ifndef REG_STATUS_H
#define REG_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Registry Error Codes */
#define REG_TYPE_ERROR             -1
#define REG_NOT_WRITABLE           -2
#define REG_BAD_PATH               -3

#ifdef __cplusplus
}
#endif

#endif
