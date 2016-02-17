/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 * Copyright (c) 2016 NXP, Inc. All rights reserved.
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
 *       remote_device.c
 *
 * COMPONENT
 *
 *       OpenAMP Stack
 *
 * DESCRIPTION
 *
 * This file provides services to manage the remote devices.It also implements
 * the interface defined by the virtio and provides few other utility functions.
 *
 *
 **************************************************************************/

#include "openamp/rpmsg.h"

/* Macro to initialize vring HW info */
#define INIT_VRING_ALLOC_INFO(ring_info,vring_hw)                             \
                         (ring_info).phy_addr  = (vring_hw).phy_addr;         \
                         (ring_info).align     = (vring_hw).align;             \
                         (ring_info).num_descs = (vring_hw).num_descs

/* Local functions */
static int rpmsg_rdev_init_channels(struct remote_device *rdev);

/* Ops table for virtio device */
virtio_dispatch rpmsg_rdev_config_ops = {
	rpmsg_rdev_create_virtqueues,
	rpmsg_rdev_get_status,
	rpmsg_rdev_set_status,
	rpmsg_rdev_get_feature,
	rpmsg_rdev_set_feature,
	rpmsg_rdev_negotiate_feature,
	rpmsg_rdev_read_config,
	rpmsg_rdev_write_config,
	rpmsg_rdev_reset
};

/**
 * rpmsg_rdev_init
 *
 * This function creates and initializes the remote device. The remote device
 * encapsulates virtio device.
 *
 * @param rdev              - pointer to newly created remote device
 * @param dev-id            - ID of device to create , remote cpu id
 * @param role              - role of the other device, Master or Remote
 * @param channel_created   - callback function for channel creation
 * @param channel_destroyed - callback function for channel deletion
 * @param default_cb        - default callback for channel
 *
 * @return - status of function execution
 *
 */
int rpmsg_rdev_init(struct remote_device **rdev, int dev_id, int role,
		    rpmsg_chnl_cb_t channel_created,
		    rpmsg_chnl_cb_t channel_destroyed, rpmsg_rx_cb_t default_cb)
{

	struct remote_device *rdev_loc;
	struct virtio_device *virt_dev;
	struct hil_proc *proc;
	struct proc_shm *shm;
	int status;

	/* Initialize HIL data structures for given device */
	proc = hil_create_proc(dev_id);

	if (!proc) {
		return RPMSG_ERR_DEV_ID;
	}

	/* Create software representation of remote processor. */
	rdev_loc =
	    (struct remote_device *)
	    env_allocate_memory(sizeof(struct remote_device));

	if (!rdev_loc) {
		return RPMSG_ERR_NO_MEM;
	}

	env_memset(rdev_loc, 0x00, sizeof(struct remote_device));
	status = env_create_mutex(&rdev_loc->lock, 1);

	if (status != RPMSG_SUCCESS) {

		/* Cleanup required in case of error is performed by caller */
		return status;
	}

	rdev_loc->proc = proc;
	rdev_loc->role = role;
	rdev_loc->channel_created = channel_created;
	rdev_loc->channel_destroyed = channel_destroyed;
	rdev_loc->default_cb = default_cb;

	/* Restrict the ept address - zero address can't be assigned */
	rdev_loc->bitmap[0] = 1;

	/* Initialize the virtio device */
	virt_dev = &rdev_loc->virt_dev;
	virt_dev->device = proc;
	virt_dev->func = &rpmsg_rdev_config_ops;
	if (virt_dev->func->set_features != RPMSG_NULL) {
		virt_dev->func->set_features(virt_dev, proc->vdev.dfeatures);
	}

	if (rdev_loc->role == RPMSG_REMOTE) {
		/*
		 * Since device is RPMSG Remote so we need to manage the
		 * shared buffers. Create shared memory pool to handle buffers.
		 */
		shm = hil_get_shm_info(proc);
		rdev_loc->mem_pool =
		    sh_mem_create_pool(shm->start_addr, shm->size,
				       RPMSG_BUFFER_SIZE);

		if (!rdev_loc->mem_pool) {
			return RPMSG_ERR_NO_MEM;
		}
	}

	/* Initialize channels for RPMSG Remote */
	status = rpmsg_rdev_init_channels(rdev_loc);

	if (status != RPMSG_SUCCESS) {
		return status;
	}

	*rdev = rdev_loc;

	return RPMSG_SUCCESS;
}

/**
 * rpmsg_rdev_deinit
 *
 * This function un-initializes the remote device.
 *
 * @param rdev - pointer to remote device to deinit.
 *
 * @return - none
 *
 */
void rpmsg_rdev_deinit(struct remote_device *rdev)
{
	struct llist *rp_chnl_head, *rp_chnl_temp, *node;
	struct rpmsg_channel *rp_chnl;

	rp_chnl_head = rdev->rp_channels;

	while (rp_chnl_head != RPMSG_NULL) {

		rp_chnl_temp = rp_chnl_head->next;
		rp_chnl = (struct rpmsg_channel *)rp_chnl_head->data;

		if (rdev->channel_destroyed) {
			rdev->channel_destroyed(rp_chnl);
		}

		if ((rdev->support_ns) && (rdev->role == RPMSG_MASTER)) {
			rpmsg_send_ns_message(rdev, rp_chnl, RPMSG_NS_DESTROY);
		}

		/* Delete default endpoint for channel */
		if (rp_chnl->rp_ept) {
			rpmsg_destroy_ept(rp_chnl->rp_ept);
		}

		_rpmsg_delete_channel(rp_chnl);
		rp_chnl_head = rp_chnl_temp;
	}

	/* Delete name service endpoint */
	node = rpmsg_rdev_get_endpoint_from_addr(rdev, RPMSG_NS_EPT_ADDR);
	if (node) {
		_destroy_endpoint(rdev, (struct rpmsg_endpoint *)node->data);
	}

	if (rdev->rvq) {
		virtqueue_free(rdev->rvq);
	}
	if (rdev->tvq) {
		virtqueue_free(rdev->tvq);
	}
	if (rdev->mem_pool) {
		sh_mem_delete_pool(rdev->mem_pool);
	}
	if (rdev->lock) {
		env_delete_mutex(rdev->lock);
	}
	if (rdev->proc) {
		hil_delete_proc(rdev->proc);
		rdev->proc = 0;
	}

	env_free_memory(rdev);
}

/**
 * rpmsg_rdev_get_chnl_node_from_id
 *
 * This function returns channel node based on channel name.
 *
 * @param stack      - pointer to remote device
 * @param rp_chnl_id - rpmsg channel name
 *
 * @return - channel node
 *
 */
struct llist *rpmsg_rdev_get_chnl_node_from_id(struct remote_device *rdev,
					       char *rp_chnl_id)
{
	struct rpmsg_channel *rp_chnl;
	struct llist *rp_chnl_head;

	rp_chnl_head = rdev->rp_channels;

	env_lock_mutex(rdev->lock);
	while (rp_chnl_head) {
		rp_chnl = (struct rpmsg_channel *)rp_chnl_head->data;
		if (env_strncmp
		    (rp_chnl->name, rp_chnl_id, sizeof(rp_chnl->name))
		    == 0) {
			env_unlock_mutex(rdev->lock);
			return rp_chnl_head;
		}
		rp_chnl_head = rp_chnl_head->next;
	}
	env_unlock_mutex(rdev->lock);

	return RPMSG_NULL;
}

/**
 * rpmsg_rdev_get_chnl_from_addr
 *
 * This function returns channel node based on src/dst address.
 *
 * @param rdev - pointer remote device control block
 * @param addr - src/dst address
 *
 * @return - channel node
 *
 */
struct llist *rpmsg_rdev_get_chnl_from_addr(struct remote_device *rdev,
					    unsigned long addr)
{
	struct rpmsg_channel *rp_chnl;
	struct llist *rp_chnl_head;

	rp_chnl_head = rdev->rp_channels;

	env_lock_mutex(rdev->lock);
	while (rp_chnl_head) {
		rp_chnl = (struct rpmsg_channel *)rp_chnl_head->data;
		if ((rp_chnl->src == addr) || (rp_chnl->dst == addr)) {
			env_unlock_mutex(rdev->lock);
			return rp_chnl_head;
		}
		rp_chnl_head = rp_chnl_head->next;
	}
	env_unlock_mutex(rdev->lock);

	return RPMSG_NULL;
}

/**
 * rpmsg_rdev_get_endpoint_from_addr
 *
 * This function returns endpoint node based on src address.
 *
 * @param rdev - pointer remote device control block
 * @param addr - src address
 *
 * @return - endpoint node
 *
 */
struct llist *rpmsg_rdev_get_endpoint_from_addr(struct remote_device *rdev,
						unsigned long addr)
{
	struct llist *rp_ept_lut_head;

	rp_ept_lut_head = rdev->rp_endpoints;

	env_lock_mutex(rdev->lock);
	while (rp_ept_lut_head) {
		struct rpmsg_endpoint *rp_ept =
		    (struct rpmsg_endpoint *)rp_ept_lut_head->data;
		if (rp_ept->addr == addr) {
			env_unlock_mutex(rdev->lock);
			return rp_ept_lut_head;
		}
		rp_ept_lut_head = rp_ept_lut_head->next;
	}
	env_unlock_mutex(rdev->lock);

	return RPMSG_NULL;
}

/*
 * rpmsg_rdev_notify
 *
 * This function checks whether remote device is up or not. If it is up then
 * notification is sent based on device role to start IPC.
 *
 * @param rdev - pointer to remote device
 *
 * @return - status of function execution
 *
 */
int rpmsg_rdev_notify(struct remote_device *rdev)
{
	int status = RPMSG_SUCCESS;

	if (rdev->role == RPMSG_REMOTE) {
		status = hil_get_status(rdev->proc);

		/*
		 * Let the remote device know that Master is ready for
		 * communication.
		 */
		if (!status)
			virtqueue_kick(rdev->rvq);

	} else {
		status = hil_set_status(rdev->proc);
	}

	if (status == RPMSG_SUCCESS) {
		rdev->state = RPMSG_DEV_STATE_ACTIVE;
	}

	return status;
}

/**
 * rpmsg_rdev_init_channels
 *
 * This function is only applicable to RPMSG remote. It obtains channel IDs
 * from the HIL and creates RPMSG channels corresponding to each ID.
 *
 * @param rdev - pointer to remote device
 *
 * @return  - status of function execution
 *
 */
int rpmsg_rdev_init_channels(struct remote_device *rdev)
{
	struct rpmsg_channel *rp_chnl;
	struct proc_chnl *chnl_info;
	int num_chnls, idx;

	if (rdev->role == RPMSG_MASTER) {

		chnl_info = hil_get_chnl_info(rdev->proc, &num_chnls);
		for (idx = 0; idx < num_chnls; idx++) {

			rp_chnl =
			    _rpmsg_create_channel(rdev, chnl_info[idx].name,
						  0x00, RPMSG_NS_EPT_ADDR);
			if (!rp_chnl) {
				return RPMSG_ERR_NO_MEM;
			}

			rp_chnl->rp_ept =
			    rpmsg_create_ept(rp_chnl, rdev->default_cb, rdev,
					     RPMSG_ADDR_ANY);

			if (!rp_chnl->rp_ept) {
				return RPMSG_ERR_NO_MEM;
			}

			rp_chnl->src = rp_chnl->rp_ept->addr;
		}
	}

	return RPMSG_SUCCESS;
}

/**
 *------------------------------------------------------------------------
 * The rest of the file implements the virtio device interface as defined
 * by the virtio.h file.
 *------------------------------------------------------------------------
 */
int rpmsg_rdev_create_virtqueues(struct virtio_device *dev, int flags, int nvqs,
				 const char *names[], vq_callback * callbacks[],
				 struct virtqueue *vqs_[])
{
	struct remote_device *rdev;
	struct vring_alloc_info ring_info;
	struct virtqueue *vqs[RPMSG_MAX_VQ_PER_RDEV];
	struct proc_vring *vring_table;
	void *buffer;
	struct llist node;
	int idx, num_vrings, status;

	rdev = (struct remote_device *)dev;

	/* Get the vring HW info for the given virtio device */
	vring_table = hil_get_vring_info(&rdev->proc->vdev, &num_vrings);

	if (num_vrings > nvqs) {
		return RPMSG_ERR_MAX_VQ;
	}

	/* Create virtqueue for each vring. */
	for (idx = 0; idx < num_vrings; idx++) {

		INIT_VRING_ALLOC_INFO(ring_info, vring_table[idx]);

		if (rdev->role == RPMSG_REMOTE) {
			env_memset((void *)ring_info.phy_addr, 0x00,
				   vring_size(vring_table[idx].num_descs,
					      vring_table[idx].align));
		}

		status =
		    virtqueue_create(dev, idx, (char *)names[idx], &ring_info,
				     callbacks[idx], hil_vring_notify,
				     &vqs[idx]);

		if (status != RPMSG_SUCCESS) {
			return status;
		}
	}

	//FIXME - a better way to handle this , tx for master is rx for remote and vice versa.
	if (rdev->role == RPMSG_MASTER) {
		rdev->tvq = vqs[0];
		rdev->rvq = vqs[1];
	} else {
		rdev->tvq = vqs[1];
		rdev->rvq = vqs[0];
	}

	if (rdev->role == RPMSG_REMOTE) {
		for (idx = 0; ((idx < rdev->rvq->vq_nentries)
			       && (idx < rdev->mem_pool->total_buffs / 2));
		     idx++) {

			/* Initialize TX virtqueue buffers for remote device */
			buffer = sh_mem_get_buffer(rdev->mem_pool);

			if (!buffer) {
				return RPMSG_ERR_NO_BUFF;
			}

			node.data = buffer;
			node.attr = RPMSG_BUFFER_SIZE;
			node.next = RPMSG_NULL;

			env_memset(buffer, 0x00, RPMSG_BUFFER_SIZE);
			status =
			    virtqueue_add_buffer(rdev->rvq, &node, 0, 1,
						 buffer);

			if (status != RPMSG_SUCCESS) {
				return status;
			}
		}
	}

	return RPMSG_SUCCESS;
}

unsigned char rpmsg_rdev_get_status(struct virtio_device *dev)
{
	return 0;
}

void rpmsg_rdev_set_status(struct virtio_device *dev, unsigned char status)
{

}

uint32_t rpmsg_rdev_get_feature(struct virtio_device *dev)
{
	return dev->features;
}

void rpmsg_rdev_set_feature(struct virtio_device *dev, uint32_t feature)
{
	dev->features |= feature;
}

uint32_t rpmsg_rdev_negotiate_feature(struct virtio_device *dev,
				      uint32_t features)
{
	return 0;
}

/*
 * Read/write a variable amount from the device specific (ie, network)
 * configuration region. This region is encoded in the same endian as
 * the guest.
 */
void rpmsg_rdev_read_config(struct virtio_device *dev, uint32_t offset,
			    void *dst, int length)
{
	return;
}

void rpmsg_rdev_write_config(struct virtio_device *dev, uint32_t offset,
			     void *src, int length)
{
	return;
}

void rpmsg_rdev_reset(struct virtio_device *dev)
{
	return;
}
