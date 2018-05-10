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

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_core.h>
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

/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS 0 /* RP supports name service notifications */

/* This feature looks only available on RPMsg on Virtio */
#define RPMSG_BUF_HELD (1U << 31) /* Flag to suggest to hold the buffer */

/**
 * struct rpmsg_virtio_device - representation of a rpmsg device based on virtio
 * @rdev: rpmsg device, first property in the struct
 * @vdev: pointer to the virtio device
 * @rvq: pointer to receive virtqueue
 * @svq: pointer to send virtqueue
 * @buffers_number: number of shared buffers
 * @shbuf_io: pointer to the shared buffer I/O region.
 * @new_endpoint_cb: callback handler for new service announcement without local
 *                   endpoints waiting to bind.
 * @endpoints: list of endpoints.
 */
struct rpmsg_endpoint;

struct rpmsg_virtio_device {
	struct rpmsg_device rdev;
	struct virtio_device *vdev;
	struct virtqueue *rvq;
	struct virtqueue *svq;
	int buffers_number;
	struct metal_io_region *shbuf_io;
	struct sh_mem_pool *shbuf;
};

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
				  vq_callback * callbacks[])
{
	return virtio_create_virtqueues(rvdev->vdev, flags, nvqs, names, callbacks);
}

int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev, struct metal_io_region *shm_io,
		    void *shm, unsigned int len);

void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev);

static inline struct rpmsg_device *
rpmsg_virtio_get_rpmsg_device(struct rpmsg_virtio_device *rvdev)
{
	return &rvdev->rdev;
}

#if defined __cplusplus
}
#endif

#endif	/* _RPMSG_VIRTIO_H_ */
