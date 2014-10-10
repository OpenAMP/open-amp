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
*       reg_impl_mem_node.h
*
*   COMPONENT
*
*       Registry
*
*   DESCRIPTION
*
*       This file defines the data structure used to build an in-memory
*       registry.
*
*   DATA STRUCTURES
*
*       REG_Memory_Node
*
*	DEPENDENCIES
*
*       nucleus.h
*
*************************************************************************/

#ifndef REG_IMPL_MEM_NODE_H
#define REG_IMPL_MEM_NODE_H

#include "nucleus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct REG_Memory_Node_Struct
{
    const CHAR *key;
    union
    {
        UNSIGNED_CHAR *bytes;
        CHAR *str;
        UINT32 u32;
        UINT16 u16;
        UINT8 u8;
        BOOLEAN b;
    } value;
    const struct REG_Memory_Node_Struct *children;
} REG_Memory_Node;

extern REG_Memory_Node *root;

#ifdef __cplusplus
}
#endif

#endif
