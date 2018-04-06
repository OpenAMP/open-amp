/*
 * Copyright (c) 2016 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remote
 * for use by the Linux Master */

#ifndef PLATFORM_INFO_H
#define PLATFORM_INFO_H

#include <openamp/hil.h>

#if defined __cplusplus
extern "C" {
#endif

struct hil_proc *platform_create_proc(int proc_index);

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H */
