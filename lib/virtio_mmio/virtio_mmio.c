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
#include <openamp/virtio_mmio.h>
#include <metal/device.h>

/*
 * Declaration of inline functions as external functions to force the inclusion
 * of the function symbols in the library
 */
extern uint32_t virtio_get_devid(const struct virtio_device *vdev);
extern uint8_t virtio_get_status(struct virtio_device *vdev);
extern void virtio_set_status(struct virtio_device *vdev, uint8_t status);
extern void virtio_read_config(struct virtio_device *vdev,
			       uint32_t offset, void *dst, int length);
extern void virtio_write_config(struct virtio_device *vdev,
				uint32_t offset, void *src, int length);
extern uint32_t virtio_get_features(struct virtio_device *vdev);
extern void virtio_set_features(struct virtio_device *vdev, uint32_t features);
extern void virtqueue_notify(struct virtqueue *vq);
extern void virtio_reset_vd(struct virtio_device *vdev);
extern int virtqueue_empty(struct virtqueue *vq);
extern int virtqueue_full(struct virtqueue *vq);

static inline void virtio_mmio_write32(struct virtio_device *vdev, int offset, uint32_t value)
{
	struct virtio_mmio_device *vmdev = (struct virtio_mmio_device *)vdev->priv;

	metal_io_write32(vmdev->cfg_io, offset, value);
}

static inline uint32_t virtio_mmio_read32(struct virtio_device *vdev, int offset)
{
	struct virtio_mmio_device *vmdev = (struct virtio_mmio_device *)vdev->priv;

	return metal_io_read32(vmdev->cfg_io, offset);
}

static inline uint8_t virtio_mmio_read8(struct virtio_device *vdev, int offset)
{
	struct virtio_mmio_device *vmdev = (struct virtio_mmio_device *)vdev->priv;

	return metal_io_read8(vmdev->cfg_io, offset);
}

static void virtio_mmio_set_status(struct virtio_device *vdev, uint8_t status)
{
	virtio_mmio_write32(vdev, VIRTIO_MMIO_STATUS, status);
}

static uint8_t virtio_mmio_get_status(struct virtio_device *vdev)
{
	return virtio_mmio_read32(vdev, VIRTIO_MMIO_STATUS);
}

static void virtio_mmio_write_config(struct virtio_device *vdev,
				     uint32_t offset, void *dst, int length)
{
	(void)(vdev);
	(void)(offset);
	(void)(dst);
	(void)length;

	metal_log(METAL_LOG_WARNING, "%s not supported\n", __func__);
}

static void virtio_mmio_read_config(struct virtio_device *vdev,
				    uint32_t offset, void *dst, int length)
{
	int i;
	uint8_t *d = dst;
	(void)(offset);

	for (i = 0; i < length; i++)
		d[i] = virtio_mmio_read8(vdev, VIRTIO_MMIO_CONFIG + i);
}

static uint32_t _virtio_mmio_get_features(struct virtio_device *vdev, int idx)
{
	uint32_t hfeatures;

	virtio_mmio_write32(vdev, VIRTIO_MMIO_DEVICE_FEATURES_SEL, idx);
	hfeatures = virtio_mmio_read32(vdev, VIRTIO_MMIO_DEVICE_FEATURES);
	return hfeatures & vdev->features;
}

static uint32_t virtio_mmio_get_features(struct virtio_device *vdev)
{
	return _virtio_mmio_get_features(vdev, 0);
}

/* This is more like negotiate_features */
static void _virtio_mmio_set_features(struct virtio_device *vdev,
				      uint32_t features, int idx)
{
	uint32_t hfeatures;

	virtio_mmio_write32(vdev, VIRTIO_MMIO_DRIVER_FEATURES_SEL, idx);
	hfeatures = virtio_mmio_read32(vdev, VIRTIO_MMIO_DEVICE_FEATURES);
	features &= hfeatures;
	virtio_mmio_write32(vdev, VIRTIO_MMIO_DRIVER_FEATURES, features);
	vdev->features = features;
}

static void virtio_mmio_set_features(struct virtio_device *vdev, uint32_t features)
{
	_virtio_mmio_set_features(vdev, features, 0);
}

static void virtio_mmio_reset_device(struct virtio_device *vdev)
{
	virtio_mmio_write32(vdev, VIRTIO_MMIO_STATUS, 0);
}

static void virtio_mmio_notify(struct virtqueue *vq)
{
	virtio_mmio_write32(vq->vq_dev, VIRTIO_MMIO_QUEUE_NOTIFY, vq->vq_queue_index);
}

const struct virtio_dispatch virtio_mmio_dispatch = {
	.get_status = virtio_mmio_get_status,
	.set_status = virtio_mmio_set_status,
	.get_features = virtio_mmio_get_features,
	.set_features = virtio_mmio_set_features,
	.read_config = virtio_mmio_read_config,
	.write_config = virtio_mmio_write_config,
	.reset_device = virtio_mmio_reset_device,
	.notify = virtio_mmio_notify,
};

static int virtio_mmio_get_metal_io(struct virtio_device *vdev, uintptr_t virt_mem_ptr,
				    uintptr_t cfg_mem_ptr)
{
	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
	struct metal_device *device;
	int32_t err;
	int device_idx = 0;
	struct virtio_mmio_device *vmdev = vdev->priv;

	metal_params.log_level = METAL_LOG_DEBUG;

	/* Setup shared memory device */
	vmdev->shm_device.regions[0].physmap = (metal_phys_addr_t *)&vmdev->shm_mem.base;
	vmdev->shm_device.regions[0].virt = (void *)virt_mem_ptr;
	vmdev->shm_device.regions[0].size = vmdev->shm_mem.size;
	vmdev->shm_device.regions[1].physmap = (metal_phys_addr_t *)&vmdev->cfg_mem.base;
	vmdev->shm_device.regions[1].virt = (void *)cfg_mem_ptr;
	vmdev->shm_device.regions[1].size = vmdev->cfg_mem.size;

	/* Libmetal setup */
	err = metal_init(&metal_params);
	if (err) {
		metal_log(METAL_LOG_ERROR, "metal_init: failed - error code %d\n", err);
		return err;
	}

	err = metal_register_generic_device(&vmdev->shm_device);
	if (err) {
		metal_log(METAL_LOG_ERROR, "Couldn't register shared memory device: %d\n", err);
		return err;
	}

	err = metal_device_open("generic", vmdev->shm_device.name, &device);
	if (err) {
		metal_log(METAL_LOG_ERROR, "metal_device_open failed: %d", err);
		return err;
	}

	vmdev->shm_io = metal_device_io_region(device, device_idx);
	if (!vmdev->shm_io) {
		metal_log(METAL_LOG_ERROR, "metal_device_io_region failed to get region %d",
			  device_idx);
		return err;
	}

	vmdev->cfg_io = metal_device_io_region(device, 1);
	if (!vmdev->cfg_io) {
		metal_log(METAL_LOG_ERROR, "metal_device_io_region failed to get region 1");
		return err;
	}

	return 0;
}

static int virtio_mmio_max_data_size_get(struct virtio_device *d)
{
	ARG_UNUSED(d);

	return VIRTIO_MMIO_MAX_DATA_SIZE;
}

uint32_t virtio_mmio_get_max_elem(struct virtio_device *vdev, int idx)
{
	/* Select the queue we're interested in */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_SEL, idx);
	return virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_NUM_MAX);
}

static struct virtqueue *virtio_mmio_set_vq(struct virtio_device *vdev, struct virtqueue *vq)
{
	uint64_t addr;
	unsigned int num;

	/* Select the queue we're interested in */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_SEL, vq->vq_queue_index);

	/* Queue shouldn't already be set up. */
	if (virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_READY)) {
		metal_log(METAL_LOG_WARNING, "%s: %s already setup\n", __func__, vq->vq_name);
		return NULL;
	}

	num = virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_NUM_MAX);
	if (num == 0) {
		metal_log(METAL_LOG_WARNING, "%s: %s max queue is NULL\n", __func__, vq->vq_name);
		return NULL;
	}

	if (num < vq->vq_nentries) {
		metal_log(METAL_LOG_WARNING, "%s: %s nb entries %d exceeds max %d",
			  __func__, vq->vq_name, vq->vq_nentries, num);
		return NULL;
	}

	/* Activate the queue */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_NUM, vq->vq_nentries);

	addr = (uint64_t)metal_io_virt_to_phys(vq->shm_io, vq->vq_ring.desc);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_DESC_LOW, addr);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_DESC_HIGH, addr >> 32);

	addr = (uint64_t)metal_io_virt_to_phys(vq->shm_io, vq->vq_ring.avail);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_AVAIL_LOW, addr);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_AVAIL_HIGH, addr >> 32);

	addr = (uint64_t)metal_io_virt_to_phys(vq->shm_io, vq->vq_ring.used);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_USED_LOW, addr);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_USED_HIGH, addr >> 32);

	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_READY, 1);

	return vq;
}

int virtio_device_init(struct virtio_mmio_device *vmdev, uintptr_t virt_mem_ptr,
		       uintptr_t cfg_mem_ptr, void *user_data)
{
	struct virtio_device *vdev = &vmdev->vdev;
	uint32_t magic, version, devid, vendor;

	vdev->role = vmdev->device_mode;
	vdev->priv = vmdev;
	vdev->func = &virtio_mmio_dispatch;
	vmdev->user_data = user_data;

	/* Set metal io mem ops */
	virtio_mmio_get_metal_io(vdev, virt_mem_ptr, cfg_mem_ptr);

	magic = virtio_mmio_read32(vdev, VIRTIO_MMIO_MAGIC_VALUE);
	if (magic != MAGIC_VALUE) {
		metal_log(METAL_LOG_ERROR, "Bad magic value %08x\n", magic);
		return -1;
	}

	version = virtio_mmio_read32(vdev, VIRTIO_MMIO_VERSION);
	devid = virtio_mmio_read32(vdev, VIRTIO_MMIO_DEVICE_ID);
	if (devid == 0) {
		/* Placeholder */
		return -1;
	}

	if (devid == VIRTIO_ID_SCMI && version != 0x2) {
		metal_log(METAL_LOG_ERROR, "VIRTIO SCMI: Version %d not supported!",
			  vdev->id.version);
		return -1;
	}
	if (version != 1) {
		metal_log(METAL_LOG_ERROR, "Bad version %08x\n", version);
		return -1;
	}

	vendor = virtio_mmio_read32(vdev, VIRTIO_MMIO_VENDOR_ID);
	metal_log(METAL_LOG_DEBUG, "VIRTIO %08x:%08x\n", vendor, devid);

	vdev->id.version = version;
	vdev->id.device = devid;
	vdev->id.vendor = vendor;

	virtio_mmio_set_status(vdev, VIRTIO_CONFIG_STATUS_ACK);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_GUEST_PAGE_SIZE, 4096);

	return 0;
}

/* Register preallocated virtqueues */
void virtio_mmio_register_device(struct virtio_device *vdev, int vq_num, struct virtqueue **vqs)
{
	int i;

	vdev->vrings_info = metal_allocate_memory(sizeof(struct virtio_vring_info) * vq_num);
	/* TODO: handle error case */
	for (i = 0; i < vq_num; i++) {
		vdev->vrings_info[i].vq = vqs[i];
	}
	vdev->vrings_num = vq_num;
}

struct virtqueue *virtio_mmio_setup_virtqueue(struct virtio_device *vdev,
					      unsigned int idx,
					      struct virtqueue *vq,
					      void (*cb)(void *),
					      void *cb_arg,
					      const char *vq_name)
{
	uint32_t maxq;
	struct virtio_vring_info _vring_info = {0};
	struct virtio_vring_info *vring_info = &_vring_info;
	struct vring_alloc_info *vring_alloc_info;
	int offset;
	struct virtio_mmio_device *vmdev = vdev->priv;

	vq->vq_dev = vdev;
	vring_info->io = vmdev->shm_io;
	vring_info->info.num_descs = virtio_mmio_get_max_elem(vdev, idx);
	vring_info->info.align = VRING_ALIGNMENT;

	offset = 0;

	if (!vq) {
		if (vdev->role == (unsigned int)VIRTIO_DEV_MASTER) {
			/* Master preallocated buffer before vring */
			offset += vring_info->info.num_descs * virtio_mmio_max_data_size_get(vdev);
		}

		/* Align offset */
		offset = (offset + vring_info->info.align - 1) & ~(vring_info->info.align - 1);

		/* Set vring base address */
		vring_info->info.vaddr = metal_io_virt(vring_info->io, offset);
		vring_info->vq = k_malloc(sizeof(struct virtqueue) +
					vring_info->info.num_descs * sizeof(struct vq_desc_extra));
		vq = vring_info->vq;
	} else {
		/* Preallocated virtqueues; vrings can be already declared via VIRTQUEUE_DECLARE */
		/* Check if vrings are already configured */
		if (vq->vq_nentries != 0 && vq->vq_nentries == vq->vq_free_cnt &&
		    vq->vq_ring.desc) {
			vring_info->info.vaddr = vq->vq_ring.desc;
			vring_info->vq = vq;
		}
		vring_info->info.num_descs = vq->vq_nentries;
	}

	vring_alloc_info = &vring_info->info;

	unsigned int role_bk = vdev->role;
	/* Assign OA VIRTIO_DEV_MASTER master role to allow virtio guests to setup the vrings */
	vdev->role = (unsigned int)VIRTIO_DEV_MASTER;
	if (virtqueue_create(vdev, idx, vq_name, vring_alloc_info, (void (*)(struct virtqueue *))cb,
			     NULL, vring_info->vq)) {
		metal_log(METAL_LOG_ERROR, "virtqueue_create failed\n");
		return NULL;
	}
	vdev->role = role_bk;
	vq->cb_arg = cb_arg;
	virtqueue_set_shmem_io(vq, vmdev->shm_io);

	if (vdev->id.version == 0x2) {
		/* SCMI for now */
		vq = virtio_mmio_set_vq(vdev, vring_info->vq);
	} else {
		virtio_mmio_write32(vdev, VIRTIO_MMIO_DRIVER_FEATURES_SEL, idx);
		maxq = virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_NUM_MAX);
		metal_assert(maxq != 0);
		metal_assert(maxq >= vq->vq_nentries);
		virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_NUM, vq->vq_nentries);
		virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_ALIGN, 4096);
		virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_PFN,
				    ((uintptr_t)metal_io_virt_to_phys(vq->shm_io,
				    (char *)vq->vq_ring.desc)) / 4096);
	}

	vdev->vrings_num++;
	virtqueue_enable_cb(vq);

	return vq;
}

void virtio_mmio_isr(struct virtio_device *vdev)
{
	struct virtio_vring_info *vrings_info = vdev->vrings_info;

	uint32_t isr = virtio_mmio_read32(vdev, VIRTIO_MMIO_INTERRUPT_STATUS);
	struct virtqueue *vq;
	unsigned int i;

	if (isr & VIRTIO_MMIO_INT_VRING) {
		for (i = 0; i < vdev->vrings_num; i++) {
			vq = vrings_info[i].vq;
			if (vq->callback)
				vq->callback(vq->cb_arg);
		}
	}
	if (isr & VIRTIO_MMIO_INT_CONFIG) {
		/* Placeholder */
	}
	virtio_mmio_write32(vdev, VIRTIO_MMIO_INTERRUPT_ACK, isr);
}
