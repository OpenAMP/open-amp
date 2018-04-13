/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg_virtio.h>
#include <string.h>
#include "openamp/rpmsg.h"
#include "openamp/remoteproc.h"
#include "openamp/hil.h"
#include "metal/utilities.h"
#include "metal/alloc.h"
#include "metal/atomic.h"
#include "metal/cpu.h"
