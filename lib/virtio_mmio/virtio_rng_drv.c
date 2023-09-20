/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/open_amp.h>
#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/errno.h>

int virtio_rng_init(struct virtio_device *vdev, struct virtqueue **vqs,
		    const char **vq_names, int vq_count)
{
	uint32_t devid, features = 0;
	int i;
	int ret;

	if (!vdev || !vqs || !vq_names) {
		return -EINVAL;
	}

	vdev->vrings_info = metal_allocate_memory(sizeof(struct virtio_vring_info) * vq_count);
	if (!vdev->vrings_info) {
		return -ENOMEM;
	}

	for (i = 0; i < vq_count; i++) {
		vdev->vrings_info[i].vq = vqs[i];
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
	(void)virtio_get_features(vdev, &features);
	metal_log(METAL_LOG_DEBUG, "features: %08x\n", features);

	ret = virtio_create_virtqueues(vdev, 0, vq_count, vq_names, NULL, NULL);
	if (ret) {
		metal_free_memory(vdev->vrings_info);
		return ret;
	}

	virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);

	for (i = 0; i < vq_count; i++) {
		virtqueue_kick(vdev->vrings_info[i].vq);
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
		return -EINVAL;
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

