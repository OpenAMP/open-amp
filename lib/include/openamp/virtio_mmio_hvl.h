/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef OPENAMP_VIRTIO_MMIO_HVL_H
#define OPENAMP_VIRTIO_MMIO_HVL_H

#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HVL_VIRTIO)

/*
 * To minimize the number of hardware notification channels used, in hypervirsorless virtio
 * deployments, a single multiplexed source of notifications is used for all the devices.
 * The callback framework allows individual device drivers, if needed, to register their
 * own callbacks (i.e. the equivalent of per-device ISRs)
 */

/* Max number of hypervisorless virtio callbacks */
#define VIRTIO_MMIO_HVL_CB_MAX 10

/*
 * For multi-virtqueue devices in hypervisorless mode, a custom configuration acknowledgment
 * mechanism is used to process same-register configuration entries like QUEUE_PFN. This is
 * the ACK value from the physical machine monitor for these configurations.
 */
#define VIRTIO_MMIO_HVL_CFG_ACK 0xAABBAABB

/* Hypervisorless virtio callback type */
typedef void (*virtio_mmio_hvl_cb_t)(void *);

/* Hypervisorless virtio inter-processor notification routine type */
typedef void (*virtio_mmio_hvl_ipi_t)(void *);

struct vq_bounce_buf {
	struct metal_list node;
	void *cookie;
	uint64_t addr;
	uint32_t len;
};

/**
 * @brief VIRTIO MMIO hypervisorless operation mode initialization
 *
 * @param[in] vdev Pointer to struct virtio_device.
 *
 * @return N/A.
 */

void virtio_mmio_hvl_init(struct virtio_device *vdev);

/**
 * @brief VIRTIO MMIO shared memory pool initialization routine.
 *
 * @param[in] mem Pointer to memory.
 * @param[in] size Size of memory region in bytes.
 *
 * @return N/A.
 */

void virtio_mmio_shm_pool_init(void *mem, size_t size);

/**
 * @brief VIRTIO MMIO shared memory pool buffer allocation routine.
 *
 * @param[in] size Number of bytes requested.
 *
 * @return pointer to allocated memory space in shared memory region.
 */

void *virtio_mmio_shm_pool_alloc(size_t size);

/**
 * @brief VIRTIO MMIO shared memory pool buffer deallocation routine.
 *
 * @param[in] ptr Pointer to memory space to free.
 *
 * @return N/A.
 */

void virtio_mmio_shm_pool_free(void *ptr);

/**
 * @brief VIRTIO MMIO (hypervisorless mode) inter-processor notification routine
 *
 * @return N/A.
 */

void virtio_mmio_hvl_ipi(void);

/**
 * @brief VIRTIO MMIO (hypervisorless mode) wait routine.
 *
 * @param[in] usec Number of microseconds to wait.
 *
 * @return N/A.
 */

void virtio_mmio_hvl_wait(uint32_t usec);

/**
 * @brief Add VIRTIO MMIO (hypervisorless mode) callback.
 *
 * @param[in] func Callback function pointer.
 * @param[in] arg  Callback parameter.
 *
 * @return int 0 for success.
 */

int virtio_mmio_hvl_cb_set(virtio_mmio_hvl_cb_t func, void *arg);

/**
 * @brief VIRTIO MMIO (hypervisorless mode) callback execution routine.
 *
 * @return N/A
 */

void virtio_mmio_hvl_cb_run(void);

/**
 * @brief Add VIRTIO MMIO (hypervisorless mode) shared-memory bounce buffer.
 *
 * In hypervisorless mode this routine transparently allocates a bounce buffer
 * to be enqueued in vring for consumption, copies the data in the original
 * buffer and  saves the original buffer information for dequeueing.
 * If the buffer is already in the pre-shared memory region, the bouncing
 * mechanism is not used.
 *
 * @param[in] vq     Pointer to VirtIO queue control block.
 * @param[in] cookie Pointer to hold call back data
 * @param[in] buffer Data buffer
 * @param[in] len    Data buffer length
 *
 * @return bounce buffer physical address or METAL_BAD_PHYS
 */

uint64_t virtio_mmio_hvl_add_bounce_buf(struct virtqueue *vq, void *cookie, char *buffer,
					unsigned int len);

/**
 * @brief Get used VIRTIO MMIO (hypervisorless mode) shared-memory bounce buffer.
 *
 * In hypervisorless mode this routine transparently copies data from a dequeued
 * bounce buffer in pre-shared memory to the original buffer location.
 *
 * @param[in] vq     Pointer to VirtIO queue control block.
 * @param[in] dp     Pointer to vring descriptor.
 * @param[in] cookie Pointer for callback data
 *
 * @return N/A
 */

void virtio_mmio_hvl_get_bounce_buf(struct virtqueue *vq, struct vring_desc *dp,
				    void *cookie);

/**
 * @brief Wait for configuration ack, i.e. value at offset to match data
 *
 * @return int 0 for success
 */

int virtio_mmio_hvl_wait_cfg(struct virtio_device *vdev, int offset, uint32_t data);

/**
 * @brief Set inter-processor notification routine
 *
 * @param[in] vdev     Pointer to virtio_device structure
 * @param[in] ipi_func Inter-processor notification routine
 * @param[in] param    Inter-processor notification routine parameter
 *
 * @return N/A
 */
void virtio_mmio_hvl_set_ipi(struct virtio_device *vdev, virtio_mmio_hvl_ipi_t ipi_func,
			     void *param);

#endif /* defined(HVL_VIRTIO) */

#ifdef __cplusplus
}
#endif

#endif /* OPENAMP_VIRTIO_MMIO_HVL_H */
