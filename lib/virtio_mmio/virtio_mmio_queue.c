/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#include <openamp/open_amp.h>
#include <openamp/virtqueue.h>
#include <openamp/virtio.h>

struct iovec;

int virtqueue_enqueue_buf(struct virtqueue *vq, void *cookie, int writable, char *buffer,
			  unsigned int len)
{
	struct virtqueue_buf vb[1] = {{.buf = buffer, .len = len} };

	return virtqueue_add_buffer(vq, vb, writable ? 0 : 1, writable, cookie);
}

int virtqueue_enqueue(struct virtqueue *vq, void *cookie, struct iovec *iov, size_t readable,
		      size_t writable)
{
	return virtqueue_add_buffer(vq, (struct virtqueue_buf *)iov, readable, writable, cookie);
}

void *virtqueue_dequeue(struct virtqueue *vq, uint32_t *len)
{
	return virtqueue_get_buffer(vq, len, NULL);
}
