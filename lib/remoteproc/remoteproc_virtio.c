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

static int rproc_virtio_notify(unsigned int notifyid, struct remoteproc *rproc)
{
	return rproc->kick(rproc, id);
}

struct _virtio_dispatch_ remoteproc_virtio_dispatch_funcs = {
	.create_virtqueue = rproc_vritio_create_virtqueues,
	.get_status =  rproc_virtio_get_status,
	.set_status = rproc_virtio_set_status,
	.get_features = rproc_virtio_get_features,
	.set_features = rproc_virtio_set_features,
	.negotiate_features = rproc_virtio_negotiate_features,
	.read_config = rproc_read_config,
	.write_config = rproc_write_config,
	.reset_device = rproc_reset_device,
};

struct virtio_device *
rproc_virtio_create_vdev(struct remoteporc *rproc, unsigned int role,
			 int index, void *rsc,
			 int (*rst_cb)(struct virtio_device *vdev))
{
	struct remoteproc_virtio *rpvdev;
	struct remoteproc_vring *rvrings;
	struct fw_rsc_vdev *vdev_rsc = rsc;
	struct fw_rsc_vdev_vring *rvring_rsc;
	struct virtio_device *vdev;
	unsigned int num_vrings = vdev_rsc->num_of_vrings;
	unsigned int i;

	rpvdev = metal_allocate_memory(sizeof(*rpvdev) +
				       sizeof(struct virtqueue)*
				       (num_vrings - 1));
	if (!rpvdev)
		return NULL;
	rvrings = metal_allocate_memory(sizeof(*rvrings) * num_vrings);
	if (!rvrings)
		return NULL;
	rpvdev->num_vrings = num_vrings;
	rpvdev->rvrings = rvrings;
	/* Assuming the shared memory has been mapped and registered if
	 * necessary
	 */
	rpvdev->vdev_rsc = vdev_rsc;
	rpvdev->vdev_rsc_io = remoteproc_get_mem_with_va(rproc,vdev_rsc);

	/* Initialize remoteproc virtio vrings */
	for (i = 0; i < num_vrings; i++) {
		struct metal_io_region *io;
		unsigned int num_descs = rvring_rsc->num;
		unsigned int align =  rvring_rsc->align;
		void *va;

		rvring_rsc = &vdev_rsc->vring[i];
		rvrings[i].num_descs = num_descs
		rvrings[i].align = align;
		va = remoteproc_get_mem_with_da(rproc, vring_rsc->da,
						vring_size(num_descs, align),
						*io);
		rvrings[i].va = va;
		rvrings[i].io = io;
	}

	memset(&rpvdev->vdev);
	vdev = rpvdev->vdev;
	vdev->index = index;
	vdev->role = role;
	vdev->rst_cb = rst_cb;
	vdev->vrings_num = num_vrings;
	metal_spinlock_init(&vdev->lock);
	/* TODO: Shall we set features here ? */

	return &rpvdev->vdev;
}


int rproc_virtio_set_shm(struct remoteproc_virtio *rpvdev,
			 struct remoteproc_vshm_pool *shm)
{
	if (!rpvdev)
		return -RPROC_EINVAL;
	rpvdev->shm = shm;

	return 0;
}
