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
#include <openamp/remoteproc_virtio.h>
#include <metal/io.h>
#include <metal/alloc.h>
#include <metal/utilities.h>
#include <metal/time.h>
#include <metal/cache.h>
#include <metal/mutex.h>

#if defined __cplusplus
extern "C" {
#endif

#define RPMSG_ADDR_BMP_SIZE	4

/**
 * struct rpmsg_virtio_device - representation of a rpmsg device based on virtio
 * @vdev: pointer to the virtio device
 * @lock: mutex lock for rpmsg management
 * @rvq: pointer to receive virtqueue
 * @svq: pointer to send virtqueue
 * @buffers_number: number of shared buffers
 * @shbuf_io: pointer to the shared buffer I/O region.
 * @new_endpoint_cb: callback handler for new service announcement without local
 *                   endpoints waiting to bind.
 * @endpoints: list of endpoints.
 * @bitmap: table endpoin address allocation.
 */
struct rpmsg_virtio_device {
	struct virtio_device *vdev;
	metal_mutex_t lock;
	struct virtqueue *rvq;
	struct virtqueue *svq;
	int buffers_number;
	struct metal_io_region *shbuf_io;
	struct sh_mem_pool *shbuf;
	int (*new_endpoint_cb)(const char *name, uint32_t addr);
	struct metal_list endpoints;
	unsigned long bitmap[RPMSG_ADDR_BMP_SIZE];
};

void rpmsg_return_buffer(struct rpmsg_virtio_device *rpmsgv, void *buffer,
			 unsigned long len, unsigned short idx);
int rpmsg_virtio_enqueue_buffer(struct rpmsg_virtio_device *rpmsgv,
				void *buffer, unsigned long len,
				unsigned short idx);

static inline unsigned int rpmsg_virtio_get_role(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->role;
}

static inline void rpmsg_virtio_set_status(struct rpmsg_virtio_device *rvdev, uint8_t status)
{
	rvdev->vdev->func->set_status(rvdev->vdev, status);
}

static inline uint8_t rpmsg_virtio_get_status(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->func->get_status(rvdev->vdev);
}

static inline uint32_t rpmsg_virtio_get_features(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->func->get_features(rvdev->vdev);
}

static inline int rpmsg_virtio_create_virtqueues(struct rpmsg_virtio_device * rvdev, int flags,
				  unsigned int nvqs, const char *names[],
				  vq_callback * callbacks[],
				  struct virtqueue * vqs[])
{
	return rvdev->vdev->func->create_virtqueues(rvdev->vdev, flags, nvqs, names, callbacks, vqs);
}

#if defined __cplusplus
}
#endif

#endif	/* _RPMSG_VIRTIO_H_ */
