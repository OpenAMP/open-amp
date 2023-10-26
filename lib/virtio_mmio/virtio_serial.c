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
#include <openamp/virtio_serial.h>

#if !defined(WITH_VIRTIO_MMIO)
#error Only VIRTIO-MMIO transport layer supported
#endif

int virtio_serial_init(struct virtio_device *vdev, struct virtqueue **vqs, char **vq_names,
			    void (**cbs)(void *), void **cb_args, int vq_count)
{
	uint32_t devid, features = 0;
	struct virtqueue *vq = NULL;
	int i;

	if (!vdev) {
		return -EINVAL;
	}

	vdev->vrings_info = metal_allocate_memory(sizeof(struct virtio_vring_info) * vq_count);
	if (!vdev->vrings_info) {
		return -ENOMEM;
	}

	devid = virtio_get_devid(vdev);
	if (devid != VIRTIO_ID_CONSOLE) {
		metal_log(METAL_LOG_ERROR, "Expected devid %04x, got %04x\n",
			  VIRTIO_ID_CONSOLE, devid);
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
				i,
				vqs[i],
				cbs[i],
				cb_args[i],
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

int virtio_serial_poll_in(const struct virtio_device *vdev, unsigned char *p_char)
{
	struct virtio_serial_chan *chan = NULL;
	struct virtqueue *vq = NULL;
	int ret = -1;
	long exp = 0;
	void *cookie = NULL;
	struct virtqueue_buf vb[1] = {0};

	if (!vdev || !vdev->priv) {
		return -EINVAL;
	}

	chan = ((struct virtio_serial_data *)(vdev->priv))->chan0;

	if (!chan) {
		return -EINVAL;
	}

	vq = vdev->vrings_info[0].vq;

	if (!atomic_compare_exchange_strong(&chan->rx_inuse, &exp, 1)) {
		return ret;
	}

	if (chan->rxpoll_active) {
		cookie = virtqueue_get_buffer(vq, NULL, NULL);

		if (!cookie) {
			/* Nothing received yet */
			atomic_store(&chan->rx_inuse, 0);
			return -1;
		}
		*p_char = chan->rxpoll;
		ret = 0;
	}

	vb[0].buf = &chan->rxpoll;
	vb[0].len = 1;

	virtqueue_add_buffer(vq, vb, 0, 1, (void *)(uintptr_t)1);
	virtqueue_kick(vq);
	chan->rxpoll_active = true;
	atomic_store(&chan->rx_inuse, 0);

	return ret;
}

void virtio_serial_poll_out(const struct virtio_device *vdev, unsigned char out_char)
{
	struct virtio_serial_chan *chan = NULL;
	struct virtqueue *vq = NULL;
	struct virtqueue_buf vb[1] = {0};
	uint8_t *data = NULL;
	long exp = 0;

	if (!vdev) {
		return;
	}

	chan = ((struct virtio_serial_data *)(vdev->priv))->chan0;

	if (!chan) {
		return;
	}

	data = &chan->txpoll;
	vq = vdev->vrings_info[1].vq;

	if (!atomic_compare_exchange_strong(&chan->tx_inuse, &exp, 1)) {
		return;
	}

	if (virtqueue_full(vq)) {
		virtqueue_get_buffer(vq, NULL, NULL);
	}

	if (virtqueue_full(vq)) {
		atomic_store(&chan->tx_inuse, 0);
		return;
	}

	data[0] = out_char;
	vb[0].buf = data;
	vb[0].len = 1;

	virtqueue_add_buffer(vq, vb, 1, 0, (void *)(uintptr_t)1);
	virtqueue_kick(vq);
	atomic_store(&chan->tx_inuse, 0);
}
