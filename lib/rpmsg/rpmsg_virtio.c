/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg_virtio.h>
#include <openamp/rpmsg.h>


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
#if !defined VIRTIO_SLAVE_ONLY
	struct virtqueue_buf vqbuf;
#endif

#if !defined VIRTIO_SLAVE_ONLY  && !defined  VIRTIO_MASTER_ONLY
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1, buffer);
	} else {
		virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
	}
	
#else
#if defined VIRTIO_MASTER_ONLY
	(void)idx;
	/* Initialize buffer node */
	vqbuf.buf = buffer;
	vqbuf.len = len;
	virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1, buffer);
#else
	(void)buffer;
	virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
#endif	
#endif
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
#if !defined VIRTIO_SLAVE_ONLY
	struct virtqueue_buf vqbuf;
#endif
	
#if !defined VIRTIO_SLAVE_ONLY  && !defined  VIRTIO_MASTER_ONLY
	if (rpmsg_virtio_get_role(rvdev) == RPMSG_MASTER) {
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		return virtqueue_add_buffer(rvdev->svq, &vqbuf, 0, 1, buffer);
	} else {
		return virtqueue_add_consumed_buffer(rvdev->svq, idx, len);
	}
	return ERROR_VQUEUE_INVLD_PARAM;
#else
#if defined VIRTIO_MASTER_ONLY
	(void)idx;
	/* Initialize buffer node */
	vqbuf.buf = buffer;
	vqbuf.len = len;
	return virtqueue_add_buffer(rvdev->svq, &vqbuf, 0, 1, buffer);
#else
	(void)buffer;
	return virtqueue_add_consumed_buffer(rvdev->svq, idx, len);
#endif	
#endif
}
