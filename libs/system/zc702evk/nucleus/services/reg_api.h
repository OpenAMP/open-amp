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
*       reg_api.h
*
*   COMPONENT
*
*       Registry
*
*   DESCRIPTION
*
*       This file contains the external interface for interacting with 
*       the registry.
*
*   DATA STRUCTURES
*
*       None.
*
*	DEPENDENCIES
*
*       nucleus.h
*       reg_status.h
*
*************************************************************************/

#ifndef REG_API_H
#define REG_API_H

#include "nucleus.h"
#include "services/reg_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions */

/* Maximum length of a key in the registry.  This value is automatically determined by FUSE for
   the given build configuration.  This is an implicitly defined configuration setting (ie there is
   no metadata configuration option associated with this macro - FUSE just defines this macro in the
   generated header file based on the longest key length in the configuration) */
#define REG_MAX_KEY_LENGTH    CFG_NU_OS_SVCS_REGISTRY_MAX_KEY_LEN

/* Queries */
BOOLEAN REG_Has_Key(const CHAR *key);
BOOLEAN REG_Key_Is_Writable(const CHAR *key);
STATUS REG_Num_Child_Keys_Value(const CHAR *key, const CHAR *sub_key, UINT *num);
#define REG_Num_Child_Keys(key, num) REG_Num_Child_Keys_Value(key, NU_NULL, num)

/* Reads and Writes */
STATUS REG_Set_Boolean_Value(const CHAR *key, const CHAR *sub_key, BOOLEAN value);
STATUS REG_Get_Boolean_Value(const CHAR *key, const CHAR *sub_key, BOOLEAN *value);
#define REG_Set_Boolean(key, value) REG_Set_Boolean_Value(key, NU_NULL, value)
#define REG_Get_Boolean(key, value) REG_Get_Boolean_Value(key, NU_NULL, value)

STATUS REG_Set_UINT8_Value(const CHAR *key, const CHAR *sub_key, UINT8 value);
STATUS REG_Get_UINT8_Value(const CHAR *key, const CHAR *sub_key, UINT8 *value);
#define REG_Set_UINT8(key, value) REG_Set_UINT8_Value(key, NU_NULL, value)
#define REG_Get_UINT8(key, value) REG_Get_UINT8_Value(key, NU_NULL, value)

STATUS REG_Set_UINT16_Value(const CHAR *key, const CHAR *sub_key, UINT16 value);
STATUS REG_Get_UINT16_Value(const CHAR *key, const CHAR *sub_key, UINT16 *value);
#define REG_Set_UINT16(key, value) REG_Set_UINT16_Value(key, NU_NULL, value)
#define REG_Get_UINT16(key, value) REG_Get_UINT16_Value(key, NU_NULL, value)

STATUS REG_Set_UINT32_Value(const CHAR *key, const CHAR *sub_key, UINT32 value);
STATUS REG_Get_UINT32_Value(const CHAR *key, const CHAR *sub_key, UINT32 *value);
#define REG_Set_UINT32(key, value) REG_Set_UINT32_Value(key, NU_NULL, value)
#define REG_Get_UINT32(key, value) REG_Get_UINT32_Value(key, NU_NULL, value)

STATUS REG_Set_String_Value(const CHAR *key, const CHAR *sub_key, const CHAR *value, UINT length);
STATUS REG_Get_String_Value(const CHAR *key, const CHAR *sub_key, CHAR *value, UINT length);
#define REG_Set_String(key, value, length) REG_Set_String_Value(key, NU_NULL, value, length)
#define REG_Get_String(key, value, length) REG_Get_String_Value(key, NU_NULL, value, length)

STATUS REG_Set_Bytes_Value(const CHAR *key, const CHAR *sub_key, const UNSIGNED_CHAR *value, UINT length);
STATUS REG_Get_Bytes_Value(const CHAR *key, const CHAR *sub_key, UNSIGNED_CHAR *value, UINT length);
#define REG_Set_Bytes(key, value, length) REG_Set_Bytes_Value(key, NU_NULL, value, length)
#define REG_Get_Bytes(key, value, length) REG_Get_Bytes_Value(key, NU_NULL, value, length)

STATUS REG_Set_Writable_Value(const CHAR *key, const CHAR *sub_key, BOOLEAN is_writable);
#define REG_Set_Writable(key, is_writable) REG_Set_Writable_Value(key, NU_NULL, is_writable)

#ifdef __cplusplus
}
#endif

#endif
