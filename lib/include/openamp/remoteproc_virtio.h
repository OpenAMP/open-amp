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

/* define vdev notification funciton user should implement */
typedef int (*notify_func)(uint32_t id, void *priv);

/**
 * struct rproc_vring - remoteproc vring structure
 * @vq virtio queue
 * @va logical address
 * @num_descs number of descriptors
 * @align vring alignment
 * @io metal I/O region of the vring memory, can be NULL
 */
struct rproc_vring {
	struct virtqueue *vq;
	void *va;
	unsigned int num_descs;
	unsigned int align;
	struct metal_io_region *io;
};

/**
 * struct rproc_vshm - remoteproc vring structure
 * @va logical address of the start of the shared memory
 * @pa physical address of the start of the shared memory
 * @size size of the shared memory pool
 * @io metal I/O region of the shared memory, can be NULL
 */
struct rproc_vshm_pool {
	void *va;
	metal_phys_addr_t pa;
	ssize_t size;
	struct metal_io_region *io;
};


/**
 * struct rproc_virtio
 * @mems shared memory list created by the rproc_virtio device
 * @vrings vrings information list
 * @vdev pointer to virtio device
 * @num_vrings number of vrings
 * @vdev_info address of vdev informaiton
 * @vdev_info_io metal I/O region of vdev_info, can be NULL
 * @vdev virtio device
 * @priv private data
 * @notify function to notify the remote
 * @node remoteproc virtio devices node
 */
struct rproc_virtio {
	struct rproc_vring *vrings;
	unsigned int num_vrings;
	void *vdev_info;
	struct metal_io_region *vdev_info_io;
	struct virtio_device vdev;
	struct rproc_vshm_pool *shm;
	void *priv;
	int (*notify)(uint32_t id, void *priv);
	struct metal_list node;
};

/**
 * rproc_virtio_create_vdev
 *
 * Create rproc virtio vdev
 *
 * @role: 0 - virtio master, 1 - virtio slave
 * @vdev_info: pointer to the vdev_info
 * @num_vrings: number of vrings
 * @priv: pointer to private data
 * @notify: notify funciton
 *
 * return pointer to the created remoteproc virtio device for success,
 * NULL for failure.
 */
struct rproc_virtio *
rproc_virtio_create_vdev(int role, void *vdev_info,
			 unsigned int num_vrings, void *priv,
			 int (*notify)(uint32_id, void *priv));

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
int rproc_virtio_create_vring(struct rproc_virtio *rpvdev, int index,
			      void *va, size_t num_descs, unsigned int align);

/**
 * rproc_virtio_create_shm
 *
 * Create rproc virtio shared memory
 *
 * @va: logical address of the shared memory
 * @pa: physical address of the shared memory
 * @size: shared memory size
 *
 * return pointer to the created remoteproc shared memory pool for success,
 * NULL for failure.
 */
struct rproc_vshm_pool *
rproc_virtio_create_shm(void *va, metal_phys_addr_t pa, size_t size);

/**
 * rproc_virtio_set_shm
 *
 * Set rproc virtio shared memory
 *
 * @rpvdev: pointer to the rproc virtio device
 * @shm: pointer to the shared memory descriptor
 *
 * return 0 for success, negative value for failure
 */
int rproc_virtio_set_shm(struct rproc_virtio *rpvdev,
			 struct rproc_vshm_pool *shm);

#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_VIRTIO_H */
