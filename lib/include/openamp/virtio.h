/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#ifndef _VIRTIO_H_
#define _VIRTIO_H_

#include <openamp/virtqueue.h>
#include <metal/spinlock.h>

#if defined __cplusplus
extern "C" {
#endif

/* TODO: define this as compiler flags */
#ifndef VIRTIO_MAX_NUM_VRINGS
#define VIRTIO_MAX_NUM_VRINGS 2
#endif

/* VirtIO device IDs. */
#define VIRTIO_ID_NETWORK    0x01
#define VIRTIO_ID_BLOCK      0x02
#define VIRTIO_ID_CONSOLE    0x03
#define VIRTIO_ID_ENTROPY    0x04
#define VIRTIO_ID_BALLOON    0x05
#define VIRTIO_ID_IOMEMORY   0x06
#define VIRTIO_ID_RPMSG	     0x07 /* virtio remote remote_proc messaging */
#define VIRTIO_ID_SCSI       0x08
#define VIRTIO_ID_9P         0x09

/* Status byte for guest to report progress. */
#define VIRTIO_CONFIG_STATUS_ACK       0x01
#define VIRTIO_CONFIG_STATUS_DRIVER    0x02
#define VIRTIO_CONFIG_STATUS_DRIVER_OK 0x04
#define VIRTIO_CONFIG_STATUS_NEEDS_RESET 0x40
#define VIRTIO_CONFIG_STATUS_FAILED    0x80

struct virtio_device_id {
	uint32_t device;
	uint32_t vendor;
};
#define VIRTIO_DEV_ANY_ID	((unsigned int)-1)

/*
 * Generate interrupt when the virtqueue ring is
 * completely used, even if we've suppressed them.
 */
#define VIRTIO_F_NOTIFY_ON_EMPTY (1 << 24)

/*
 * The guest should never negotiate this feature; it
 * is used to detect faulty drivers.
 */
#define VIRTIO_F_BAD_FEATURE (1 << 30)

/*
 * Some VirtIO feature bits (currently bits 28 through 31) are
 * reserved for the transport being used (eg. virtio_ring), the
 * rest are per-device feature bits.
 */
#define VIRTIO_TRANSPORT_F_START      28
#define VIRTIO_TRANSPORT_F_END        32

typedef struct _virtio_dispatch_ virtio_dispatch;

struct virtio_feature_desc {
	uint32_t vfd_val;
	const char *vfd_str;
};

/**
* struct proc_vring
*
* This structure is maintained by hardware interface layer to keep
* vring physical memory and notification info.
*
*/
struct virtio_vring_info {
	/* Vring logical address */
	void *vaddr;
	/* Vring I/O region */
	struct metal_io_region *io;
	/* Number of vring descriptors */
	unsigned short num_descs;
	/* Vring alignment */
	unsigned long align;
};

/**
 * struct proc_shm
 *
 * This structure is maintained by hardware interface layer for
 * shared memory information. The shared memory provides buffers
 * for use by the vring to exchange messages between the cores.
 *
 */
struct virtio_buffer_info {
	/* Start address of shared memory used for buffers. */
	void *vaddr;
	/* Start physical address of shared memory used for buffers. */
	metal_phys_addr_t paddr;
	/* sharmed memory I/O region */
	struct metal_io_region *io;
	/* Size of shared memory. */
	unsigned long size;
};

/*
 * Structure definition for virtio devices for use by the
 * applications/drivers
 */

struct virtio_device {
	int index; /**< unique position on the virtio bus */
	struct virtio_device_id id; /**< the device type identification
				         (used to match it with a driver). */
	struct metal_spinlock lock; /**< spin lock */
	uint64_t features; /**< the features supported by both ends. */
	unsigned int role; /**< if it is virtio backend or front end. */
	void (*rst_cb)(struct virtio_device *vdev); /**< user registered virtio
						         device callback */
	virtio_dispatch *func;/**< Virtio dispatch table */
	void *priv; /**< TODO: should remove pointer to virtio_device private
		         data */
	int vrings_num; /**< number of vrings */
	struct virtqueue vqs[1]; /**< array of virtqueues */
};

/*
 * Helper functions.
 */
const char *virtio_dev_name(uint16_t devid);
void virtio_describe(struct virtio_device *dev, const char *msg,
		     uint32_t features,
		     struct virtio_feature_desc *feature_desc);

/*
 * Functions for virtio device configuration as defined in Rusty Russell's
 * paper.
 * Drivers are expected to implement these functions in their respective codes.
 */

struct _virtio_dispatch_ {
	int (*create_virtqueues) (struct virtio_device * dev, int flags,
				  unsigned int nvqs, const char *names[],
				  vq_callback * callbacks[],
				  struct virtqueue * vqs[]);
	uint8_t(*get_status) (struct virtio_device * dev);
	void (*set_status) (struct virtio_device * dev, uint8_t status);
	uint32_t(*get_features) (struct virtio_device * dev);
	void (*set_features) (struct virtio_device * dev, uint32_t feature);
	uint32_t(*negotiate_features) (struct virtio_device * dev,
				       uint32_t features);

	/*
	 * Read/write a variable amount from the device specific (ie, network)
	 * configuration region. This region is encoded in the same endian as
	 * the guest.
	 */
	void (*read_config) (struct virtio_device * dev, uint32_t offset,
			     void *dst, int length);
	void (*write_config) (struct virtio_device * dev, uint32_t offset,
			      void *src, int length);
	void (*reset_device) (struct virtio_device * dev);

};

#if defined __cplusplus
}
#endif

#endif				/* _VIRTIO_H_ */
