/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_core.h>
#include <metal/sleep.h>

#ifndef RPMSG_NUM_VRINGS
#define RPMSG_NUM_VRINGS (2)
#endif

#define RPMSG_ADDR_BMP_SIZE     4

/* Total tick count for 15secs - 1msec tick. */
#define RPMSG_TICK_COUNT                        15000

/* Time to wait - In multiple of 10 msecs. */
#define RPMSG_TICKS_PER_INTERVAL                10

/**
 * rpmsg_get_buffer_size
 *
 * Returns buffer size available for sending messages.
 *
 * @param channel - pointer to rpmsg channel
 *
 * @return - buffer size
 *
 */
static int rpmsg_get_buffer_size(struct rpmsg_virtio_device *rvdev)
{
	int length;

	metal_mutex_acquire(&rvdev->lock);

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE) {
		/*
		 * If device role is Remote then buffers are provided by us
		 * (RPMSG Master), so just provide the macro.
		 */
		length = RPMSG_BUFFER_SIZE - sizeof(struct rpmsg_hdr);
	} else {
		/*
		 * If other core is Master then buffers are provided by it,
		 * so get the buffer size from the virtqueue.
		 */
		length =
		    (int)virtqueue_get_desc_size(rvdev->svq) -
		    sizeof(struct rpmsg_hdr);
	}

	metal_mutex_release(&rvdev->lock);

	return length;
}

/**
 * This function sends rpmsg "message" to remote device.
 *
 * @param ept     - pointer to end point
 * @param src     - source address of channel
 * @param dst     - destination address of channel
 * @param data    - data to transmit
 * @param size    - size of data
 * @param wait    - boolean, wait or not for buffer to become
 *                  available
 *
 * @return - size of data sent or negative value for failure.
 *
 */

int rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data,
			      int size, int wait)
{
	struct rpmsg_virtio_device *rvdev;
	struct rpmsg_hdr rp_hdr;
	void *buffer;
	unsigned short idx;
	int ret, tick_count = 0;
	unsigned long buff_len;
	uint8_t status;
	struct metal_io_region *io;

	if (!ept || !data)
		return RPMSG_ERR_PARAM;

	/* Get the associated remote device for channel. */
	rvdev = ept->rvdev;

	status = rpmsg_virtio_get_status(rvdev);
	/* Validate device state */
	if (ept->dest_addr == RPMSG_ADDR_ANY ||
	    status != VIRTIO_CONFIG_STATUS_DRIVER_OK) {
		return RPMSG_ERR_DEV_STATE;
	}

	if (size > (rpmsg_get_buffer_size(rvdev)))
		return RPMSG_ERR_BUFF_SIZE;

	/* Lock the device to enable exclusive access to virtqueues */
	metal_mutex_acquire(&rvdev->lock);
	/* Get rpmsg buffer for sending message. */
	buffer = rpmsg_get_tx_buffer(rvdev, &buff_len, &idx);
	/* Unlock the device */
	metal_mutex_release(&rvdev->lock);

	if (!buffer && !wait)
		return RPMSG_ERR_NO_BUFF;

	while (!buffer) {
		/*
		 * Wait parameter is true - pool the buffer for
		 * 15 secs as defined by the APIs.
		 */
		metal_sleep_usec(RPMSG_TICKS_PER_INTERVAL);
		metal_mutex_acquire(&rvdev->lock);
		buffer = rpmsg_get_tx_buffer(rvdev, &buff_len, &idx);
		metal_mutex_release(&rvdev->lock);
		tick_count += RPMSG_TICKS_PER_INTERVAL;
		if (!buffer && (tick_count >=
		    (RPMSG_TICK_COUNT / RPMSG_TICKS_PER_INTERVAL))) {
			return RPMSG_ERR_NO_BUFF;
		}
	}

	/* Initialize RPMSG header. */
	rp_hdr.dst = dst;
	rp_hdr.src = src;
	rp_hdr.len = size;
	rp_hdr.reserved = 0;

	/* Copy data to rpmsg buffer. */
	io = rvdev->shbuf_io;
	metal_io_block_write(io,
			metal_io_virt_to_offset(io, buffer),
			&rp_hdr, sizeof(rp_hdr));
	metal_io_block_write(io,
			metal_io_virt_to_offset(io, RPMSG_LOCATE_DATA(buffer)),
			data, size);
	metal_mutex_acquire(&rvdev->lock);

	/* Enqueue buffer on virtqueue. */
	ret = rpmsg_virtio_enqueue_buffer(rvdev, buffer, buff_len, idx);
	RPMSG_ASSERT(ret == VQUEUE_SUCCESS, "failed to enqueue buffer\n");
	/* Let the other side know that there is a job to process. */
	virtqueue_kick(rvdev->svq);

	metal_mutex_release(&rvdev->lock);

	return size;
}

int rpmsg_create_ept(struct rpmsg_virtio_device *rvdev,
		     struct rpmsg_endpoint *ept)
{
	int status;

	if (!ept->cb)
		return RPMSG_ERR_PARAM;

	status = rpmsg_register_endpoint(rvdev, ept);
	if (status < 0)
		return status;

	if (ept->dest_addr == RPMSG_ADDR_ANY)
		/* Send NS announcement to remote processor */
		status = rpmsg_send_ns_message(rvdev, ept, RPMSG_NS_CREATE);

	return status;
}

/**
 * rpmsg_destroy_ept
 *
 * This function deletes rpmsg endpoint and performs cleanup.
 *
 * @param ept - pointer to endpoint to destroy
 *
 */
void rpmsg_destroy_ept(struct rpmsg_endpoint *ept)
{
	struct rpmsg_virtio_device *rvdev;

	if (!ept)
		return;

	rvdev = ept->rvdev;

	metal_mutex_acquire(&rvdev->lock);
	rpmsg_release_address(rvdev->bitmap, RPMSG_ADDR_BMP_SIZE,
			      ept->addr);
	metal_list_del(&ept->node);
	metal_mutex_release(&rvdev->lock);
}

/**
 * rpmsg_init_vdev: ropmsg initialisation
 * Master side:
 * Initialize RPMsg virtio queues and shared buffers, the address of shm can be
 * ANY. In this case, function will get shared memory from system shared memory
 * pools. If the vdev has RPMsg name service feature, this API will create an
 * name service endpoint.
 *
 * Slave side:
 * This API will not return until the driver ready is set by the master side.
 *
 * @param rvdev   - pointer to the rpmsg device
 * @param vdev   - pointer to the virtio device
 * @param shm    - pointer to the share memory.
 * @param len    - length of the shared memory section.
 *
 * @return - status of function execution
 */

int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev, void *shm, int len)
{
	struct remoteproc_vshm_pool *rp_shm = shm;
	const char *vq_names[RPMSG_NUM_VRINGS];
	void (*callback[RPMSG_NUM_VRINGS]) (struct virtqueue *vq);
	unsigned long dev_features;
	static struct rpmsg_endpoint ns_ept;
	int status;

	metal_mutex_init(&rvdev->lock);

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/*
		 * Since device is RPMSG Remote so we need to manage the
		 * shared buffers. Create shared memory pool to handle buffers.
		 */
		if (!shm || !len)
			return -RPMSG_ERR_NO_MEM;

		rvdev->shbuf =
		    sh_mem_create_pool(rp_shm->va, rp_shm->size,
				       RPMSG_BUFFER_SIZE);

		if (!rvdev->shbuf)
			return RPMSG_ERR_NO_MEM;

		vq_names[0] = "tx_vq";
		vq_names[1] = "rx_vq";
		callback[0] = rpmsg_tx_callback;
		callback[1] = rpmsg_rx_callback;
	} else {
		vq_names[0] = "rx_vq";
		vq_names[1] = "tx_vq";
		callback[0] = rpmsg_rx_callback;
		callback[1] = rpmsg_tx_callback;
	}
	rvdev->shbuf_io = rp_shm->io;

	/* Create virtqueues for remote device */
	status = rpmsg_virtio_create_virtqueues(rvdev, 0, RPMSG_NUM_VRINGS,
					       vq_names, callback, RPMSG_NULL);
	if (status != RPMSG_SUCCESS)
		return status;
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		rpmsg_virtio_set_status(rvdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);
	} else {
		/* wait synchro with the master */
		rpmsg_wait_remote_ready(rvdev);
	}
	rvdev->vdev = vdev;
	vdev->priv = rvdev;

	/* Initialize channels and endpoints list */
	metal_list_init(&rvdev->endpoints);

	dev_features = rpmsg_virtio_get_features(rvdev);

	/*
	 * Create name service announcement endpoint if device supports name
	 * service announcement feature.
	 */
	if ((dev_features & (1 << VIRTIO_RPMSG_F_NS))) {
		ns_ept.addr = RPMSG_NS_EPT_ADDR;
		ns_ept.cb = rpmsg_ns_callback;
		strncpy(ns_ept.name, "NS", sizeof("NS"));
		status = rpmsg_register_endpoint(rvdev, &ns_ept);
	}

	return status;
}

void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev)
{
	struct metal_list *node;
	struct rpmsg_endpoint *ept;

	metal_mutex_acquire(&rvdev->lock);
	while (!metal_list_is_empty(&rvdev->endpoints)) {
		node = rvdev->endpoints.next;
		ept = metal_container_of(node, struct rpmsg_endpoint, node);

		rpmsg_destroy_ept(ept);
	}

	rvdev->rvq = 0;
	rvdev->svq = 0;
	if (rvdev->shbuf) {
		sh_mem_delete_pool(rvdev->shbuf);
		rvdev->shbuf = NULL;
	}
	metal_mutex_release(&rvdev->lock);

	metal_mutex_deinit(&rvdev->lock);
}

