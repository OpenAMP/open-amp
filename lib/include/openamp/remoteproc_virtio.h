/*
 * Remoteproc Virtio Framwork
 *
 * Copyright(c) 2018 Xilinx Ltd.
 * Copyright(c) 2011 Texas Instruments, Inc.
 * Copyright(c) 2011 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name Texas Instruments nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef REMOTEPROC_VIRTIO_H
#define REMOTEPROC_VIRTIO_H

#include <metal/io.h>
#include <metal/list.h>

#if defined __cplusplus
extern "C" {
#endif

#define VIRTIO_DEV_MASTER 0
#define VIRTIO_DEV_SLAVE  1

/* define vdev notification funciton user should implement */
typedef int (*notify_func)(uint32_t id, void *priv);

/**
 * struct remoteproc_vring - remoteproc vring structure
 * @vq virtio queue
 * @va logical address
 * @notifyid vring notify id
 * @num_descs number of descriptors
 * @align vring alignment
 * @io metal I/O region of the vring memory, can be NULL
 */
struct remoteproc_vring {
	struct virtqueue *vq;
	void *va;
	uint32_t notifyid;
	unsigned int num_descs;
	unsigned int align;
	struct metal_io_region *io;
};

/**
 * struct remoteproc_virtio
 * @rproc pointer to remoteproc instance
 * @rvrings vrings information list
 * @vdev pointer to virtio device
 * @num_vrings number of vrings
 * @vdev_rsc address of vdev informaiton
 * @vdev_rsc_io metal I/O region of vdev_info, can be NULL
 * @vdev virtio device
 */
struct remoteproc_virtio {
	struct remoteproc *rproc;
	struct remoteproc_vring *rvrings;
	unsigned int num_vrings;
	void *vdev_rsc;
	struct metal_io_region *vdev_rsc_io;
	struct virtio_device vdev;
};

/**
 * rproc_virtio_create_vdev
 *
 * Create rproc virtio vdev
 *
 * @rproc:  pointer to the remoteproc instance
 * @role: 0 - virtio master, 1 - virtio slave
 * @index: virtio device index
 * @rsc: pointer to the virtio device resource
 * @rst_cb: reset virtio device callback
 *
 * return pointer to the created virtio device for success,
 * NULL for failure.
 */
struct virtio_device *
rproc_virtio_create_vdev(struct remoteproc *rproc, unsigned int role,
			 int index, void *rsc,
			 int (*rst_cb)(struct virtio_device *vdev));

/**
 * rproc_virtio_create_vring
 *
 * Create rproc virtio vring
 *
 * @rpvdev: pointer to the rproc virtio device
 * @index: vring index
 * @va: vring virtual address
 * @num_desc: number of descriptors
 * @align: vring alignment
 *
 * return 0 for success, negative value for failure.
 */
int rproc_virtio_create_vring(struct remoteproc_virtio *rpvdev, int index,
			      void *va, size_t num_descs, unsigned int align);

#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_VIRTIO_H */
