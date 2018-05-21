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

#include <metal/io.h>
#include <metal/mutex.h>
#include <openamp/rpmsg.h>
#include <openamp/virtio.h>

#if defined __cplusplus
extern "C" {
#endif

/* Configurable parameters */
#ifndef RPMSG_BUFFER_SIZE
#define RPMSG_BUFFER_SIZE	(512)
#endif

/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS 0 /* RP supports name service notifications */

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
struct rpmsg_virtio_device {
	struct rpmsg_device rdev;
	struct virtio_device *vdev;
	struct virtqueue *rvq;
	struct virtqueue *svq;
	struct metal_io_region *shbuf_io;
	struct sh_mem_pool *shbuf;
};

#define RPMSG_REMOTE	VIRTIO_DEV_SLAVE
#define RPMSG_MASTER	VIRTIO_DEV_MASTER
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

/**
 * rpmsg_virtio_get_buffer_size - get rpmsg virtio buffer size
 *
 * @rdev - pointer to the rpmsg device
 *
 * @return - next available buffer size for text, negative value for failure
 */
int rpmsg_virtio_get_buffer_size(struct rpmsg_device *rdev);

int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev,
		    void (*new_endpoint_cb)(const char *name, uint32_t src),
		    struct metal_io_region *shm_io,
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
