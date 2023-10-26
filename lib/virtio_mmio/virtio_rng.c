/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#include <openamp/open_amp.h>
#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <openamp/virtio_mmio.h>
#include <metal/device.h>

#if !defined(WITH_VIRTIO_MMIO)
#error Only VIRTIO-MMIO transport layer supported
#endif

int virtio_rng_init(struct virtio_device *vdev, struct virtqueue **vqs,
			 char **vq_names, int vq_count)
{
	uint32_t devid, features = 0;
	struct virtqueue *vq = NULL;
	int i;

	if (!vdev || !vqs || !vq_names) {
		return -EINVAL;
	}

	vdev->vrings_info = metal_allocate_memory(sizeof(struct virtio_vring_info) * vq_count);
	if (!vdev->vrings_info) {
		return -ENOMEM;
	}

	devid = virtio_get_devid(vdev);
	if (devid != VIRTIO_ID_ENTROPY) {
		metal_log(METAL_LOG_ERROR, "Expected devid %04x, got %04x\n",
			  VIRTIO_ID_ENTROPY, devid);
		metal_free_memory(vdev->vrings_info);
		return -ENODEV;
	}

	virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER);
	virtio_set_features(vdev, 0/*VIRTIO_F_NOTIFY_ON_EMPTY*/);
	virtio_get_features(vdev, &features);
	metal_log(METAL_LOG_DEBUG, "features: %08x\n", features);

	for (i = 0; i < vq_count; i++) {
		/* TODO: update API for compatibility with other transports like
		 * remoteproc virtio
		 */
		vq = virtio_mmio_setup_virtqueue(
				vdev,
				0,
				vqs[i],
				NULL,
				NULL,
				vq_names[i]
				);
		if (!vq) {
			return -1;
		}
	}

	virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);

	for (i = 0; i < vq_count; i++) {
		virtqueue_kick(vq);
	}

	return 0;
}

int virtio_rng_get_entropy(const struct virtio_device *vdev, uint8_t *buffer,
				uint16_t length)
{
	struct virtqueue_buf vb[1] = {{.buf = buffer, .len = length} };
	struct virtqueue *vq = vdev->vrings_info[0].vq;
	void *cookie;

	if (!vdev) {
		return -1;
	}

	if (virtqueue_add_buffer(vq, vb, 0, 1, (void *)buffer)) {
		return -EIO;
	}
	virtqueue_kick(vq);

	do {
		cookie = virtqueue_get_buffer(vq, NULL, NULL);
	} while (!cookie);

	VIRTIO_ASSERT(cookie == buffer, "Got the wrong cookie");

	return 0;
}

