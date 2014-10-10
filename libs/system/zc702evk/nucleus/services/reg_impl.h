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
*       reg_impl.h
*
*   COMPONENT
*
*       Registry
*
*   DESCRIPTION
*
*       This file defines a collection of types used to define a new
*       registry implementation.
*
*   DATA STRUCTURES
*
*       REG_Has_Key_Func
*       REG_Key_Is_Writable_Func
*       REG_Num_Child_Keys_Func
*       REG_Set_Boolean_Func
*       REG_Get_Boolean_Func
*       REG_Set_UINT8_Func
*       REG_Get_UINT8_Func
*       REG_Set_UINT16_Func
*       REG_Get_UINT16_Func
*       REG_Set_UINT32_Func
*       REG_Get_UINT32_Func
*       REG_Set_String_Func
*       REG_Get_String_Func
*       REG_Set_Bytes_Func
*       REG_Get_Bytes_Func
*       REG_Set_Writable_Func
*       REG_IMPL
*
*	DEPENDENCIES
*
*       nucleus.h
*       reg_status.h
*
*************************************************************************/

#ifndef REG_IMPL_H
#define REG_IMPL_H

#include "nucleus.h"
#include "services/reg_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOLEAN (*REG_Has_Key_Func)(const CHAR *key);
typedef BOOLEAN (*REG_Key_Is_Writable_Func)(const CHAR *key);
typedef STATUS (*REG_Num_Child_Keys_Func)(const CHAR *key, UINT *num);
typedef STATUS (*REG_Set_Boolean_Func)(const CHAR *key, BOOLEAN value);
typedef STATUS (*REG_Get_Boolean_Func)(const CHAR *key, BOOLEAN *value);
typedef STATUS (*REG_Set_UINT8_Func)(const CHAR *key, UINT8 value);
typedef STATUS (*REG_Get_UINT8_Func)(const CHAR *key, UINT8 *value);
typedef STATUS (*REG_Set_UINT16_Func)(const CHAR *key, UINT16 value);
typedef STATUS (*REG_Get_UINT16_Func)(const CHAR *key, UINT16 *value);
typedef STATUS (*REG_Set_UINT32_Func)(const CHAR *key, UINT32 value);
typedef STATUS (*REG_Get_UINT32_Func)(const CHAR *key, UINT32 *value);
typedef STATUS (*REG_Set_String_Func)(const CHAR *key, const CHAR *value, 
                                          UINT length);
typedef STATUS (*REG_Get_String_Func)(const CHAR *key, CHAR *value, 
                                          UINT length);
typedef STATUS (*REG_Set_Bytes_Func)(const CHAR *key, 
                                         const UNSIGNED_CHAR *value,
                                         UINT length);
typedef STATUS (*REG_Get_Bytes_Func)(const CHAR *key, UNSIGNED_CHAR *value, 
                                         UINT length);
typedef STATUS (*REG_Set_Writable_Func)(const CHAR *key, 
                                            BOOLEAN is_writable);

typedef struct
{
    REG_Has_Key_Func         REG_Has_Key;
    REG_Key_Is_Writable_Func REG_Key_Is_Writable;
    REG_Num_Child_Keys_Func  REG_Num_Child_Keys_Value;
    REG_Set_Boolean_Func     REG_Set_Boolean_Value;
    REG_Get_Boolean_Func     REG_Get_Boolean_Value;
    REG_Set_UINT8_Func       REG_Set_UINT8_Value;
    REG_Get_UINT8_Func       REG_Get_UINT8_Value;
    REG_Set_UINT16_Func      REG_Set_UINT16_Value;
    REG_Get_UINT16_Func      REG_Get_UINT16_Value;
    REG_Set_UINT32_Func      REG_Set_UINT32_Value;
    REG_Get_UINT32_Func      REG_Get_UINT32_Value;
    REG_Set_String_Func      REG_Set_String_Value;
    REG_Get_String_Func      REG_Get_String_Value;
    REG_Set_Bytes_Func       REG_Set_Bytes_Value;
    REG_Get_Bytes_Func       REG_Get_Bytes_Value;
    REG_Set_Writable_Func    REG_Set_Writable_Value;
} REG_IMPL;

extern REG_IMPL *impl;

#ifdef __cplusplus
}
#endif

#endif
