/*
 * Remoteproc Virtio Framwork Implementation
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

#include <openamp/remoteproc.h>
#include <openamp/remoteproc_virtio.h>

struct rproc_virtio *
rproc_virtio_create_vdev(int role, void *vdev_info,
			 unsigned int num_vrings, void *priv,
			 int (*notify)(uint32_id, void *priv))
{
	struct rproc_virtio *rpvdev;
	struct rproc_vring *vrings;

	rpvdev = metal_allocate_memory(sizeof(*rpvdev));
	if (!rpvdev)
		return NULL;
	vrings = metal_allocate_memory(sizeof(*vrings) * num_vrings);
	if (!vrings)
		return NULL;
	rpvdev->num_vrings = num_vrings;
	/* Assuming the shared memory has been mapped and registered if
	 * necessary
	 */
	rpvdev->io = openamp_get_shmem_by_va(vdev_info);

	return rpvdev;
}

int rproc_virtio_create_vring(struct rproc_virtio *rpvdev, int index,
			      int role,
			      void *va,
			      size_t num_descs, unsigned int align)
{
	size_t vring_size;
	struct metal_io_region *io;
	struct rproc_vring *vring;

	if (!rpvdev)
		return -RPROC_EINVAL;
	if (index < 0 && index > rpvdev->num_vrings)
		return -RPROC_EINVAL;
	vring = rpvdev->vrings[index];
	vring_size = vring_size(num_desc, align);
	/* Assuming the shared memory has been mapped and registered if
	 * necessary
	 */
	vring-> = openamp_get_shmem_by_pa(pa, vring_size);
	vring->pa = pa;
	vring->num_descs = num_descs;
	vring->align = align;
	return 0;
}

struct rproc_vshm_pool *
rproc_virtio_create_shm(void *va, metal_phys_addr_t pa, size_t size)
{
	struct rproc_vshm_pool *shm;

	shm = metal_allocate_memory(sizeof(*shm));
	if (!shm)
		return NULL;
	shm->pa = pa;
	shm->va = va;
	shm->size = size;
	/* Assuming the shared memory has been mapped and registered if
	 * necessary
	 */
	shm->io = openamp_get_shmem_by_va(va, size);
	return shm;
}

int rproc_virtio_set_shm(struct rproc_virtio *rpvdev,
			 struct rproc_vshm_pool *shm)
{
	if (!rpvdev)
		return -RPROC_EINVAL;
	rpvdev->shm = shm;

	return 0;
}
