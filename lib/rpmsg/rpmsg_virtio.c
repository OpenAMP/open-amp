/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg_virtio.h>
#include <string.h>
#include "openamp/rpmsg.h"
#include "openamp/remoteproc.h"
#include "openamp/hil.h"
#include "metal/utilities.h"
#include "metal/alloc.h"
#include "metal/atomic.h"
#include "metal/cpu.h"

/* Macro to initialize vring HW info */
#define INIT_VRING_ALLOC_INFO(ring_info, vring_hw)                  \
			(ring_info).vaddr     = (vring_hw).vaddr;   \
			(ring_info).align     = (vring_hw).align;   \
			(ring_info).num_descs = (vring_hw).num_descs

unsigned char rpmsg_virtio_get_status(struct virtio_device *dev)
{
	struct fw_rsc_vdev *vdev_rsc = (struct fw_rsc_vdev *)dev->vdev_info;

	if (!vdev_rsc)
		return -1;

	atomic_thread_fence(memory_order_seq_cst);
	return vdev_rsc->status;
}

void rpmsg_virtio_set_status(struct virtio_device *dev, unsigned char status)
{
	struct fw_rsc_vdev *vdev_rsc = (struct fw_rsc_vdev *)dev->vdev_info;

	if (!vdev_rsc)
		return;

	vdev_rsc->status = status;

	atomic_thread_fence(memory_order_seq_cst);
}


uint32_t rpmsg_virtio_get_feature(struct virtio_device *dev)
{
	return dev->features;
}

void rpmsg_virtio_set_feature(struct virtio_device *dev, uint32_t feature)
{
	dev->features |= feature;
}

/**
 *------------------------------------------------------------------------
 * The rest of the file implements the virtio device interface as defined
 * by the virtio.h file.
 *------------------------------------------------------------------------
 */
int rpmsg_virtio_create_virtqueues(struct virtio_device *vdev, int flags,
				   unsigned int  nvqs, const char *names[],
				   vq_callback * callbacks[],
				   struct virtqueue *vqs_[])
{
	struct rpmsg_virtio *rpmsg_dev = vdev->client_dev;
	struct vring_alloc_info ring_info;
	struct virtio_vring_info *vring_table = vdev->vring_info;
	void *buffer;
	struct metal_sg sg;
	unsigned int idx, num_vrings = vdev->num_vrings;
	int status = RPMSG_SUCCESS;

	(void)flags;

	if (num_vrings > nvqs)
		return RPMSG_ERR_MAX_VQ;

	/* Create virtqueue for each vring. */
	for (idx = 0; idx < num_vrings; idx++) {

		INIT_VRING_ALLOC_INFO(ring_info, vring_table[idx]);

		if (rpmsg_dev->role == RPMSG_MASTER) {
			metal_io_block_set(vring_table[idx].io,
				metal_io_virt_to_offset(vring_table[idx].io,
							ring_info.vaddr),
				0x00,
				vring_size(vring_table[idx].num_descs,
				vring_table[idx].align));
		}

		status =
		    virtqueue_create(vdev, idx, (char *)names[idx], &ring_info,
				     callbacks[idx], _notify,
				     vring_table[idx].io, vqs_[idx]);

		if (status != RPMSG_SUCCESS)
			return status;
	}

	if (rpmsg_dev->role == RPMSG_MASTER) {
		rpmsg_dev->tvq = vqs_[1];
		rpmsg_dev->rvq = vqs_[0];
	} else {
		rpmsg_dev->tvq = vqs_[0];
		rpmsg_dev->rvq = vqs_[1];
	}

	if (rpmsg_dev->role == RPMSG_MASTER) {
		sg.io = vdev->sh_buff.io;
		sg.len = RPMSG_BUFFER_SIZE;
		for (idx = 0; ((idx < rpmsg_dev->rvq->vq_nentries)
			       && (idx < rpmsg_dev->mem_pool->total_buffs / 2));
		     idx++) {

			/* Initialize TX virtqueue buffers for remote device */
			buffer = sh_mem_get_buffer(rpmsg_dev->mem_pool);

			if (!buffer)
				return RPMSG_ERR_NO_BUFF;

			sg.virt = buffer;

			metal_io_block_set(sg.io,
				metal_io_virt_to_offset(sg.io, buffer),
				0x00,
				RPMSG_BUFFER_SIZE);
			status = virtqueue_add_buffer(rpmsg_dev->rvq, &sg, 0,
						      1, buffer);
		}
	}

	return status;
}
