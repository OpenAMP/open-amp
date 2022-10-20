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

/* VirtIO device IDs. */
#define VIRTIO_ID_NETWORK    0x01UL
#define VIRTIO_ID_BLOCK      0x02UL
#define VIRTIO_ID_CONSOLE    0x03UL
#define VIRTIO_ID_ENTROPY    0x04UL
#define VIRTIO_ID_BALLOON    0x05UL
#define VIRTIO_ID_IOMEMORY   0x06UL
#define VIRTIO_ID_RPMSG      0x07UL /* remote processor messaging */
#define VIRTIO_ID_SCSI       0x08UL
#define VIRTIO_ID_9P         0x09UL
#define VIRTIO_ID_SCMI       0x20UL
#define VIRTIO_DEV_ANY_ID    (-1)UL

/* Status byte for guest to report progress. */
#define VIRTIO_CONFIG_STATUS_ACK       0x01
#define VIRTIO_CONFIG_STATUS_DRIVER    0x02
#define VIRTIO_CONFIG_STATUS_DRIVER_OK 0x04
#define VIRTIO_CONFIG_FEATURES_OK      0x08
#define VIRTIO_CONFIG_STATUS_NEEDS_RESET 0x40
#define VIRTIO_CONFIG_STATUS_FAILED    0x80

/* Virtio device role */
#define VIRTIO_DEV_DRIVER	0UL
#define VIRTIO_DEV_DEVICE	1UL

#define VIRTIO_DEV_MASTER	deprecated_virtio_dev_master()
#define VIRTIO_DEV_SLAVE	deprecated_virtio_dev_slave()

__deprecated static inline int deprecated_virtio_dev_master(void)
{
	/* "VIRTIO_DEV_MASTER is deprecated, please use VIRTIO_DEV_DRIVER" */
	return VIRTIO_DEV_DRIVER;
}

__deprecated static inline int deprecated_virtio_dev_slave(void)
{
	/* "VIRTIO_DEV_SLAVE is deprecated, please use VIRTIO_DEV_DEVICE" */
	return VIRTIO_DEV_DEVICE;
}

#ifdef VIRTIO_MASTER_ONLY
#define VIRTIO_DRIVER_ONLY
#warning "VIRTIO_MASTER_ONLY is deprecated, please use VIRTIO_DRIVER_ONLY"
#endif

#ifdef VIRTIO_SLAVE_ONLY
#define VIRTIO_DEVICE_ONLY
#warning "VIRTIO_SLAVE_ONLY is deprecated, please use VIRTIO_DEVICE_ONLY"
#endif

struct virtio_device_id {
	uint32_t device;
	uint32_t vendor;
	uint32_t version;
};

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

/*
 * Version 1 compliant
 */
#define VIRTIO_F_VERSION_1 32

/*
 * The device can be used on a platform where device access to data
 * in memory is limited and/or translated.
 */
#define VIRTIO_F_ACCESS_PLATFORM      33

/*
 * Packed virtqueue layout
 */
#define VIRTIO_F_RING_PACKED          34

#ifdef VIRTIO_DEBUG
#include <metal/log.h>

#define VIRTIO_ASSERT(_exp, _msg) do { \
		if (!(_exp)) { \
			metal_log(METAL_LOG_EMERGENCY, \
				  "FATAL: %s - "_msg, __func__); \
			metal_assert(_exp); \
		} \
	} while (0)
#else
#define VIRTIO_ASSERT(_exp, _msg) metal_assert(_exp)
#endif /* VIRTIO_DEBUG */

typedef void (*virtio_dev_reset_cb)(struct virtio_device *vdev);

struct virtio_dispatch;

struct virtio_feature_desc {
	uint32_t vfd_val;
	const char *vfd_str;
};

/**
 * struct virtio_vring_info
 * @vq virtio queue
 * @info vring alloc info
 * @notifyid vring notify id
 * @io metal I/O region of the vring memory, can be NULL
 */
struct virtio_vring_info {
	struct virtqueue *vq;
	struct vring_alloc_info info;
	uint32_t notifyid;
	struct metal_io_region *io;
};

/*
 * Structure definition for virtio devices for use by the
 * applications/drivers
 */

struct virtio_device {
	uint32_t notifyid; /**< unique position on the virtio bus */
	struct virtio_device_id id; /**< the device type identification
				      *  (used to match it with a driver
				      */
	uint64_t features; /**< the features supported by both ends. */
	unsigned int role; /**< if it is virtio backend or front end. */
	virtio_dev_reset_cb reset_cb; /**< user registered device callback */
	const struct virtio_dispatch *func; /**< Virtio dispatch table */
	void *priv; /**< TODO: remove pointer to virtio_device private data */
	unsigned int vrings_num; /**< number of vrings */
	struct virtio_vring_info *vrings_info;
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

struct virtio_dispatch {
	uint8_t (*get_status)(struct virtio_device *dev);
	void (*set_status)(struct virtio_device *dev, uint8_t status);
	uint32_t (*get_features)(struct virtio_device *dev);
	void (*set_features)(struct virtio_device *dev, uint32_t feature);
	uint32_t (*negotiate_features)(struct virtio_device *dev,
				       uint32_t features);

	/*
	 * Read/write a variable amount from the device specific (ie, network)
	 * configuration region. This region is encoded in the same endian as
	 * the guest.
	 */
	void (*read_config)(struct virtio_device *dev, uint32_t offset,
			    void *dst, int length);
	void (*write_config)(struct virtio_device *dev, uint32_t offset,
			     void *src, int length);
	void (*reset_device)(struct virtio_device *dev);
	void (*notify)(struct virtqueue *vq);
};

int virtio_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
			     unsigned int nvqs, const char *names[],
			     vq_callback callbacks[]);

/**
 * @brief Retrieve device status.
 *
 * @param[in] dev Pointer to device structure.
 *
 * @return status of the device.
 */

inline uint8_t virtio_device_get_status(struct virtio_device *vdev)
{
	return vdev->func->get_status(vdev);
}

/**
 * @brief Set device status.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] status Value to be set as device status.
 *
 * @return N/A.
 */

inline void virtio_device_set_status(struct virtio_device *vdev, uint8_t status)
{
	vdev->func->set_status(vdev, status);
}

/**
 * @brief Retrieve configuration data from the device.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] offset Offset of the data within the configuration area.
 * @param[in] dst Address of the buffer that will hold the data.
 * @param[in] len Length of the data to be retrieved.
 *
 * @return N/A.
 */

inline void virtio_device_read_config(struct virtio_device *vdev,
		 uint32_t offset, void *dst, int length)
{
	vdev->func->read_config(vdev, offset, dst, length);
}

/**
 * @brief Write configuration data to the device.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] offset Offset of the data within the configuration area.
 * @param[in] src Address of the buffer that holds the data to write.
 * @param[in] len Length of the data to be written.
 *
 * @return N/A.
 */
inline void virtio_device_write_config(struct virtio_device *vdev,
		 uint32_t offset, void *src, int length)
{
	vdev->func->write_config(vdev, offset, src, length);
}

/**
 * @brief Retrieve features supported by both the VIRTIO driver and the VIRTIO device.
 *
 * @param[in] dev Pointer to device structure.
 *
 * @return Features supported by both the driver and the device as a bitfield.
 */

inline uint32_t virtio_device_get_features(struct virtio_device *vdev)
{
	return vdev->func->get_features(vdev);
}

/**
 * @brief Set features supported by the VIRTIO driver.
 *
 * @param[in] dev Pointer to device structure.
 * @param[in] features Features supported by the driver as a bitfield.
 *
 * @return N/A.
 */

inline void virtio_device_set_features(struct virtio_device *vdev, uint32_t features)
{
	return vdev->func->set_features(vdev, features);
}

/**
 * @brief Reset virtio device.
 * @param[in] vdev Pointer to virtio_device structure.
 * @return N/A.
 */
inline void virtio_device_reset(struct virtio_device *vdev)
{
	vdev->func->reset_device(vdev);
}

#if defined __cplusplus
}
#endif

#endif				/* _VIRTIO_H_ */
