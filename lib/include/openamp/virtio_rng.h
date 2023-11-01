/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIRTIO_RNG_H
#define VIRTIO_RNG_H

#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/device.h>

#define VQIN_SIZE 2


/**
 * @brief Initialize a VIRTIO entropy device.
 *
 * @param[in] vdev Pointer to virtio_device structure.
 * @param[in] vqs Array of pointers to the virtqueues used by the device.
 * @param[in] vq_names Array of pointers to the virtqueues names.
 * @param[in] vq_count Number of virtqueues the device uses.
 *
 * @return int 0 for success.
 */

int virtio_rng_init(struct virtio_device *vdev, struct virtqueue **vqs,
			 char **vq_names, int vq_count);

/**
 * @brief Get random data.
 *
 * @param[in] vdev Pointer to virtio_device structure.
 * @param[out] buffer Buffer to fill with random data.
 * @param[in] length Length of buffer to fill.
 *
 * @return int 0 for success.
 */

int virtio_rng_get_entropy(const struct virtio_device *vdev, uint8_t *buffer,
				uint16_t length);

#endif /* VIRTIO_RNG_H */

