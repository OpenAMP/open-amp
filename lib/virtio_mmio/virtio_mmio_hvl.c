/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#if defined(HVL_VIRTIO)

#include <openamp/open_amp.h>
#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <openamp/virtio_mmio.h>
#include <openamp/virtio_mmio_hvl.h>
#include <metal/device.h>
#include <metal/mutex.h>
#include <metal/sleep.h>

/*
 * The implementation depends on OS-specific services for memory pool management
 * routines, blocking wait and IPI.
 * TODO: Replace them in a future iteration of the hypervisorless virtio feature
 * set with APIs (to be introduced) in libmetal.
 */

/* hypervisorless virtio callback framework */
static virtio_mmio_hvl_cb_t _cb_funcs[VIRTIO_MMIO_HVL_CB_MAX];
static void *_cb_args[VIRTIO_MMIO_HVL_CB_MAX];
static int _num_cb_funcs;

/* pre-shared memory */
uint32_t __shmem0_pool_start __attribute__((weak));
uint32_t __shmem0_end __attribute__((weak));

void virtio_mmio_hvl_init(struct virtio_device *vdev)
{
	static int init;
	uint32_t *shmem_pool_mem = NULL;
	uint32_t shmem_pool_size = 0;
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	VIRTIO_ASSERT(vmdev->hvl_func->hvl_shm_pool_init,
		      "Invalid HVL dispatch configuration");
	VIRTIO_ASSERT(vmdev->hvl_func->hvl_shm_pool_alloc,
		      "Invalid HVL dispatch configuration");
	VIRTIO_ASSERT(vmdev->hvl_func->hvl_shm_pool_free,
		      "Invalid HVL dispatch configuration");
	VIRTIO_ASSERT(vmdev->hvl_func->hvl_wait_cfg_event,
		      "Invalid HVL dispatch configuration");
	VIRTIO_ASSERT(vmdev->hvl_func->hvl_ipi,
		      "Invalid HVL dispatch configuration");

	if (!init) {
		shmem_pool_mem = &__shmem0_pool_start;
		if (shmem_pool_mem != 0) {
			shmem_pool_size = ((uintptr_t)&__shmem0_end -
					   (uintptr_t)&__shmem0_pool_start);
			vmdev->hvl_func->hvl_shm_pool_init(shmem_pool_mem, shmem_pool_size);
			init = 1;
		}
	}
	if (vdev) {
		virtio_mmio_hvl_set_ipi(vdev, vmdev->hvl_func->hvl_ipi, NULL);
	}
	metal_mutex_init(&vmdev->cfg_sem);
	metal_mutex_acquire(&vmdev->cfg_sem);
}

int virtio_mmio_hvl_cb_set(virtio_mmio_hvl_cb_t func, void *arg)
{
	if (_num_cb_funcs < VIRTIO_MMIO_HVL_CB_MAX - 1) {
		_cb_funcs[_num_cb_funcs] = func;
		_cb_args[_num_cb_funcs] = arg;
		_num_cb_funcs++;
		return 0;
	}
	return -1;
}

void virtio_mmio_hvl_cb_run(void)
{
	int i = 0;

	for (i = 0; i < _num_cb_funcs; i++) {
		if (_cb_funcs[i]) {
			_cb_funcs[i](_cb_args[i]);
		}
	}
}

int virtio_mmio_hvl_wait_cfg(struct virtio_device *vdev, int offset, uint32_t data)
{
	uint32_t val;
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	if (!vmdev) {
		return -1;
	}

	if (vmdev->hvl_mode == 0) {
		return 0;
	}

	val = metal_io_read32(vmdev->cfg_io, offset);

	if (val == data) {
		return 0;
	}

	vmdev->hvl_func->hvl_wait_cfg_event(vdev, 4000000);

	val = metal_io_read32(vmdev->cfg_io, offset);
	if (val == data) {
		return 0;
	}

	return -1;
}

uint64_t virtio_mmio_hvl_add_bounce_buf(struct virtqueue *vq, void *cookie, char *buffer,
					unsigned int len)
{
	struct vq_bounce_buf *bb = NULL;
	uint64_t addr = 0;
	char *buf = NULL;
	struct virtio_mmio_device *vmdev = NULL;

	if (!vq || !cookie || !buffer || len == 0)
		return 0;

	vmdev = metal_container_of(vq->vq_dev, struct virtio_mmio_device, vdev);

	if (vmdev && vmdev->hvl_mode == 1) {
		/* check if buffer is already in pre-shared shared memory */
		addr = metal_io_virt_to_phys(vq->shm_io, buffer);
		if (addr != METAL_BAD_PHYS) {
			return addr;
		}

		buf = vmdev->hvl_func->hvl_shm_pool_alloc(len);
		if (buf) {
			memcpy(buf, buffer, len);
			addr = metal_io_virt_to_phys(vq->shm_io, buf);
			bb = vmdev->hvl_func->hvl_shm_pool_alloc(sizeof(struct vq_bounce_buf));
			if (bb) {
				bb->addr = (uintptr_t)buffer;
				bb->len = len;
				bb->cookie = cookie;
				metal_list_add_tail(&vmdev->bounce_buf_list, &bb->node);
				return addr;
			}
			vmdev->hvl_func->hvl_shm_pool_free(buf);
		}
	}

	return 0;
}

void virtio_mmio_hvl_get_bounce_buf(struct virtqueue *vq, struct vring_desc *dp, void *cookie)
{
	struct vq_bounce_buf *bb = NULL;
	struct metal_list *node = NULL;
	struct virtio_mmio_device *vmdev = NULL;

	VIRTIO_ASSERT(vq, "Invalid virtqueue");
	VIRTIO_ASSERT(dp, "Invalid vring descriptor");
	VIRTIO_ASSERT(cookie, "Invalid cookie");

	vmdev = metal_container_of(vq->vq_dev, struct virtio_mmio_device, vdev);

	if (vmdev && vmdev->hvl_mode == 1) {
		metal_list_for_each(&vmdev->bounce_buf_list, node) {
			bb = metal_container_of(node, struct vq_bounce_buf, node);
			if (bb->cookie == cookie) {
				memcpy((void *)(uintptr_t)bb->addr,
				       (void *)(metal_io_phys_to_virt(vq->shm_io, dp->addr)),
				       bb->len);
				metal_list_del(node);
				vmdev->hvl_func->hvl_shm_pool_free(bb);
				vmdev->hvl_func->hvl_shm_pool_free(
					(void *)(metal_io_phys_to_virt(vq->shm_io, dp->addr)));
				break;
			}
		}
	}
}

void virtio_mmio_hvl_set_ipi(struct virtio_device *vdev, virtio_mmio_hvl_ipi_t ipi_func,
			     void *param)
{
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	vmdev->ipi = ipi_func;
	vmdev->ipi_param = param;
}

#endif /* defined(HVL_VIRTIO) */

