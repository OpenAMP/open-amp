/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************
 * FILE NAME
 *
 *       rpmsg_core.c
 *
 * COMPONENT
 *
 *       OpenAMP
 *
 * DESCRIPTION
 *
 * This file provides the core functionality of RPMSG messaging part like
 * message parsing ,Rx/Tx callbacks handling , channel creation/deletion
 * and address management.
 *
 *
 **************************************************************************/
#include "openamp/rpmsg.h"

/* Internal functions */
static void rpmsg_rx_callback(struct virtqueue *vq);
static void rpmsg_tx_callback(struct virtqueue *vq);

/**
 * rpmsg_start_ipc
 *
 * This function creates communication links(virtqueues) for remote device
 * and notifies it to start IPC.
 *
 * @param rdev - remote device handle
 *
 * @return - status of function execution
 *
 */
int rpmsg_start_ipc(struct remote_device *rdev)
{
	struct virtio_device *virt_dev;
	struct rpmsg_endpoint *ns_ept;
	void (*callback[2]) (struct virtqueue * vq);
	const char *vq_names[2];
	unsigned long dev_features;
	int status;
	struct virtqueue *vqs[2];
	int i;

	virt_dev = &rdev->virt_dev;

	/* Initialize names and callbacks based on the device role */
	if (rdev->role == RPMSG_MASTER) {
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
	status = virt_dev->func->create_virtqueues(virt_dev, 0,
						   RPMSG_MAX_VQ_PER_RDEV,
						   vq_names, callback,
						   RPMSG_NULL);
	if (status != RPMSG_SUCCESS) {
		return status;
	}

	dev_features = virt_dev->func->get_features(virt_dev);

	/*
	 * Create name service announcement endpoint if device supports name
	 * service announcement feature.
	 */
	if ((dev_features & (1 << VIRTIO_RPMSG_F_NS))) {
		rdev->support_ns = RPMSG_TRUE;
		ns_ept = _create_endpoint(rdev, rpmsg_ns_callback, rdev,
					  RPMSG_NS_EPT_ADDR);
		if (!ns_ept) {
			return RPMSG_ERR_NO_MEM;
		}
	}

	/* Initialize notifications for vring. */
	if (rdev->role == RPMSG_MASTER) {
		vqs[0] = rdev->tvq;
		vqs[1] = rdev->rvq;
	} else {
		vqs[0] = rdev->rvq;
		vqs[1] = rdev->tvq;
	}
	for (i = 0; i <= 1; i++) {
		status = hil_enable_vring_notifications(i, vqs[i]);
		if (status != RPMSG_SUCCESS) {
			return status;
		}
	}

	status = rpmsg_rdev_notify(rdev);

	return status;
}

/**
 * _rpmsg_create_channel
 *
 * Creates new rpmsg channel with the given parameters.
 *
 * @param rdev - pointer to remote device which contains the channel
 * @param name - name of the device
 * @param src  - source address for the rpmsg channel
 * @param dst  - destination address for the rpmsg channel
 *
 * @return - pointer to new rpmsg channel
 *
 */
struct rpmsg_channel *_rpmsg_create_channel(struct remote_device *rdev,
					    char *name, unsigned long src,
					    unsigned long dst)
{
	struct rpmsg_channel *rp_chnl;
	struct llist *node;

	rp_chnl = env_allocate_memory(sizeof(struct rpmsg_channel));
	if (rp_chnl) {
		env_memset(rp_chnl, 0x00, sizeof(struct rpmsg_channel));
		env_strncpy(rp_chnl->name, name, sizeof(rp_chnl->name));
		rp_chnl->src = src;
		rp_chnl->dst = dst;
		rp_chnl->rdev = rdev;
		/* Place channel on channels list */
		node = env_allocate_memory(sizeof(struct llist));
		if (!node) {
			env_free_memory(rp_chnl);
			return RPMSG_NULL;
		}
		node->data = rp_chnl;
		env_lock_mutex(rdev->lock);
		add_to_list(&rdev->rp_channels, node);
		env_unlock_mutex(rdev->lock);
	}

	return rp_chnl;
}

/**
 * _rpmsg_delete_channel
 *
 * Deletes given rpmsg channel.
 *
 * @param rp_chnl -  pointer to rpmsg channel to delete
 *
 * return - none
 */
void _rpmsg_delete_channel(struct rpmsg_channel *rp_chnl)
{
	struct llist *node;
	if (rp_chnl) {
		node =
		    rpmsg_rdev_get_chnl_node_from_id(rp_chnl->rdev,
						     rp_chnl->name);
		if (node) {
			env_lock_mutex(rp_chnl->rdev->lock);
			remove_from_list(&rp_chnl->rdev->rp_channels, node);
			env_unlock_mutex(rp_chnl->rdev->lock);
			env_free_memory(node);
		}
		env_free_memory(rp_chnl);
	}
}

/**
 * _create_endpoint
 *
 * This function creates rpmsg endpoint.
 *
 * @param rdev    - pointer to remote device
 * @param cb      - Rx completion call back
 * @param priv    - private data
 * @param addr    - endpoint src address
 *
 * @return - pointer to endpoint control block
 *
 */
struct rpmsg_endpoint *_create_endpoint(struct remote_device *rdev,
					rpmsg_rx_cb_t cb, void *priv,
					unsigned long addr)
{

	struct rpmsg_endpoint *rp_ept;
	struct llist *node;
	int status = RPMSG_SUCCESS;

	rp_ept = env_allocate_memory(sizeof(struct rpmsg_endpoint));
	if (!rp_ept) {
		return RPMSG_NULL;
	}
	env_memset(rp_ept, 0x00, sizeof(struct rpmsg_endpoint));

	node = env_allocate_memory(sizeof(struct llist));
	if (!node) {
		env_free_memory(rp_ept);
		return RPMSG_NULL;
	}

	env_lock_mutex(rdev->lock);

	if (addr != RPMSG_ADDR_ANY) {
		/*
		 * Application has requested a particular src address for endpoint,
		 * first check if address is available.
		 */
		if (!rpmsg_is_address_set
		    (rdev->bitmap, RPMSG_ADDR_BMP_SIZE, addr)) {
			/* Mark the address as used in the address bitmap. */
			rpmsg_set_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
					  addr);

		} else {
			status = RPMSG_ERR_DEV_ADDR;
		}
	} else {
		addr = rpmsg_get_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE);
		if (addr < 0) {
			status = RPMSG_ERR_DEV_ADDR;
		}
	}

	/* Do cleanup in case of error and return */
	if (status) {
		env_free_memory(node);
		env_free_memory(rp_ept);
		env_unlock_mutex(rdev->lock);
		return RPMSG_NULL;
	}

	rp_ept->addr = addr;
	rp_ept->cb = cb;
	rp_ept->priv = priv;

	node->data = rp_ept;
	add_to_list(&rdev->rp_endpoints, node);

	env_unlock_mutex(rdev->lock);

	return rp_ept;
}

/**
 * rpmsg_destroy_ept
 *
 * This function deletes rpmsg endpoint and performs cleanup.
 *
 * @param rdev   - pointer to remote device
 * @param rp_ept - pointer to endpoint to destroy
 *
 */
void _destroy_endpoint(struct remote_device *rdev,
		       struct rpmsg_endpoint *rp_ept)
{
	struct llist *node;
	node = rpmsg_rdev_get_endpoint_from_addr(rdev, rp_ept->addr);
	if (node) {
		env_lock_mutex(rdev->lock);
		rpmsg_release_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
				      rp_ept->addr);
		remove_from_list(&rdev->rp_endpoints, node);
		env_unlock_mutex(rdev->lock);
		env_free_memory(node);
	}
	env_free_memory(rp_ept);
}

/**
 * rpmsg_send_ns_message
 *
 * Sends name service announcement to remote device
 *
 * @param rdev    - pointer to remote device
 * @param rp_chnl - pointer to rpmsg channel
 * @param flags   - Channel creation/deletion flags
 *
 */
void rpmsg_send_ns_message(struct remote_device *rdev,
			   struct rpmsg_channel *rp_chnl, unsigned long flags)
{

	struct rpmsg_hdr *rp_hdr;
	struct rpmsg_ns_msg *ns_msg;
	unsigned short idx;
	unsigned long len;

	env_lock_mutex(rdev->lock);

	/* Get Tx buffer. */
	rp_hdr = (struct rpmsg_hdr *)rpmsg_get_tx_buffer(rdev, &len, &idx);
	if (!rp_hdr)
		return;

	/* Fill out name service data. */
	rp_hdr->dst = RPMSG_NS_EPT_ADDR;
	rp_hdr->len = sizeof(struct rpmsg_ns_msg);
	ns_msg = (struct rpmsg_ns_msg *)rp_hdr->data;
	env_strncpy(ns_msg->name, rp_chnl->name, sizeof(rp_chnl->name));
	ns_msg->flags = flags;
	ns_msg->addr = rp_chnl->src;

	/* Place the buffer on virtqueue. */
	rpmsg_enqueue_buffer(rdev, rp_hdr, len, idx);

	/* Notify the other side that it has data to process. */
	virtqueue_kick(rdev->tvq);

	env_unlock_mutex(rdev->lock);
}

/**
 * rpmsg_enqueue_buffers
 *
 * Places buffer on the virtqueue for consumption by the other side.
 *
 * @param rdev   - pointer to remote core
 * @param buffer - buffer pointer
 * @param len    - buffer length
 * @idx          - buffer index
 *
 * @return - status of function execution
 *
 */
int rpmsg_enqueue_buffer(struct remote_device *rdev, void *buffer,
			 unsigned long len, unsigned short idx)
{
	struct llist node;
	int status;

	/* Initialize buffer node */
	node.data = buffer;
	node.attr = len;
	node.next = RPMSG_NULL;
	node.prev = RPMSG_NULL;

	if (rdev->role == RPMSG_REMOTE) {
		status = virtqueue_add_buffer(rdev->tvq, &node, 0, 1, buffer);
	} else {
		status = virtqueue_add_consumed_buffer(rdev->tvq, idx, len);
	}

	return status;
}

/**
 * rpmsg_return_buffer
 *
 * Places the used buffer back on the virtqueue.
 *
 * @param rdev   - pointer to remote core
 * @param buffer - buffer pointer
 * @param len    - buffer length
 * @param idx    - buffer index
 *
 */
void rpmsg_return_buffer(struct remote_device *rdev, void *buffer,
			 unsigned long len, unsigned short idx)
{
	struct llist node;

	/* Initialize buffer node */
	node.data = buffer;
	node.attr = len;
	node.next = RPMSG_NULL;
	node.prev = RPMSG_NULL;

	if (rdev->role == RPMSG_REMOTE) {
		virtqueue_add_buffer(rdev->rvq, &node, 0, 1, buffer);
	} else {
		virtqueue_add_consumed_buffer(rdev->rvq, idx, len);
	}
}

/**
 * rpmsg_get_tx_buffer
 *
 * Provides buffer to transmit messages.
 *
 * @param rdev - pointer to remote device
 * @param len  - length of returned buffer
 * @param idx  - buffer index
 *
 * return - pointer to buffer.
 */
void *rpmsg_get_tx_buffer(struct remote_device *rdev, unsigned long *len,
			  unsigned short *idx)
{
	void *data;

	if (rdev->role == RPMSG_REMOTE) {
		data = virtqueue_get_buffer(rdev->tvq, (uint32_t *) len);
		if (data == RPMSG_NULL) {
			data = sh_mem_get_buffer(rdev->mem_pool);
			*len = RPMSG_BUFFER_SIZE;
		}
	} else {
		data =
		    virtqueue_get_available_buffer(rdev->tvq, idx,
						   (uint32_t *) len);
	}
	return ((void *)env_map_vatopa(data));
}

/**
 * rpmsg_get_rx_buffer
 *
 * Retrieves the received buffer from the virtqueue.
 *
 * @param rdev - pointer to remote device
 * @param len  - size of received buffer
 * @param idx  - index of buffer
 *
 * @return - pointer to received buffer
 *
 */
void *rpmsg_get_rx_buffer(struct remote_device *rdev, unsigned long *len,
			  unsigned short *idx)
{

	void *data;
	if (rdev->role == RPMSG_REMOTE) {
		data = virtqueue_get_buffer(rdev->rvq, (uint32_t *) len);
	} else {
		data =
		    virtqueue_get_available_buffer(rdev->rvq, idx,
						   (uint32_t *) len);
	}
	return ((void *)env_map_vatopa(data));
}

/**
 * rpmsg_free_buffer
 *
 * Frees the allocated buffers.
 *
 * @param rdev   - pointer to remote device
 * @param buffer - pointer to buffer to free
 *
 */
void rpmsg_free_buffer(struct remote_device *rdev, void *buffer)
{
	if (rdev->role == RPMSG_REMOTE) {
		sh_mem_free_buffer(rdev->mem_pool, buffer);
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
	struct remote_device *rdev;
	struct virtio_device *vdev;
	struct rpmsg_channel *rp_chnl;
	struct llist *chnl_hd;

	vdev = (struct virtio_device *)vq->vq_dev;
	rdev = (struct remote_device *)vdev;
	chnl_hd = rdev->rp_channels;

	/* Check if the remote device is master. */
	if (rdev->role == RPMSG_MASTER) {

		/* Notification is received from the master. Now the remote(us) can
		 * performs one of two operations;
		 *
		 * a. If name service announcement is supported then it will send NS message.
		 *    else
		 * b. It will update the channel state to active so that further communication
		 *    can take place.
		 */
		while (chnl_hd != RPMSG_NULL) {
			rp_chnl = (struct rpmsg_channel *)chnl_hd->data;

			if (rp_chnl->state == RPMSG_CHNL_STATE_IDLE) {

				if (rdev->support_ns) {
					rp_chnl->state = RPMSG_CHNL_STATE_NS;
				} else {
					rp_chnl->state =
					    RPMSG_CHNL_STATE_ACTIVE;
				}

				if (rp_chnl->state == RPMSG_CHNL_STATE_NS) {
					rpmsg_send_ns_message(rdev, rp_chnl,
							      RPMSG_NS_CREATE);
				}
			}

			chnl_hd = chnl_hd->next;
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
	struct remote_device *rdev;
	struct virtio_device *vdev;
	struct rpmsg_channel *rp_chnl;
	struct rpmsg_endpoint *rp_ept;
	struct rpmsg_hdr *rp_hdr;
	struct llist *node;
	unsigned long len;
	unsigned short idx;
	struct llist *chnl_hd;

	vdev = (struct virtio_device *)vq->vq_dev;
	rdev = (struct remote_device *)vdev;

	chnl_hd = rdev->rp_channels;
	if ((chnl_hd != RPMSG_NULL) && (rdev->role == RPMSG_MASTER)) {
		rp_chnl = (struct rpmsg_channel *)chnl_hd->data;
		if (rp_chnl->state == RPMSG_CHNL_STATE_IDLE) {
			if (rdev->support_ns) {
				rp_chnl->state = RPMSG_CHNL_STATE_NS;
				rpmsg_send_ns_message(rdev, rp_chnl,
						      RPMSG_NS_CREATE);
			} else {
				rp_chnl->state = RPMSG_CHNL_STATE_ACTIVE;
			}
			return;
		}
	}

	env_lock_mutex(rdev->lock);

	/* Process the received data from remote node */
	rp_hdr = (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rdev, &len, &idx);

	env_unlock_mutex(rdev->lock);

	while (rp_hdr) {

		/* Get the channel node from the remote device channels list. */
		node = rpmsg_rdev_get_endpoint_from_addr(rdev, rp_hdr->dst);

		if (!node)
			/* Fatal error no endpoint for the given dst addr. */
			return;

		rp_ept = (struct rpmsg_endpoint *)node->data;

		rp_chnl = rp_ept->rp_chnl;

		if ((rp_chnl) && (rp_chnl->state == RPMSG_CHNL_STATE_NS)) {
			/* First message from RPMSG Master, update channel
			 * destination address and state */
			rp_chnl->dst = rp_hdr->src;
			rp_chnl->state = RPMSG_CHNL_STATE_ACTIVE;

			/* Notify channel creation to application */
			if (rdev->channel_created) {
				rdev->channel_created(rp_chnl);
			}
		} else {
			rp_ept->cb(rp_chnl, rp_hdr->data, rp_hdr->len,
				   rp_ept->priv, rp_hdr->src);
		}

		env_lock_mutex(rdev->lock);

		/* Return used buffers. */
		rpmsg_return_buffer(rdev, rp_hdr, len, idx);

		rp_hdr =
		    (struct rpmsg_hdr *)rpmsg_get_rx_buffer(rdev, &len, &idx);
		env_unlock_mutex(rdev->lock);
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
void rpmsg_ns_callback(struct rpmsg_channel *server_chnl, void *data, int len,
		       void *priv, unsigned long src)
{
	struct remote_device *rdev;
	struct rpmsg_channel *rp_chnl;
	struct rpmsg_ns_msg *ns_msg;
	struct llist *node;

	rdev = (struct remote_device *)priv;

	//FIXME: This assumes same name string size for channel name both on master
	//and remote. If this is not the case then we will have to parse the
	//message contents.

	ns_msg = (struct rpmsg_ns_msg *)data;
	ns_msg->name[len - 1] = '\0';

	if (ns_msg->flags & RPMSG_NS_DESTROY) {
		node = rpmsg_rdev_get_chnl_node_from_id(rdev, ns_msg->name);
		if (node) {
			rp_chnl = (struct rpmsg_channel *)node->data;
			if (rdev->channel_destroyed) {
				rdev->channel_destroyed(rp_chnl);
			}
			rpmsg_destroy_ept(rp_chnl->rp_ept);
			_rpmsg_delete_channel(rp_chnl);
		}
	} else {
		rp_chnl =
		    _rpmsg_create_channel(rdev, ns_msg->name, 0x00,
					  ns_msg->addr);
		if (rp_chnl) {
			rp_chnl->state = RPMSG_CHNL_STATE_ACTIVE;
			/* Create default endpoint for channel */
			rp_chnl->rp_ept =
			    rpmsg_create_ept(rp_chnl, rdev->default_cb, rdev,
					     RPMSG_ADDR_ANY);
			if (rp_chnl->rp_ept) {
				rp_chnl->src = rp_chnl->rp_ept->addr;
				/*
				 * Echo back the NS message to remote in order to
				 * complete the connection stage. Remote will know the endpoint
				 * address from this point onward which will enable it to send
				 * message without waiting for any application level message from
				 * master.
				 */
				rpmsg_send(rp_chnl, data, len);
				if (rdev->channel_created) {
					rdev->channel_created(rp_chnl);
				}
			}
		}
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
			addr = tmp32 + i + 1;
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
