/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/device.h>
#include <openamp/open_amp.h>
#include <openamp/virtio.h>
#include <openamp/virtio_mmio.h>
#include <openamp/virtqueue.h>
#include <stdbool.h>

void virtio_mmio_isr(struct virtio_device *vdev);

typedef void (*virtio_mmio_vq_callback)(void *);

static int virtio_mmio_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
					 unsigned int nvqs, const char *names[],
					 vq_callback callbacks[], void *callback_args[]);

static inline void virtio_mmio_write32(struct virtio_device *vdev, int offset, uint32_t value)
{
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	metal_io_write32(vmdev->cfg_io, offset, value);
}

static inline uint32_t virtio_mmio_read32(struct virtio_device *vdev, int offset)
{
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	return metal_io_read32(vmdev->cfg_io, offset);
}

static inline uint8_t virtio_mmio_read8(struct virtio_device *vdev, int offset)
{
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	return metal_io_read8(vmdev->cfg_io, offset);
}

static inline void virtio_mmio_set_status(struct virtio_device *vdev, uint8_t status)
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

	/* Writing selection register VIRTIO_MMIO_DEVICE_FEATURES_SEL. In pure AMP
	 * mode this needs to be followed by a synchronization w/ the device
	 * before reading VIRTIO_MMIO_DEVICE_FEATURES
	 */
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

	/* Writing selection register VIRTIO_MMIO_DEVICE_FEATURES_SEL. In pure AMP
	 * mode this needs to be followed by a synchronization w/ the device
	 * before reading VIRTIO_MMIO_DEVICE_FEATURES
	 */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_DEVICE_FEATURES_SEL, idx);
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
	virtio_mmio_set_status(vdev, 0);
}

static void virtio_mmio_notify(struct virtqueue *vq)
{
	/* VIRTIO_F_NOTIFICATION_DATA is not supported for now */
	virtio_mmio_write32(vq->vq_dev, VIRTIO_MMIO_QUEUE_NOTIFY, vq->vq_queue_index);
}

const struct virtio_dispatch virtio_mmio_dispatch = {
	.create_virtqueues = virtio_mmio_create_virtqueues,
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
	struct metal_device *device;
	int32_t err;
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	/* Setup shared memory device */
	vmdev->shm_device.regions[0].physmap = (metal_phys_addr_t *)&vmdev->shm_mem.base;
	vmdev->shm_device.regions[0].virt = (void *)virt_mem_ptr;
	vmdev->shm_device.regions[0].size = vmdev->shm_mem.size;

	VIRTIO_ASSERT((METAL_MAX_DEVICE_REGIONS > 1),
		      "METAL_MAX_DEVICE_REGIONS must be greater that 1");

	vmdev->shm_device.regions[1].physmap = (metal_phys_addr_t *)&vmdev->cfg_mem.base;
	vmdev->shm_device.regions[1].virt = (void *)cfg_mem_ptr;
	vmdev->shm_device.regions[1].size = vmdev->cfg_mem.size;

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

	vmdev->shm_io = metal_device_io_region(device, 0);
	if (!vmdev->shm_io) {
		metal_log(METAL_LOG_ERROR, "metal_device_io_region failed to get region 0");
		return err;
	}

	vmdev->cfg_io = metal_device_io_region(device, 1);
	if (!vmdev->cfg_io) {
		metal_log(METAL_LOG_ERROR, "metal_device_io_region failed to get region 1");
		return err;
	}

	return 0;
}

uint32_t virtio_mmio_get_max_elem(struct virtio_device *vdev, int idx)
{
	/* Select the queue we're interested in by writing selection register
	 * VIRTIO_MMIO_QUEUE_SEL. In pure AMP mode this needs to be followed by a
	 * synchronization w/ the device before reading VIRTIO_MMIO_QUEUE_NUM_MAX
	 */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_SEL, idx);
	return virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_NUM_MAX);
}

int virtio_mmio_device_init(struct virtio_mmio_device *vmdev, uintptr_t virt_mem_ptr,
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
	if (magic != VIRTIO_MMIO_MAGIC_VALUE_STRING) {
		metal_log(METAL_LOG_ERROR, "Bad magic value %08x\n", magic);
		return -1;
	}

	version = virtio_mmio_read32(vdev, VIRTIO_MMIO_VERSION);
	devid = virtio_mmio_read32(vdev, VIRTIO_MMIO_DEVICE_ID);
	if (devid == 0) {
		/* Placeholder */
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
	struct virtio_mmio_device *vmdev = metal_container_of(vdev,
							      struct virtio_mmio_device, vdev);

	if (vdev->role != (unsigned int)VIRTIO_DEV_DRIVER) {
		metal_log(METAL_LOG_ERROR, "Only VIRTIO_DEV_DRIVER is currently supported\n");
		return NULL;
	}

	if (!vq) {
		metal_log(METAL_LOG_ERROR,
			  "Only preallocated virtqueues are currently supported\n");
		return NULL;
	}

	if (vdev->id.version != 0x1) {
		metal_log(METAL_LOG_ERROR,
			  "Only VIRTIO MMIO version 1 is currently supported\n");
		return NULL;
	}

	vring_info->io = vmdev->shm_io;
	vring_info->info.num_descs = virtio_mmio_get_max_elem(vdev, idx);
	vring_info->info.align = VIRTIO_MMIO_VRING_ALIGNMENT;

	/* Check if vrings are already configured */
	if (vq->vq_nentries != 0 && vq->vq_nentries == vq->vq_free_cnt &&
	    vq->vq_ring.desc) {
		vring_info->info.vaddr = vq->vq_ring.desc;
		vring_info->vq = vq;
	}
	vring_info->info.num_descs = vq->vq_nentries;

	vq->vq_dev = vdev;

	vring_alloc_info = &vring_info->info;

	unsigned int role_bk = vdev->role;
	/* Assign OA VIRTIO_DEV_DRIVER role to allow virtio guests to setup the vrings */
	vdev->role = (unsigned int)VIRTIO_DEV_DRIVER;
	if (virtqueue_create(vdev, idx, vq_name, vring_alloc_info, (void (*)(struct virtqueue *))cb,
			     vdev->func->notify, vring_info->vq)) {
		metal_log(METAL_LOG_ERROR, "virtqueue_create failed\n");
		return NULL;
	}
	vdev->role = role_bk;
	vq->priv = cb_arg;
	virtqueue_set_shmem_io(vq, vmdev->shm_io);

	/* Writing selection register VIRTIO_MMIO_QUEUE_SEL. In pure AMP
	 * mode this needs to be followed by a synchronization w/ the device
	 * before reading VIRTIO_MMIO_QUEUE_NUM_MAX
	 */
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_SEL, idx);
	maxq = virtio_mmio_read32(vdev, VIRTIO_MMIO_QUEUE_NUM_MAX);
	VIRTIO_ASSERT((maxq != 0),
		      "VIRTIO_MMIO_QUEUE_NUM_MAX cannot be 0");
	VIRTIO_ASSERT((maxq >= vq->vq_nentries),
		      "VIRTIO_MMIO_QUEUE_NUM_MAX must be greater than vqueue->vq_nentries");
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_NUM, vq->vq_nentries);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_ALIGN, 4096);
	virtio_mmio_write32(vdev, VIRTIO_MMIO_QUEUE_PFN,
			    ((uintptr_t)metal_io_virt_to_phys(vq->shm_io,
			    (char *)vq->vq_ring.desc)) / 4096);

	vdev->vrings_info[vdev->vrings_num].vq = vq;
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
				vq->callback(vq->priv);
		}
	}

	if (isr & ~(VIRTIO_MMIO_INT_VRING))
		metal_log(METAL_LOG_WARNING, "Unhandled interrupt type: 0x%x\n", isr);

	virtio_mmio_write32(vdev, VIRTIO_MMIO_INTERRUPT_ACK, isr);
}

static int virtio_mmio_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
					 unsigned int nvqs, const char *names[],
					 vq_callback callbacks[], void *callback_args[])
{
	struct virtqueue *vq;
	struct virtqueue *vring_vq;
	void (*cb)(void *);
	void *cb_arg;
	unsigned int i;

	(void)flags;

	if (!vdev || !names || !vdev->vrings_info)
		return -EINVAL;

	for (i = 0; i < nvqs; i++) {
		vring_vq = NULL;
		cb = NULL;
		cb_arg = NULL;
		if (vdev->vrings_info[i].vq)
			vring_vq = vdev->vrings_info[i].vq;
		if (callbacks)
			cb = (virtio_mmio_vq_callback)callbacks[i];
		if (callback_args)
			cb_arg = callback_args[i];
		vq = virtio_mmio_setup_virtqueue(vdev, i, vring_vq, cb, cb_arg, names[i]);
		if (!vq)
			return -ENODEV;
	}

	return 0;
}
