/*
 * Copyright (c) 2016 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remote
 * for use by the Linux Master */

#ifndef PLATFORM_INFO_H
#define PLATFORM_INFO_H

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>

#if defined __cplusplus
extern "C" {
#endif

#define RPMSG_CHAN_NAME         "rpmsg-openamp-demo-channel"

struct remoteproc *platform_create_proc(int proc_index, int rsc_index);

struct  rpmsg_device *
platform_create_rpmsg_vdev(struct remoteproc *rproc, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ept_create_cb new_endpoint_cb);
int platform_poll(void *priv);

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H */
