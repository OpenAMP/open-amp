/*
 * rpmsg based on virtio
 *
 * Copyright (C) 2018 Linaro, Inc.
 *
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RPMSG_VIRTIO_H_
#define _RPMSG_VIRTIO_H_

#include <openamp/virtio.h>
#include <metal/io.h>
#include <metal/alloc.h>
#include <metal/utilities.h>
#include <metal/time.h>
#include <metal/cache.h>
#include <metal/mutex.h>

#if defined __cplusplus
extern "C" {
#endif

/**
 * rpmsg_virtio - rpmsg virtio structure
 * @virt_dev: virtio device assocuited to the rpmsg
 * @vring_table: shared local address
 * @dst: destination address
 * rdev: rpmsg remote device
 * @ept: the rpmsg endpoint of this channel
 * @state: channel state
 */
struct rpmsg_virtio {
	struct virtio_device *virt_dev;
	struct virtqueue *rvq;
	struct virtqueue *tvq;
	struct proc_vring *vring_table;
	struct metal_list rp_channels;
	struct metal_list rp_endpoints;
	struct sh_mem_pool *mem_pool;
//	unsigned long bitmap[RPMSG_ADDR_BMP_SIZE];
//	rpmsg_chnl_cb_t channel_created;
//	rpmsg_chnl_cb_t channel_destroyed;
//	rpmsg_cb_t default_cb;
	metal_mutex_t lock;
	unsigned int role;
	unsigned int state;
	int support_ns;
};

int rpmsg_virtio_create_virtqueues(struct virtio_device *dev, int flags,
				   unsigned int nvqs, const char *names[],
				   vq_callback * callbacks[],
				   struct virtqueue *vqs_[]);

#if defined __cplusplus
}
#endif

#endif	/* _RPMSG_VIRTIO_H_ */
