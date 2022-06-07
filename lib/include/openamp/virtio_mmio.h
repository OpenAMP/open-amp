/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 * Based on Virtio PCI driver by Anthony Liguori, copyright IBM Corp. 2007
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef OPENAMP_VIRTIO_MMIO_H
#define OPENAMP_VIRTIO_MMIO_H

#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Control registers */

/* Magic value ("virt" string) - Read Only */
#define VIRTIO_MMIO_MAGIC_VALUE		0x000

#define VIRTIO_MMIO_MAGIC_VALUE_STRING ('v' | ('i' << 8) | ('r' << 16) | ('t' << 24))

/* Virtio device version - Read Only */
#define VIRTIO_MMIO_VERSION		0x004

/* Virtio device ID - Read Only */
#define VIRTIO_MMIO_DEVICE_ID		0x008

/* Virtio vendor ID - Read Only */
#define VIRTIO_MMIO_VENDOR_ID		0x00c

/*
 * Bitmask of the features supported by the device (host)
 * (32 bits per set) - Read Only
 */
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010

/* Device (host) features set selector - Write Only */
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL	0x014

/*
 * Bitmask of features activated by the driver (guest)
 * (32 bits per set) - Write Only
 */
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020

/* Activated features set selector - Write Only */
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL	0x024

#ifndef VIRTIO_MMIO_NO_LEGACY /* LEGACY DEVICES ONLY! */
/* Guest's memory page size in bytes - Write Only */
#define VIRTIO_MMIO_GUEST_PAGE_SIZE	0x028
#endif

/* Queue selector - Write Only */
#define VIRTIO_MMIO_QUEUE_SEL		0x030

/* Maximum size of the currently selected queue - Read Only */
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034

/* Queue size for the currently selected queue - Write Only */
#define VIRTIO_MMIO_QUEUE_NUM		0x038

#ifndef VIRTIO_MMIO_NO_LEGACY /* LEGACY DEVICES ONLY! */
/* Used Ring alignment for the currently selected queue - Write Only */
#define VIRTIO_MMIO_QUEUE_ALIGN		0x03c
/* Guest's PFN for the currently selected queue - Read Write */
#define VIRTIO_MMIO_QUEUE_PFN		0x040
#endif

/* Ready bit for the currently selected queue - Read Write */
#define VIRTIO_MMIO_QUEUE_READY		0x044

/* Queue notifier - Write Only */
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050

/* Interrupt status - Read Only */
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060

/* Interrupt acknowledge - Write Only */
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064

/* Device status register - Read Write */
#define VIRTIO_MMIO_STATUS		0x070

/* Selected queue's Descriptor Table address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_DESC_LOW	0x080
#define VIRTIO_MMIO_QUEUE_DESC_HIGH	0x084

/* Selected queue's Available Ring address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW	0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH	0x094

/* Selected queue's Used Ring address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_USED_LOW	0x0a0
#define VIRTIO_MMIO_QUEUE_USED_HIGH	0x0a4

/* Shared memory region id */
#define VIRTIO_MMIO_SHM_SEL             0x0ac

/* Shared memory region length, 64 bits in two halves */
#define VIRTIO_MMIO_SHM_LEN_LOW         0x0b0
#define VIRTIO_MMIO_SHM_LEN_HIGH        0x0b4

/* Shared memory region base address, 64 bits in two halves */
#define VIRTIO_MMIO_SHM_BASE_LOW        0x0b8
#define VIRTIO_MMIO_SHM_BASE_HIGH       0x0bc

/* Configuration atomicity value */
#define VIRTIO_MMIO_CONFIG_GENERATION	0x0fc

/*
 * The config space is defined by each driver as
 * the per-driver configuration space - Read Write
 */
#define VIRTIO_MMIO_CONFIG		0x100

/* Interrupt flags (re: interrupt status & acknowledge registers) */
#define VIRTIO_MMIO_INT_VRING		(1 << 0)
#define VIRTIO_MMIO_INT_CONFIG		(1 << 1)

/**
 * @cond INTERNAL_HIDDEN
 *
 * For internal use only, skip these in public documentation.
 */

/*
 * The maximum virtqueue size is 2^15. Use that value as the end of
 * descriptor chain terminator since it will never be a valid index
 * in the descriptor table. This is used to verify we are correctly
 * handling vq_free_cnt.
 */
#define VQ_RING_DESC_CHAIN_END 32768

#define VIRTIO_MMIO_MAX_DATA_SIZE 128
#define VRING_ALIGNMENT           4096

#define VRING_SIZE(n, align) \
	(( \
		( \
		sizeof(struct vring_desc) * n + \
		sizeof(struct vring_avail) + \
		sizeof(uint16_t) * (n + 1) + \
		align - 1 \
		) \
		& ~(align - 1) \
	) + \
	sizeof(struct vring_used) + \
	sizeof(struct vring_used_elem) * n + sizeof(uint16_t))

#define VRING_DECLARE(name, n, align) \
static char __vrbuf_##name[VRING_SIZE(n, align)] __aligned(4096); \
static struct vring __vring_##name = { \
	.desc = (void *)__vrbuf_##name, \
	.avail = (void *)((unsigned long)__vrbuf_##name + n * sizeof(struct vring_desc)), \
	.used = (void *)((unsigned long)__vrbuf_##name + ((n * sizeof(struct vring_desc) + \
		(n + 1) * sizeof(uint16_t) + align - 1) & ~(align - 1))), \
}

/**
 * @endcond
 */

/** @brief Declare a virtqueue structure.
 *
 * @param[in] name The name of the virtqueue structure.
 * @param[in] n Size of the virtqueue. Must be a power of 2.
 * @param[in] align Memory alignment of the associated vring structures.
 * @note TODO: look into MMIO_QUEUE_ALIGN
 */

#define VQ_DECLARE(name, n, align) \
	static char __vrbuf_##name[VRING_SIZE(n, align)] __aligned(4096); \
	static struct { \
	struct virtqueue vq; \
	struct vq_desc_extra extra[n]; \
	} __vq_wrapper_##name = { \
	.vq = { \
		.vq_nentries = n, \
		.vq_ring = { \
		.desc = (void *)__vrbuf_##name, \
				.avail = (void *)((unsigned long)__vrbuf_##name + \
					n * sizeof(struct vring_desc)), \
				.used = (void *)((unsigned long)__vrbuf_##name + \
					((n * sizeof(struct vring_desc) + \
					(n + 1) * sizeof(uint16_t) + align - 1) & ~(align - 1))), \
		}, \
		.vq_queued_cnt = 0, \
		.vq_free_cnt = n, \
	}, \
	} \
	/**< @hideinitializer */

/** @brief Retrieve a pointer to the virtqueue structure declared with VQ_DECLARE().
 * @details Use this for creating the virtqueue array required by virtio_register_device().
 *
 * @param[in] name The name of the virtqueue structure.
 *
 * @return A pointer to the virtqueue structure.
 */

#define VQ_PTR(name) \
	(&__vq_wrapper_##name.vq) \
	/**< @hideinitializer */

struct virtio_buf {
	unsigned int index;
	unsigned int out_num;
	unsigned int in_num;
	struct virtqueue_buf *buffers;
};

struct virtio_device_mem {
	void *base;
	size_t size;
};

struct virtio_mmio_device {
	struct virtio_device vdev;
	struct metal_io_region *cfg_io;
	struct metal_io_region *shm_io;
	struct metal_device shm_device;
	struct virtio_device_mem cfg_mem;
	struct virtio_device_mem shm_mem;
	unsigned int device_mode;
	unsigned int irq;
	void *user_data;
};

/**
 * @brief Get device ID.
 *
 * @param[in] dev Pointer to device structure.
 *
 * @return Device ID value.
 */

inline uint32_t virtio_get_devid(const struct virtio_device *vdev)
{
	if (!vdev)
		return 0;
	return vdev->id.device;
}

/**
 * @brief Register a VIRTIO device with the VIRTIO stack.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] vq_num Number of virtqueues the device uses.
 * @param[in] vqs Array of pointers to vthe virtqueues used by the device.
 *
 * @return N/A.
 */

void virtio_mmio_register_device(struct virtio_device *vdev, int vq_num, struct virtqueue **vqs);

/**
 * @brief Setup a virtqueue structure.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] idx Index of the virtqueue.
 * @param[in] vq Pointer to virtqueue structure.
 * @param[in] cb Pointer to virtqueue callback. Can be NULL.
 * @param[in] cb_arg Argument for the virtqueue callback.
 *
 * @return pointer to virtqueue structure.
 */

struct virtqueue *virtio_mmio_setup_virtqueue(struct virtio_device *vdev,
					      unsigned int idx,
					      struct virtqueue *vq,
					      void (*cb)(void *),
					      void *cb_arg,
					      const char *vq_name);

/**
 * @brief VIRTIO MMIO device initialization.
 *
 * @param[in] vmdev Pointer to virtio_mmio_device structure.
 * @param[in] virt_mem_ptr Guest virtio (shared) memory base address (virtual).
 * @param[in] cfg_mem_ptr Virtio device configuration memory base address (virtual).
 * @param[in] user_data Pointer to custom user data.
 *
 * @return int 0 for success.
 */

int virtio_mmio_device_init(struct virtio_mmio_device *vmdev, uintptr_t virt_mem_ptr,
		       uintptr_t cfg_mem_ptr, void *user_data);

/**
 * @brief VIRTIO MMIO interrupt service routine.
 *
 * @param[in] vdev Pointer to virtio_device structure.
 *
 * @return N/A.
 */

void virtio_mmio_isr(struct virtio_device *vdev);

#ifdef __cplusplus
}
#endif

#endif /* OPENAMP_VIRTIO_MMIO_H */
