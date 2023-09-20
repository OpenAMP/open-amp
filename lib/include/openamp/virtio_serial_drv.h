/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIRTIO_SERIAL_H
#define VIRTIO_SERIAL_H

#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/device.h>

#define VQIN_SIZE    4
#define VQOUT_SIZE   4

struct virtio_serial_chan {
	atomic_long tx_inuse;
	atomic_long rx_inuse;
	bool rxpoll_active;
	bool txpoll_active;
	uint8_t rxpoll;
	uint8_t txpoll;
};

struct virtio_serial_data {
	struct virtio_serial_chan *chan0;
};

typedef void (*con_funcptr)(void *);

/**
 * @brief Initialize a VIRTIO console device.
 *
 * @param vdev		Pointer to virtio_device structure.
 * @param vqs		Array of pointers to the virtqueues used by the device.
 * @param vq_names	Array of pointers to the virtqueues names.
 * @param cbs		Array of function pointers to call on virtqueue kick.
 * @param cb_args	Array of pointers to parameters for kick callbacks.
 * @param vq_count	Number of virtqueues the device uses.
 *
 * @return int 0 for success.
 */
int virtio_serial_init(struct virtio_device *vdev, struct virtqueue **vqs, char **vq_names,
		       con_funcptr *cbs, void **cb_args, int vq_count);

/**
 * @brief Poll console input queue for data.
 *
 * @param vdev		Pointer to virtio_device structure.
 * @param p_char	Output char.
 *
 * @return int 0 for success.
 */

int virtio_serial_poll_in(const struct virtio_device *vdev, unsigned char *p_char);

/**
 * @brief Send data routine.
 *
 * @param vdev		Pointer to virtio_device structure.
 * @param out_char	Character to send.
 *
 * @return int 0 for success.
 */
void virtio_serial_poll_out(const struct virtio_device *vdev, unsigned char out_char);

#endif /* VIRTIO_SERIAL_H */

