/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**************************************************************************
 * FILE NAME
 *
 *       env.h
 *
 * COMPONENT
 *
 *         OpenAMP stack.
 *
 * DESCRIPTION
 *
 *       This file defines abstraction layer for OpenAMP stack. The implementor
 *       must provide definition of all the functions.
 *
 * DATA STRUCTURES
 *
 *        none
 *
 * FUNCTIONS
 *
 *       env_allocate_memory
 *       env_free_memory
 *       env_map_vatopa
 *       env_map_patova
 *       env_sleep_msec
 *       env_disable_interrupts
 *       env_restore_interrupts
 *
 **************************************************************************/
#ifndef _ENV_H_
#define _ENV_H_

#include <assert.h>

#if defined __cplusplus
extern "C" {
#endif

#define openamp_assert(...) assert(__VA_ARGS__)

#if defined __cplusplus
}
#endif

#endif				/* _ENV_H_ */
