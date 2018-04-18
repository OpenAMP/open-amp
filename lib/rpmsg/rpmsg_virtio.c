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
void rpmsg_return_buffer(struct rpmsg_virtio_device *rvdev, void *buffer,
			 unsigned long len, unsigned short idx)
{
	struct virtqueue_buf vqbuf;
	if (rvdev->vdev->role == RPMSG_REMOTE) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1, buffer);
	} else {
		virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
	}
}

/**
 * rpmsg_enqueue_buffers
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

	if (rvdev->vdev->role == RPMSG_MASTER) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		status = virtqueue_add_buffer(rvdev->svq, &vqbuf, 0, 1, buffer);
	} else {
		status = virtqueue_add_consumed_buffer(rvdev->svq, idx, len);
	}

	return status;
}
