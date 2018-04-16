/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_core.h>


#if 0
static struct rpmsg_endpoint *rpmsg_get_ept_from_id(
				struct rpmsg_virtio_device *rvdev,
				char *name, unsigned int addr)
{
	struct rpmsg_endpoint *ept;
	struct metal_list *node;

	metal_list_for_each(&rvdev->endpoints, node) {
		ept = metal_container_of(node, struct rpmsg_endpoint, node);
		if ((strncmp(ept->name, name, sizeof(ept->name)) == 0) &&
		    (ept->addr == addr))
			return ept;
	}

	return RPMSG_NULL;
}
#endif

static struct rpmsg_endpoint *rpmsg_get_ept_from_remote_addr(
			      struct rpmsg_virtio_device *rvdev, uint32_t addr)
{
	struct rpmsg_endpoint *ept;
	struct metal_list *node;

	metal_list_for_each(&rvdev->endpoints, node) {
		ept = metal_container_of(node, struct rpmsg_endpoint, node);
		if (ept->dest_addr == addr)
			return ept;
	}

	return RPMSG_NULL;
}

struct rpmsg_endpoint *rpmsg_get_ept_from_addr(
			      struct rpmsg_virtio_device *rvdev, uint32_t addr)
{
	struct rpmsg_endpoint *ept;
	struct metal_list *node;

	metal_list_for_each(&rvdev->endpoints, node) {
		ept = metal_container_of(node, struct rpmsg_endpoint, node);
		if (ept->addr == addr)
			return ept;
	}

	return RPMSG_NULL;
}

int rpmsg_register_endpoint(struct rpmsg_virtio_device *rvdev,
			    struct rpmsg_endpoint *ept)
{
	struct metal_list *node;
	struct rpmsg_endpoint *r_ept;
	uint32_t dest_addr = RPMSG_ADDR_ANY;

	metal_mutex_acquire(&rvdev->lock);

	if (ept->addr != RPMSG_ADDR_ANY) {
		/*
		 * Application has requested a particular src address for
		 * endpoint, first check if address is available.
		 */
		if (!rpmsg_is_address_set
		    (rvdev->bitmap, RPMSG_ADDR_BMP_SIZE, ept->addr)) {
			/* Mark the address as used in the address bitmap. */
			rpmsg_set_address(rvdev->bitmap, RPMSG_ADDR_BMP_SIZE,
					  ept->addr);

		} else {
			return RPMSG_ERR_ADDR;
		}
	} else {
		ept->addr = rpmsg_get_address(rvdev->bitmap,
					      RPMSG_ADDR_BMP_SIZE);
		if ((int)ept->addr < 0)
			return RPMSG_ERR_ADDR;
	}

	/* Check if a remote endpoint has been registered */
	metal_list_for_each(&rvdev->endpoints, node) {
		r_ept = metal_container_of(node, struct rpmsg_endpoint, node);
		if ((strncmp(r_ept->name, ept->name, sizeof(ept->name)) == 0) &&
		    (r_ept->addr == RPMSG_ADDR_ANY)) {
			/* Free the temporary endpoint in the list */
			dest_addr = r_ept->dest_addr;
			metal_list_del(&r_ept->node);
			metal_free_memory(r_ept);
		}
	}

	metal_list_add_tail(&rvdev->endpoints, &ept->node);
	ept->dest_addr = dest_addr;

	metal_mutex_release(&rvdev->lock);

	return RPMSG_SUCCESS;
}

int rpmsg_send_ns_message(struct rpmsg_virtio_device *rvdev,
			  struct rpmsg_endpoint *ept, unsigned long flags)
{
	struct rpmsg_hdr rp_hdr;
	struct rpmsg_ns_msg ns_msg;
	unsigned short idx;
	unsigned long len;
	struct metal_io_region *io;
	void *shbuf;

	metal_mutex_acquire(&rvdev->lock);

	/* Get Tx buffer. */
	shbuf = rpmsg_get_tx_buffer(rvdev, &len, &idx);
	if (!shbuf) {
		metal_mutex_release(&rvdev->lock);
		return RPMSG_ERR_NO_BUFF;
	}

	/* Fill out name service data. */
	rp_hdr.dst = RPMSG_NS_EPT_ADDR;
	rp_hdr.len = sizeof(ns_msg);
	ns_msg.flags = flags;
	ns_msg.addr = ept->addr;
	strncpy(ns_msg.name, ept->name, sizeof(ns_msg.name));

	io = rvdev->shbuf_io;
	metal_io_block_write(io, metal_io_virt_to_offset(io, shbuf),
			     &rp_hdr, sizeof(rp_hdr));
	metal_io_block_write(io,
			metal_io_virt_to_offset(io, RPMSG_LOCATE_DATA(shbuf)),
			&ns_msg, rp_hdr.len);

	/* Place the buffer on virtqueue. */
	rpmsg_virtio_enqueue_buffer(rvdev, shbuf, len, idx);

	/* Notify the other side that it has data to process. */
	virtqueue_kick(rvdev->svq);

	metal_mutex_release(&rvdev->lock);
	return RPMSG_SUCCESS;
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
void *rpmsg_get_tx_buffer(struct rpmsg_virtio_device *rvdev,
			  unsigned long *len, unsigned short *idx)
{
	void *data;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE) {
		data = virtqueue_get_buffer(rvdev->svq, (uint32_t *)len, idx);
		if (data == RPMSG_NULL) {
			data = sh_mem_get_buffer(rvdev->shbuf);
			*len = RPMSG_BUFFER_SIZE;
			return 0;
		}
	} else {
		data =
		    virtqueue_get_available_buffer(rvdev->svq, idx,
						   (uint32_t *)len);
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
void *rpmsg_get_rx_buffer(struct rpmsg_virtio_device *rvdev, unsigned long *len,
			  unsigned short *idx)
{
	void *data;

	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE) {
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
int rpmsg_wait_remote_ready(struct rpmsg_virtio_device *rvdev)
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
 * rpmsg_tx_callback
 *
 * Tx callback function.
 *
 * @param vq - pointer to virtqueue on which Tx is has been
 *             completed.
 *
 */
void rpmsg_tx_callback(struct virtqueue *vq)
{
	struct virtio_device *vdev = vq->vq_dev;
	struct rpmsg_virtio_device *rvdev = vdev->priv;
	struct rpmsg_endpoint *ept;
	unsigned long dev_features;
	struct metal_list *node;

	/* Check if the remote device is master. */
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_REMOTE) {
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
		metal_list_for_each(&rvdev->endpoints, node) {
			ept = metal_container_of(node, struct rpmsg_endpoint,
						 node);

			dev_features = rpmsg_virtio_get_features(rvdev);
			if ((dev_features & (1 << VIRTIO_RPMSG_F_NS))) {
				if (rpmsg_send_ns_message(rvdev, ept,
					RPMSG_NS_CREATE) != RPMSG_SUCCESS)
					return;
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
	struct rpmsg_virtio_device *rvdev = vdev->priv;
	struct rpmsg_endpoint *ept;
	struct rpmsg_hdr *rp_hdr;
	unsigned long len;
	unsigned short idx;

	metal_mutex_acquire(&rvdev->lock);

	/* Process the received data from remote node */
	rp_hdr = (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rvdev, &len, &idx);

	metal_mutex_release(&rvdev->lock);

	while (rp_hdr) {
		/* Get the channel node from the remote device channels list. */
		metal_mutex_acquire(&rvdev->lock);
		ept = rpmsg_get_ept_from_addr(rvdev, rp_hdr->dst);
		metal_mutex_release(&rvdev->lock);

		if (!ept)
			/* Fatal error no endpoint for the given dst addr. */
			return;

		if (ept && ept->dest_addr == RPMSG_ADDR_ANY) {
			/*
			 * First message from RPMSG Master, update channel
			 * destination address
			 */
			ept->dest_addr = rp_hdr->src;
		} else {
			ept->cb(ept, (void *)RPMSG_LOCATE_DATA(rp_hdr),
				   rp_hdr->len, ept->addr, ept->priv);
		}

		metal_mutex_acquire(&rvdev->lock);

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
		rpmsg_return_buffer(rvdev, rp_hdr, len, idx);
#endif

		rp_hdr =
		    (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rvdev, &len,
							    &idx);
		metal_mutex_release(&rvdev->lock);
	}
}

/**
 * rpmsg_ns_callback
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
void rpmsg_ns_callback(struct rpmsg_endpoint *ept, void *data,
		       size_t len, uint32_t src, void *priv)
{
	struct rpmsg_virtio_device *rvdev = ept->rvdev;
	struct rpmsg_endpoint *_ept;
	struct rpmsg_ns_msg *ns_msg;
	int status;
	(void)priv;

	ns_msg = (struct rpmsg_ns_msg *)data;

	/* check if a Ept has been locally registered */
	metal_mutex_acquire(&rvdev->lock);
	_ept = rpmsg_get_ept_from_remote_addr(rvdev, ns_msg->addr);
	metal_mutex_release(&rvdev->lock);

	if (ns_msg->flags & RPMSG_NS_DESTROY) {
		if (!_ept)
			return;
		if (_ept->destroy_cb)
			_ept->destroy_cb(ept);

		rpmsg_destroy_ept(_ept);
		if (_ept->addr == RPMSG_ADDR_ANY)
			metal_free_memory(_ept);
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
			status = rpmsg_register_endpoint(rvdev, _ept);
			if (status < 0) {
				metal_free_memory(ept);
				return;
			}
			if (rvdev->new_endpoint_cb)
				rvdev->new_endpoint_cb(_ept->name,
						       _ept->dest_addr);
		}
		_ept->dest_addr = src;
	}
}

/**
 * rpmsg_get_address
 *
 * This function provides unique 32 bit address.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 *
 * return - a unique address
 */
int rpmsg_get_address(unsigned long *bitmap, int size)
{
	int addr = -1;
	int i, tmp32;

	/* Find first available buffer */
	for (i = 0; i < size; i++) {
		tmp32 = get_first_zero_bit(bitmap[i]);

		if (tmp32 < 32) {
			addr = tmp32 + (i * 32);
			bitmap[i] |= (1 << tmp32);
			break;
		}
	}

	return addr;
}

/**
 * rpmsg_release_address
 *
 * Frees the given address.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - none
 */
int rpmsg_release_address(unsigned long *bitmap, int size, int addr)
{
	unsigned int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the addr as available */
	i = addr / 32;
	j = addr % 32;

	mask = mask << j;
	bitmap[i] = bitmap[i] & (~mask);

	return RPMSG_SUCCESS;
}

/**
 * rpmsg_is_address_set
 *
 * Checks whether address is used or free.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - TRUE/FALSE
 */
int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr)
{
	int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the id as available */
	i = addr / 32;
	j = addr % 32;
	mask = mask << j;

	return (bitmap[i] & mask);
}

/**
 * rpmsg_set_address
 *
 * Marks the address as consumed.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - none
 */
int rpmsg_set_address(unsigned long *bitmap, int size, int addr)
{
	int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the id as available */
	i = addr / 32;
	j = addr % 32;
	mask = mask << j;
	bitmap[i] |= mask;

	return RPMSG_SUCCESS;
}

