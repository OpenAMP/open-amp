/*
 * Copyright (c) 2023, STMicroelectronics
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef OPENAMP_VIRTIO_MMIO_DEV_H
#define OPENAMP_VIRTIO_MMIO_DEV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <metal/utilities.h>
#include <metal/device.h>
#include <openamp/virtio_mmio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define an Empty MMIO register table with only the strict necessary
 * for a driver to recognise the device
 *
 * @note The initial state NOT_READY is the current approach to block the
 * device side usage from driver until it gets properly configured.
 */
#define EMPTY_MMIO_TABLE {					\
	.magic = VIRTIO_MMIO_MAGIC_VALUE_STRING,		\
	.version = 2,						\
	.status = VIRTIO_CONFIG_STATUS_NOT_READY,		\
}

/**
 * @brief MMIO Device Registers: 256 bytes in practice, more if the configuration space is used.
 * This is a trimmed down version with only the essential values needed to be detected correctly.
 */
struct mmio_table {

	/* 0x00 R should be 0x74726976 */
	uint32_t magic;

	/* 0x04 R */
	uint32_t version;

	/* padding */
	uint32_t padding[26];

	/* 0x70 RW Writing non-zero values to this register sets the status flags,
	 * indicating the driver progress.
	 * Writing zero (0x0) to this register triggers a device reset.
	 */
	uint32_t status;
};

/**
 * @brief This is called when the other side should be notifyed
 *
 * @param dev The device that need to notify the other side
 */
typedef void (*virtio_notify)(struct virtio_device *dev);

/**
 * @brief VirtIO mmio dev instance, should be init with mmio_dev_init,
 * then the VirtIO driver should set it's data using mmio_dev_set_device_data
 *
 * @param vdev VirtIO device instance
 * @param vring_size Number of descriptors per ring
 * @param vqs Array of virtqueues
 * @param io Metal IO Region used to access the MMIO registers
 * @param notify Called when an interrupt should be sent to the other side
 * @param device_features The features supported by this device
 * @param driver_features The features supported by the driver from the other side
 */
struct virtio_mmio_dev {
	struct virtio_device vdev;
	int vring_size;
	struct virtqueue *vqs;
	struct metal_io_region *io;
	virtio_notify notify;
	uint64_t device_features;
	uint64_t driver_features;
};

/**
 * @brief This should be called to initialize a virtio mmio device,
 * the configure function should be called next by the device driver
 *
 * @param dev The device to initialize
 * @param io The memory region in wich the device should operate
 * @param callback The callback that will be called when the other side should be notifyed
 */
void mmio_dev_init(struct virtio_mmio_dev *dev, struct metal_io_region *io, virtio_notify callback);

/**
 * @brief Should be called by the app when it receive an interrupt for the mmio device
 *
 * @param dev The virtio mmio device
 */
void mmio_dev_interrupt(struct virtio_mmio_dev *dev);

#ifdef __cplusplus
}
#endif

#endif /* OPENAMP_VIRTIO_MMIO_DEV_H */
