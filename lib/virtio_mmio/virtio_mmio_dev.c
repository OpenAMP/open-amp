/*
 * Copyright (c) 2023, STMicroelectronics
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/virtio_mmio.h>

static inline uint32_t virtio_mmio_dev_read_reg(struct virtio_mmio_dev *dev, uint32_t address)
{
	return metal_io_read32(dev->io, address);
}

static inline void virtio_mmio_dev_write_reg(struct virtio_mmio_dev *dev, uint32_t address, uint32_t value)
{
	metal_io_write32(dev->io, address, value);
}

static void virtio_mmio_dev_notify_driver(struct virtqueue *vq)
{
	struct virtio_mmio_dev *dev = metal_container_of(vq->vq_dev, struct virtio_mmio_dev, vdev);

	if (!dev->notify)
		return;

	/* we set the interrupt status to 1 to tell that a buffer was used */
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_INTERRUPT_STATUS, 1);

	dev->notify(&dev->vdev);
}

static int virtio_mmio_dev_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
				      unsigned int nvqs, const char **names, vq_callback *callbacks)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);
	struct virtio_vring_info *vring_info;

	/* this should be the same number as in configure */
	if (nvqs != vdev->vrings_num)
		return ERROR_VQUEUE_INVLD_PARAM;

	for (int i = 0; i < nvqs; i++) {
		vring_info = &vdev->vrings_info[i];

		/* if this is null it means that the virtqueue was not sent by the other side */
		if (!vring_info->vq)
			return ERROR_VQUEUE_INVLD_PARAM;

		/* we set all infos needed by the virtqueue */
		vring_info->vq->callback = callbacks[i];
		vring_info->vq->vq_name = names[i];
		vring_info->vq->notify = virtio_mmio_dev_notify_driver;
	}

	return 0;
}

/* Since the virtqueues are not actually instantiated in the create virtqueues function,
 * maybe we should free the allocs from configure device ?
 */
static void virtio_mmio_dev_delete_virtqueues(struct virtio_device *vdev)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	if (!vdev->vrings_num)
		return;

	/* we are deleting virtqueues so we reset the device at the same time */
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_STATUS, VIRTIO_CONFIG_STATUS_NOT_READY);

	vdev->vrings_num = 0;
	dev->vring_size = 0;
	metal_free_memory(dev->vqs);
	metal_free_memory(vdev->vrings_info);
}

static int virtio_mmio_dev_configure_device(struct virtio_device *vdev, uint64_t device_features,
				     uint32_t device_type, int nvqs, int vring_size)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);
	int ret = ERROR_NO_MEM;

	/* Set virtio features supported by the device */
	dev->device_features = device_features;

	/* we preallocate our virtqueues and vring_infos */
	dev->vqs = metal_allocate_memory(nvqs * sizeof(struct virtqueue));

	if (!dev->vqs)
		return ret;

	if (!vdev->vrings_info)
		vdev->vrings_info = metal_allocate_memory(nvqs * sizeof(struct virtio_vring_info));

	if (!vdev->vrings_info)
		goto free_vqs;

	/* save sizes in ours devs */
	dev->vring_size = vring_size;
	vdev->vrings_num = nvqs;
	vdev->id.device = device_type;

	/* tell the other side the max number of vring allowed */
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_QUEUE_NUM_MAX, vring_size);

	/* Set the type of driver needed by the guest */
	virtio_mmio_dev_write_reg(dev->io, VIRTIO_MMIO_DEVICE_ID, device_type);

	/* Allow the other side to init the mmio device */
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_DEVICE_ID, device_type);
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_STATUS,
		  (uint32_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_STATUS) & ~VIRTIO_CONFIG_STATUS_NOT_READY);

	return 0;

free_vqs:
	metal_free_memory(dev->vqs);
	return ret;
}

static uint8_t virtio_mmio_dev_get_status(struct virtio_device *vdev)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	return (uint8_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_STATUS);
}

static void virtio_mmio_dev_set_status(struct virtio_device *vdev, uint8_t status)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_STATUS, status);
}

static uint64_t virtio_mmio_dev_get_features(struct virtio_device *vdev)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	return dev->device_features;
}

static void virtio_mmio_dev_set_features(struct virtio_device *vdev, uint32_t feature)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	dev->device_features = feature;
}

static void virtio_mmio_dev_read_config(struct virtio_device *vdev, uint32_t offset, void *dst,
				 int length)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);
	uint8_t *buf = dst;

	/* TODO: replace to metal_io_block_read, it also need a sanity check regarding
	 * the offset and length of block read
	 */
	for (int i = 0; i < length; i++)
		buf[i] = metal_io_read8(dev->io, VIRTIO_MMIO_CONFIG + i);
}

static void virtio_mmio_dev_write_config(struct virtio_device *vdev, uint32_t offset, void *src,
				  int length)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);
	uint8_t *buf = src;

	/* TODO: replace to metal_io_block_write, it also need a sanity check regarding
	 * the offset and length of block being written
	 */
	for (int i = 0; i < length; i++)
		metal_io_write8(dev->io, VIRTIO_MMIO_CONFIG + i, buf[i]);
}

static void virtio_mmio_dev_reset_device(struct virtio_device *vdev)
{
	struct virtio_mmio_dev *dev = metal_container_of(vdev, struct virtio_mmio_dev, vdev);

	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_STATUS, virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_STATUS) |
						    VIRTIO_CONFIG_STATUS_NEEDS_RESET);
}

const static struct virtio_dispatch virtio_ops = {

	.create_virtqueues = virtio_mmio_dev_create_virtqueues,
	.delete_virtqueues = virtio_mmio_dev_delete_virtqueues,

	.configure_device = virtio_mmio_dev_configure_device,

	.get_status = virtio_mmio_dev_get_status,
	.set_status = virtio_mmio_dev_set_status,

	.get_features = virtio_mmio_dev_get_features,
	.set_features = virtio_mmio_dev_set_features,

	.read_config = virtio_mmio_dev_read_config,
	.write_config = virtio_mmio_dev_write_config,

	.reset_device = virtio_mmio_dev_reset_device,
};

static void virtio_mmio_dev_negotiate_features(struct virtio_mmio_dev *dev)
{
	/* we update the device features each time in case the feature sel changed */
	virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_DEVICE_FEATURES, dev->device_features >>
		  (virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_DEVICE_FEATURES_SEL) * 32));

	/* we update the driver features each time in case the feature sel changed */
	dev->driver_features |= ((uint64_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_DRIVER_FEATURES) <<
				 (virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_DRIVER_FEATURES_SEL) * 32));
}

static void virtio_mmio_dev_receive_queues(struct virtio_mmio_dev *dev, struct virtio_device *vdev)
{
	/* Now the driver should send us the virtqueues */
	uint32_t queuesel = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_SEL);

	/* Update virtqueue registers according to the queuesel index */
	if (queuesel >= vdev->vrings_num)
		return;

	struct virtio_vring_info *vinfo = &vdev->vrings_info[queuesel];
	struct vring_alloc_info *alloc_info = &vinfo->info;
	struct virtqueue *vq = &dev->vqs[queuesel];

	/* if this queue already exists or the driver did not set it up */
	if (vinfo->vq || !virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_READY)) {
		/* tell the driver if the queue already exists or not */
		virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_QUEUE_READY, vinfo->vq ? 1 : 0);
		return;
	}

	/* If the host sets Queue Ready then we can read the virtqueue */
	alloc_info->num_descs = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_NUM);

	alloc_info->vaddr = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_DESC_LOW) |
			    ((uint64_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_DESC_HIGH) << 32);

	/* TODO: the alignment should be set in a config instead of being hardcoded */
	alloc_info->align = 4096;

	/* create an empty virtqueue that will be completed in create_virtqueues */
	virtqueue_create(vdev, queuesel, NULL, alloc_info, NULL, NULL, vq);

	vq->shm_io = dev->io;
	vinfo->vq = vq;

	/* Use the vring addresses provided in case they were aligned differently */
	vq->vq_ring.desc = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_DESC_LOW) |
			   ((uint64_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_DESC_HIGH) << 32);

	vq->vq_ring.avail = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_AVAIL_LOW) |
			    ((uint64_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_AVAIL_HIGH) << 32);

	vq->vq_ring.used = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_USED_LOW) |
			   ((uint64_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_USED_HIGH) << 32);
}

void virtio_mmio_dev_init(struct virtio_mmio_dev *dev, struct metal_io_region *io, virtio_mmio_notify callback)
{
	dev->io = io;
	dev->notify = callback;
	dev->vdev.func = &virtio_ops;

	dev->driver_features = 0;

	/* Init vdev struct */
	dev->vdev.role = VIRTIO_DEV_DEVICE;
}

void virtio_mmio_dev_interrupt(struct virtio_mmio_dev *dev)
{
	struct virtio_device *vdev = &dev->vdev;

	switch (virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_STATUS)) {
	case VIRTIO_CONFIG_STATUS_RESET:
		dev->driver_features = 0;
		break;

	/* Perform feature exchange:
	 * the features sel register are used to select the features bits we want to read
	 */
	case (VIRTIO_CONFIG_STATUS_ACK | VIRTIO_CONFIG_STATUS_DRIVER):
		virtio_mmio_dev_negotiate_features(dev);
		break;

	/* Check if the features negotiated are the right ones
	 * Receive the virtqueues
	 */
	case (VIRTIO_CONFIG_STATUS_ACK | VIRTIO_CONFIG_STATUS_DRIVER |
	      VIRTIO_CONFIG_STATUS_FEATURES_OK):
		/* if the features does not match then we should not allow feature ok status */
		if (dev->driver_features != dev->device_features) {
			virtio_mmio_dev_write_reg(dev, VIRTIO_MMIO_STATUS,
				  (uint32_t)virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_STATUS) &
				  ~VIRTIO_CONFIG_STATUS_FEATURES_OK);
			return;
		}
		/* The features are fully negotiated ! */
		vdev->features = dev->driver_features;
		virtio_mmio_dev_receive_queues(dev, vdev);
		break;

	case VIRTIO_CONFIG_STATUS_READY:
		uint32_t notify_idx = virtio_mmio_dev_read_reg(dev, VIRTIO_MMIO_QUEUE_NOTIFY);
		/* if the virtqueue does have an available buffer, notify it */
		if (notify_idx < vdev->vrings_num && vdev->vrings_info[notify_idx].vq &&
		    virtqueue_get_desc_size(&dev->vqs[notify_idx]) > 0)
			virtqueue_notification(&dev->vqs[notify_idx]);
		break;

	}
}
