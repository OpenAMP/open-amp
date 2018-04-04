/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>

#ifndef RPMSG_NUM_VRINGS
#define RPMSG_NUM_VRINGS (2)
#endif


static unsigned int rpmsg_features = VIRTIO_RPMSG_F_NS;

/**
 * rpmsg_rdev_get_endpoint_from_addr
 *
 * This function returns endpoint node based on src address. It must be called
 * with mutex locked.
 *
 * @param rdev - pointer remote device control block
 * @param addr - src address
 *
 * @return - rpmsg endpoint
 *
 */
struct rpmsg_endpoint *rpmsg_get_endpoint_from_addr(struct rpmsg_virtio *rvdev,
						uint32_t addr)
{
	struct rpmsg_endpoint *rp_ept;
	struct metal_list *node;

	metal_list_for_each(&rvdev->rp_endpoints, node) {
		rp_ept = metal_container_of(node,
				struct rpmsg_endpoint, node);
		if (rp_ept->src == addr)
			return rp_ept;
	}

	return RPMSG_NULL;
}

/**
 * rpmsg_get_tx_buffer
 *
 * Provides buffer to transmit messages.
 *
 * @param rvdev - pointer to rpmsg device
 * @param len  - length of returned buffer
 * @param idx  - buffer index
 *
 * return - pointer to buffer.
 */
void *rpmsg_get_tx_buffer(struct rpmsg_virtio *rvdev, unsigned long *len,
			  unsigned short *idx)
{
	void *data;

	if (rvdev->role == RPMSG_REMOTE) {
		data = virtqueue_get_buffer(rvdev->tvq, (uint32_t *) len, idx);
		if (data == RPMSG_NULL) {
			data = sh_mem_get_buffer(rvdev->mem_pool);
			*len = RPMSG_BUFFER_SIZE;
		}
	} else {
		data =
		    virtqueue_get_available_buffer(rvdev->tvq, idx,
						   (uint32_t *) len);
	}
	return data;
}
/**
 * rpmsg_get_rx_buffer
 *
 * Retrieves the received buffer from the virtqueue.
 *
 * @param rvdev - pointer to rpmsg device
 * @param len  - size of received buffer
 * @param idx  - index of buffer
 *
 * @return - pointer to received buffer
 *
 */
void *rpmsg_get_rx_buffer(struct rpmsg_virtio *rvdev, unsigned long *len,
			  unsigned short *idx)
{

	void *data;

	if (rvdev->role == RPMSG_REMOTE) {
		data = virtqueue_get_buffer(rvdev->rvq, (uint32_t *) len, idx);
	} else {
		data =
		    virtqueue_get_available_buffer(rvdev->rvq, idx,
						   (uint32_t *) len);
	}
	if (data) {
		/* FIX ME: library should not worry about if it needs
		 * to flush/invalidate cache, it is shared memory.
		 * The shared memory should be mapped properly before
		 * using it.
		 */
		metal_cache_invalidate(data, (unsigned int)(*len));
	}

	return data;
}

/**
 * rpmsg_return_buffer
 *
 * Places the used buffer back on the virtqueue.
 *
 * @param rvdev   - pointer to remote core
 * @param buffer - buffer pointer
 * @param len    - buffer length
 * @param idx    - buffer index
 *
 */
void rpmsg_return_buffer(struct rpmsg_virtio *rvdev, void *buffer,
			 unsigned long len, unsigned short idx)
{
	struct metal_sg sg;

	if (rvdev->role == RPMSG_REMOTE) {
		/* Initialize buffer node */
		sg.virt = buffer;
		sg.len = len;
		sg.io = rvdev->virt_dev->sh_buff.io;
		virtqueue_add_buffer(rvdev->rvq, &sg, 0, 1, buffer);
	} else {
		(void)sg;
		virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
	}
}

/**
 * rpmsg_tx_callback
 *
 * Tx callback function.
 *
 * @param vq - pointer to virtqueue on which Tx is has been
 *             completed.
 *
 */
static void rpmsg_tx_callback(struct virtqueue *vq)
{
	struct virtio_device *vdev = vq->vq_dev;
	struct rpmsg_virtio *rpmsg_dev = vdev->client_dev;
	struct rpmsg_channel *rp_chnl;
	struct metal_list *node;

	/* Check if the remote device is master. */
	if (rpmsg_dev->role == RPMSG_MASTER) {

		/*
		 * Notification is received from the master. Now the remote(us)
		 * can performs one of two operations;
		 *
		 * a. If name service announcement is supported then it will
		 *    send NS message.
		 * else
		 * b. It will update the channel state to active so that further
		 *    communication can take place.
		 */
		metal_list_for_each(&rpmsg_dev->rp_channels, node) {
			rp_chnl = metal_container_of(node,
				struct rpmsg_channel, node);

			if (rp_chnl->state == RPMSG_CHNL_STATE_IDLE) {
				if (rpmsg_dev->support_ns) {
#if 0
					if (rpmsg_send_ns_message(rpmsg_dev,
						rp_chnl, RPMSG_NS_CREATE) ==
						RPMSG_SUCCESS)
						rp_chnl->state =
							RPMSG_CHNL_STATE_NS;
#endif
				} else {
					rp_chnl->state =
					    RPMSG_CHNL_STATE_ACTIVE;
				}

			}

		}
	}
}

/**
 * rpmsg_rx_callback
 *
 * Rx callback function.
 *
 * @param vq - pointer to virtqueue on which messages is received
 *
 */
void rpmsg_rx_callback(struct virtqueue *vq)
{
	struct virtio_device *vdev = vq->vq_dev;
	struct rpmsg_virtio *rpmsg_dev = vdev->client_dev;
	struct rpmsg_channel *rp_chnl;
	struct rpmsg_endpoint *rp_ept;
	struct rpmsg_hdr *rp_hdr;
//	struct rpmsg_hdr_reserved *reserved;
	unsigned long len;
	unsigned short idx;

	metal_mutex_acquire(&rpmsg_dev->lock);

	/* Process the received data from remote node */
	rp_hdr = (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rpmsg_dev, &len, &idx);

	metal_mutex_release(&rpmsg_dev->lock);

	while (rp_hdr) {
		/* Get the channel node from the remote device channels list. */
		metal_mutex_acquire(&rpmsg_dev->lock);
		rp_ept = rpmsg_get_endpoint_from_addr(rpmsg_dev, rp_hdr->dst);
		metal_mutex_release(&rpmsg_dev->lock);

		if (!rp_ept)
			/* Fatal error no endpoint for the given dst addr. */
			return;

		rp_chnl = rp_ept->rp_chnl;

		if ((rp_chnl) && (rp_chnl->state == RPMSG_CHNL_STATE_NS)) {
			/*
			 * First message from RPMSG Master, update channel
			 * destination address and state
			 */
			rp_ept->dst = rp_ept->src;
			rp_chnl->state = RPMSG_CHNL_STATE_ACTIVE;

#if 0
			/* Notify channel creation to application */
			if (rpmsg_dev->channel_created)
				rpmsg_dev->channel_created(rp_chnl);
#endif
		} else {
			rp_ept->cb(rp_ept, (void *)RPMSG_LOCATE_DATA(rp_hdr),
				   rp_hdr->len, rp_ept->src, rp_ept->priv);
		}

		metal_mutex_acquire(&rpmsg_dev->lock);

		/* Check whether callback wants to hold buffer */
		if (rp_hdr->reserved & RPMSG_BUF_HELD) {
#if 0
			/*
			 * 'rp_hdr->reserved' field is now used as storage for
			 * 'idx' to release buffer later
			 */
			reserved =
				(struct rpmsg_hdr_reserved *)&rp_hdr->reserved;
			reserved->idx = (uint16_t)idx;
#endif
		} else {
			/* Return used buffers. */
			rpmsg_return_buffer(rpmsg_dev, rp_hdr, len, idx);
		}

		rp_hdr =
		    (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rpmsg_dev, &len,
							    &idx);
		metal_mutex_release(&rpmsg_dev->lock);
	}
}

/**
 * check if the remote is ready to start RPMsg communication
 */
static int rpmsg_wait_remote_ready(struct rpmsg_virtio *rpmsg_vdev)
{
	struct virtio_device *vdev = rpmsg_vdev->virt_dev;
	uint8_t status;

	while (1) {
		status = vdev->func->get_status(vdev);
		/* Busy wait until the remote is ready */
		if (status & VIRTIO_CONFIG_STATUS_NEEDS_RESET) {
			vdev->func->set_status(vdev, 0);
			/* TODO notify remote processor */
		} else if (status & VIRTIO_CONFIG_STATUS_DRIVER_OK) {
			return true;
		}
		/* TODO: clarify metal_cpu_yield usage*/
		metal_cpu_yield();
	}

	return false;
}

/**
 * This function sends rpmsg "message" to remote device.
 *
 * @param rp_chnl - pointer to rpmsg channel
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

int rpmsg_send_offchannel_raw(struct rpmsg_channel *rp_chnl, uint32_t src,
			      uint32_t dst, const void *data,
			      int size, int wait)
{
	(void)rp_chnl;
	(void)src;
	(void)dst;
	(void)data;
	(void)size;
	(void)wait;

	return RPMSG_SUCCESS;
}

/**
 * rpmsg_vdev_init: ropmsg initialisation
 * Master side:
 * Initialize RPMsg virtio queues and shared buffers, the address of shm can be
 * ANY. In this case, function will get shared memory from system shared memory
 * pools. If the vdev has RPMsg name service feature, this API will create an
 * name service endpoint.
 *
 * Slave side:
 * This API will not return until the driver ready is set by the master side.
 *
 * @param rpmsg_vdev   - pointer to the rpmsg device
 * @param vdev   - pointer to the virtio device
 * @param shm    - pointer to the share memory. Can be NULL for slave.
 * @param len    - length of the shared memory section.
 *
 * @return - status of function execution
 */

int rpmsg_vdev_init(struct rpmsg_virtio *rpmsg_vdev, struct virtio_device *vdev,
		    void *shm, int len)
{
	struct sh_mem_pool *mem_pool = shm;
	const char *vq_names[RPMSG_NUM_VRINGS];
	void (*callback[RPMSG_NUM_VRINGS]) (struct virtqueue *vq);
	int status;

	metal_mutex_init(&rpmsg_vdev->lock);

	vdev->client_dev = rpmsg_vdev;
	vdev->func = &rpmsg_virtio_config_ops;

	vdev->func->set_features(vdev, rpmsg_features);

	/* Initialize names and callbacks based on the device role */
	if (rpmsg_vdev->role == RPMSG_MASTER) {
		if (!shm || !len)
			return -RPMSG_ERR_NO_MEM;
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

	/* Create virtqueues for remote device */
	status = vdev->func->create_virtqueues(vdev, 0, RPMSG_NUM_VRINGS,
					       vq_names, callback, RPMSG_NULL);
	if (status != RPMSG_SUCCESS)
		return status;
	if (rpmsg_vdev->role == RPMSG_MASTER) {

		rpmsg_vdev->mem_pool = mem_pool;
		vdev->func->set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);
	} else {
		/* wait synchro with the master */
		rpmsg_wait_remote_ready(rpmsg_vdev);
	}
	rpmsg_vdev->virt_dev = vdev;

	/* Initialize channels and endpoints list */
	metal_list_init(&rpmsg_vdev->rp_endpoints);
	metal_list_init(&rpmsg_vdev->rp_channels);

	/*TODO : create name servioce endpoints */
	return RPMSG_SUCCESS;
}
