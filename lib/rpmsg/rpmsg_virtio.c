/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg_virtio.h>
#include <openamp/remoteproc.h>
#include <metal/utilities.h>
#include <metal/alloc.h>
#include <metal/atomic.h>
#include <metal/cpu.h>
#include <metal/sleep.h>
#include <string.h>

#ifndef RPMSG_NUM_VRINGS
#define RPMSG_NUM_VRINGS (2)
#endif

/* Total tick count for 15secs - 1msec tick. */
#define RPMSG_TICK_COUNT                        15000

/* Time to wait - In multiple of 10 msecs. */
#define RPMSG_TICKS_PER_INTERVAL                10

/**
 * rpmsg_virtio_return_buffer
 *
 * Places the used buffer back on the virtqueue.
 *
 * @param rvdev   - pointer to remote core
 * @param buffer - buffer pointer
 * @param len    - buffer length
 * @param idx    - buffer index
 *
 */
void rpmsg_virtio_return_buffer(struct rpmsg_virtio_device *rvdev,
				void *buffer,
				unsigned long len, unsigned short idx)
{
	struct virtqueue_buf vqbuf;
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1, buffer);
	} else {
		virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
	}
}

/**
 * rpmsg_virtio_enqueue_buffers
 *
 * Places buffer on the virtqueue for consumption by the other side.
 *
 * @param rvdev   - pointer to rpmsg virtio
 * @param buffer - buffer pointer
 * @param len    - buffer length
 * @idx          - buffer index
 *
 * @return - status of function execution
 */
int rpmsg_virtio_enqueue_buffer(struct rpmsg_virtio_device *rvdev, void *buffer,
				unsigned long len, unsigned short idx)
{
	int status;
	struct virtqueue_buf vqbuf;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		status = virtqueue_add_buffer(rvdev->svq, &vqbuf, 0, 1, buffer);
	} else {
		status = virtqueue_add_consumed_buffer(rvdev->svq, idx, len);
	}

	return status;
}

/**
 * rpmsg_virtio_get_tx_buffer
 *
 * Provides buffer to transmit messages.
 *
 * @param rvdev - pointer to rpmsg device
 * @param len  - length of returned buffer
 * @param idx  - buffer index
 *
 * return - pointer to buffer.
 */
void *rpmsg_virtio_get_tx_buffer(struct rpmsg_virtio_device *rvdev,
				 unsigned long *len, unsigned short *idx)
{
	void *data;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		data = virtqueue_get_buffer(rvdev->svq, (uint32_t *)len, idx);
		if (data == RPMSG_NULL) {
			data = sh_mem_get_buffer(rvdev->shbuf);
			*len = RPMSG_BUFFER_SIZE;
		}
	} else {
		data =
		    virtqueue_get_available_buffer(rvdev->svq, idx,
						   (uint32_t *)len);
	}
	return data;
}

/**
 * rpmsg_virtio_get_rx_buffer
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
void *rpmsg_virtio_get_rx_buffer(struct rpmsg_virtio_device *rvdev,
				 unsigned long *len,
				 unsigned short *idx)
{
	void *data;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		data = virtqueue_get_buffer(rvdev->rvq, (uint32_t *)len, idx);
	} else {
		data =
		    virtqueue_get_available_buffer(rvdev->rvq, idx,
						   (uint32_t *)len);
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
 * check if the remote is ready to start RPMsg communication
 */
int rpmsg_virtio_wait_remote_ready(struct rpmsg_virtio_device *rvdev)
{
	uint8_t status;

	while (1) {
		status = rpmsg_virtio_get_status(rvdev);
		/* Busy wait until the remote is ready */
		if (status & VIRTIO_CONFIG_STATUS_NEEDS_RESET) {
			rpmsg_virtio_set_status(rvdev, 0);
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
 * rpmsg_virtio_get_buffer_size
 *
 * Returns buffer size available for sending messages.
 *
 * @param channel - pointer to rpmsg channel
 *
 * @return - buffer size
 *
 */
static int rpmsg_virtio_get_buffer_size(struct rpmsg_virtio_device *rvdev)
{
	int length;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
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

	return length;
}

/**
 * This function sends rpmsg "message" to remote device.
 *
 * @param rdev    - pointer to rpmsg device
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
int rpmsg_virtio_send_offchannel_raw(struct rpmsg_device *rdev, uint32_t src,
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

	if (!rdev || !data)
		return RPMSG_ERR_PARAM;

	/* Get the associated remote device for channel. */
	rvdev = (struct rpmsg_virtio_device *)rdev;

	status = rpmsg_virtio_get_status(rvdev);
	/* Validate device state */
	if (dst == RPMSG_ADDR_ANY ||
	    !(status & VIRTIO_CONFIG_STATUS_DRIVER_OK)) {
		return RPMSG_ERR_DEV_STATE;
	}

	/* Lock the device to enable exclusive access to virtqueues */
	metal_mutex_acquire(&rdev->lock);
	if (size > (rpmsg_virtio_get_buffer_size(rvdev))) {
		metal_mutex_release(&rdev->lock);
		return RPMSG_ERR_BUFF_SIZE;
	}

	/* Get rpmsg buffer for sending message. */
	buffer = rpmsg_virtio_get_tx_buffer(rvdev, &buff_len, &idx);
	/* Unlock the device */
	metal_mutex_release(&rdev->lock);

	if (!buffer && !wait) {
		return RPMSG_ERR_NO_BUFF;
	}

	while (!buffer) {
		/*
		 * Wait parameter is true - pool the buffer for
		 * 15 secs as defined by the APIs.
		 */
		metal_sleep_usec(RPMSG_TICKS_PER_INTERVAL);
		metal_mutex_acquire(&rdev->lock);
		buffer = rpmsg_virtio_get_tx_buffer(rvdev, &buff_len, &idx);
		metal_mutex_release(&rdev->lock);
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
	metal_mutex_acquire(&rdev->lock);

	/* Enqueue buffer on virtqueue. */
	ret = rpmsg_virtio_enqueue_buffer(rvdev, buffer, buff_len, idx);
	RPMSG_ASSERT(ret == VQUEUE_SUCCESS, "failed to enqueue buffer\n");
	/* Let the other side know that there is a job to process. */
	virtqueue_kick(rvdev->svq);

	metal_mutex_release(&rdev->lock);

	return size;
}

/**
 * rpmsg_virtio_tx_callback
 *
 * Tx callback function.
 *
 * @param vq - pointer to virtqueue on which Tx is has been
 *             completed.
 *
 */
void rpmsg_virtio_tx_callback(struct virtqueue *vq)
{
	(void)vq;
}

/**
 * rpmsg_virtio_rx_callback
 *
 * Rx callback function.
 *
 * @param vq - pointer to virtqueue on which messages is received
 *
 */
void rpmsg_virtio_rx_callback(struct virtqueue *vq)
{
	struct virtio_device *vdev = vq->vq_dev;
	struct rpmsg_virtio_device *rvdev = vdev->priv;
	struct rpmsg_device *rdev = &rvdev->rdev;
	struct rpmsg_endpoint *ept;
	struct rpmsg_hdr *rp_hdr;
	unsigned long len;
	unsigned short idx;

	metal_mutex_acquire(&rdev->lock);

	/* Process the received data from remote node */
	rp_hdr = (struct rpmsg_hdr *)rpmsg_virtio_get_rx_buffer(rvdev,
								&len, &idx);

	metal_mutex_release(&rdev->lock);

	while (rp_hdr) {
		/* Get the channel node from the remote device channels list. */
		metal_mutex_acquire(&rdev->lock);
		ept = rpmsg_get_ept_from_addr(rdev, rp_hdr->dst);
		metal_mutex_release(&rdev->lock);

		if (!ept)
			/* Fatal error no endpoint for the given dst addr. */
			return;

		if (ept && ept->dest_addr == RPMSG_ADDR_ANY &&
		    ept->addr != RPMSG_NS_EPT_ADDR) {
			/*
			 * First message received from the remote side,
			 * update channel destination address
			 */
			ept->dest_addr = rp_hdr->src;
		}
		ept->cb(ept, (void *)RPMSG_LOCATE_DATA(rp_hdr),
				   rp_hdr->len, ept->addr, ept->priv);

		metal_mutex_acquire(&rdev->lock);

#if 0
		/* Check whether callback wants to hold buffer */
		if (rp_hdr->reserved & RPMSG_BUF_HELD) {
			/*
			 * 'rp_hdr->reserved' field is now used as storage for
			 * 'idx' to release buffer later
			 */
			reserved =
				(struct rpmsg_hdr_reserved *)&rp_hdr->reserved;
			reserved->idx = (uint16_t)idx;

		} else {
			/* Return used buffers. */
			rpmsg_return_buffer(rvdev, rp_hdr, len, idx);
		}
#else
		/* Return used buffers. */
		rpmsg_virtio_return_buffer(rvdev, rp_hdr, len, idx);
#endif

		rp_hdr = (struct rpmsg_hdr *)
			 rpmsg_virtio_get_rx_buffer(rvdev, &len, &idx);
		metal_mutex_release(&rdev->lock);
	}
}

/**
 * rpmsg_virtio_ns_callback
 *
 * This callback handles name service announcement from the remote device
 * and creates/deletes rpmsg channels.
 *
 * @param server_chnl - pointer to server channel control block.
 * @param data        - pointer to received messages
 * @param len         - length of received data
 * @param priv        - any private data
 * @param src         - source address
 *
 * @return - none
 */
static void rpmsg_virtio_ns_callback(struct rpmsg_endpoint *ept, void *data,
				     size_t len, uint32_t src, void *priv)
{
	struct rpmsg_device *rdev = ept->rdev;
	struct rpmsg_virtio_device *rvdev = (struct rpmsg_virtio_device *)rdev;
	struct rpmsg_endpoint *_ept;
	struct rpmsg_ns_msg *ns_msg;
	int status;

	(void)priv;
	(void)src;

	ns_msg = (struct rpmsg_ns_msg *)data;

	/* check if a Ept has been locally registered */
	metal_mutex_acquire(&rdev->lock);
	_ept = rpmsg_get_ept_from_remote_addr(rdev, ns_msg->addr);
	metal_mutex_release(&rdev->lock);

	if (ns_msg->flags & RPMSG_NS_DESTROY) {
		if (!_ept)
			return;
		if (_ept->destroy_cb)
			_ept->destroy_cb(_ept);

		rpmsg_destroy_ept(_ept);
#if 0
		if (_ept->addr == RPMSG_ADDR_ANY)
			metal_free_memory(_ept);
#endif
	} else {
		struct metal_io_region *io = rvdev->shbuf_io;

		if (!_ept) {
			/*create an endpoint to store remote end point */
			_ept = (struct rpmsg_endpoint *)
				metal_allocate_memory(sizeof(*_ept));
			metal_io_block_read(io,
				metal_io_virt_to_offset(io, ns_msg->name),
				&_ept->name, len);
			_ept->addr = RPMSG_ADDR_ANY;
			status = rpmsg_register_endpoint(rdev, _ept);
			if (status < 0) {
				metal_free_memory(ept);
				return;
			}
			if (rdev->new_endpoint_cb)
				rdev->new_endpoint_cb(_ept);
		}
		_ept->dest_addr = ns_msg->addr;
	}
}

struct rpmsg_device_ops rpmsg_virtio_rpmsg_device_ops = {
	.send_offchannel_raw = rpmsg_virtio_send_offchannel_raw,
};

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
 * @param rvdev  - pointer to the rpmsg device
 * @param vdev   - pointer to the virtio device
 * @param shm_io - pointer to the share memory I/O region.
 * @param shm    - pointer to the share memory.
 * @param len    - length of the shared memory section.
 *
 * @return - status of function execution
 */

int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev, struct metal_io_region *shm_io,
		    void *shm, unsigned int len)
{
	struct rpmsg_device *rdev;
	const char *vq_names[RPMSG_NUM_VRINGS];
	void (*callback[RPMSG_NUM_VRINGS]) (struct virtqueue *vq);
	unsigned long dev_features;
	int status;
	unsigned int i;

	if (!rvdev)
		return -RPMSG_ERR_INIT;
	memset(rvdev, 0, sizeof(*rvdev));
	rdev = &rvdev->rdev;
	metal_mutex_init(&rdev->lock);
	rvdev->vdev = vdev;
	vdev->priv = rvdev;
	rdev->ops = &rpmsg_virtio_rpmsg_device_ops;
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/*
		 * Since device is RPMSG Remote so we need to manage the
		 * shared buffers. Create shared memory pool to handle buffers.
		 */
		if (!shm || !len)
			return -RPMSG_ERR_NO_MEM;

		rvdev->shbuf =
		    sh_mem_create_pool(shm, len, RPMSG_BUFFER_SIZE);

		if (!rvdev->shbuf)
			return RPMSG_ERR_NO_MEM;

		vq_names[0] = "rx_vq";
		vq_names[1] = "tx_vq";
		callback[0] = rpmsg_virtio_rx_callback;
		callback[1] = rpmsg_virtio_tx_callback;
		rvdev->rvq  = vdev->vrings_info[0].vq;
		rvdev->svq  = vdev->vrings_info[1].vq;
	} else {
		vq_names[0] = "tx_vq";
		vq_names[1] = "rx_vq";
		callback[0] = rpmsg_virtio_tx_callback;
		callback[1] = rpmsg_virtio_rx_callback;
		rvdev->rvq  = vdev->vrings_info[1].vq;
		rvdev->svq  = vdev->vrings_info[0].vq;
	}

	rvdev->shbuf_io = shm_io;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE) {
		/* wait synchro with the master */
		rpmsg_virtio_wait_remote_ready(rvdev);
	}

	/* Create virtqueues for remote device */
	status = rpmsg_virtio_create_virtqueues(rvdev, 0, RPMSG_NUM_VRINGS,
					       vq_names, callback);
	if (status != RPMSG_SUCCESS)
		return status;

	/* TODO: can have a virtio function to set the shared memory I/O */
	for (i = 0; i < RPMSG_NUM_VRINGS; i++) {
		struct virtqueue *vq;

		vq = vdev->vrings_info[i].vq;
		vq->shm_io = shm_io;
	}

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		struct virtqueue_buf vqbuf;
		unsigned int idx;
		void * buffer;

                vqbuf.len = RPMSG_BUFFER_SIZE;
                for (idx = 0; ((idx < rvdev->rvq->vq_nentries)
                               && (idx < rvdev->shbuf->total_buffs / 2));
                     idx++) {

                        /* Initialize TX virtqueue buffers for remote device */
                        buffer = sh_mem_get_buffer(rvdev->shbuf);

                        if (!buffer) {
                                return RPMSG_ERR_NO_BUFF;
                        }

                        vqbuf.buf = buffer;

                        metal_io_block_set(shm_io,
                                metal_io_virt_to_offset(shm_io, buffer),
                                0x00,
                                RPMSG_BUFFER_SIZE);
                        status =
                            virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1,
                                                 buffer);

                        if (status != RPMSG_SUCCESS) {
                                return status;
                        }
                }

	}

	/* Initialize channels and endpoints list */
	metal_list_init(&rdev->endpoints);

	dev_features = rpmsg_virtio_get_features(rvdev);

	/*
	 * Create name service announcement endpoint if device supports name
	 * service announcement feature.
	 */
	if ((dev_features & (1 << VIRTIO_RPMSG_F_NS))) {
		if (!rpmsg_create_ept(rdev, "NS", RPMSG_NS_EPT_ADDR,
				      RPMSG_NS_EPT_ADDR,
				      rpmsg_virtio_ns_callback,
				      NULL))
			return RPMSG_ERR_NO_MEM;

	}
	/* check if there is message sent from master */
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE)
		rpmsg_virtio_rx_callback(vdev->vrings_info[0].vq);
	else
		rpmsg_virtio_set_status(rvdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);


	return status;
}

void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev)
{
	struct metal_list *node;
	struct rpmsg_device *rdev;
	struct rpmsg_endpoint *ept;

	rdev = &rvdev->rdev;
	metal_mutex_acquire(&rdev->lock);
	while (!metal_list_is_empty(&rdev->endpoints)) {
		node = rdev->endpoints.next;
		ept = metal_container_of(node, struct rpmsg_endpoint, node);
		rpmsg_destroy_ept(ept);
	}

	rvdev->rvq = 0;
	rvdev->svq = 0;
	if (rvdev->shbuf) {
		sh_mem_delete_pool(rvdev->shbuf);
		rvdev->shbuf = NULL;
	}
	metal_mutex_release(&rdev->lock);

	metal_mutex_deinit(&rdev->lock);
}

