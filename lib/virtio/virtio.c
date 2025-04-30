/*-
 * Copyright (c) 2011, Bryan Venteicher <bryanv@FreeBSD.org>
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <openamp/virtio.h>

/*
 * TODO :
 * This structure may change depending on the types of devices we support.
 */
static const struct virtio_ident {
	unsigned short devid;
	const char *name;
} virtio_ident_table[] = {
	{
	VIRTIO_ID_NETWORK, "Network"}, {
	VIRTIO_ID_BLOCK, "Block"}, {
	VIRTIO_ID_CONSOLE, "Console"}, {
	VIRTIO_ID_ENTROPY, "Entropy"}, {
	VIRTIO_ID_BALLOON, "Balloon"}, {
	VIRTIO_ID_IOMEMORY, "IOMemory"}, {
	VIRTIO_ID_SCSI, "SCSI"}, {
	VIRTIO_ID_9P, "9P Transport"}, {
	VIRTIO_ID_MAC80211_WLAN, "MAC80211 WLAN"}, {
	VIRTIO_ID_RPROC_SERIAL, "Remoteproc Serial"}, {
	VIRTIO_ID_GPU, "GPU"}, {
	VIRTIO_ID_INPUT, "Input"}, {
	VIRTIO_ID_VSOCK, "Vsock Transport"}, {
	VIRTIO_ID_SOUND, "Sound"}, {
	VIRTIO_ID_FS, "File System"}, {
	VIRTIO_ID_MAC80211_HWSIM, "MAC80211 HWSIM"}, {
	VIRTIO_ID_I2C_ADAPTER, "I2C Adapter"}, {
	VIRTIO_ID_BT, "Bluetooth"}, {
	VIRTIO_ID_GPIO, "GPIO" }, {
	0, NULL}
};

const char *virtio_dev_name(unsigned short devid)
{
	const struct virtio_ident *ident;

	for (ident = virtio_ident_table; ident->name; ident++) {
		if (ident->devid == devid)
			return ident->name;
	}

	return NULL;
}

__deprecated void virtio_describe(struct virtio_device *dev, const char *msg,
				  uint32_t features, struct virtio_feature_desc *desc)
{
	(void)dev;
	(void)msg;
	(void)features;
	(void)desc;
}

int virtio_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
			     unsigned int nvqs, const char *names[],
			     vq_callback callbacks[], void *callback_args[])
{
	struct virtio_vring_info *vring_info;
	struct vring_alloc_info *vring_alloc;
	unsigned int num_vrings, i;
	int ret;
	(void)flags;

	if (!vdev)
		return -EINVAL;

	if (vdev->func && vdev->func->create_virtqueues) {
		return vdev->func->create_virtqueues(vdev, flags, nvqs,
						     names, callbacks, callback_args);
	}

	num_vrings = vdev->vrings_num;
	if (nvqs > num_vrings)
		return ERROR_VQUEUE_INVLD_PARAM;
	/* Initialize virtqueue for each vring */
	for (i = 0; i < nvqs; i++) {
		vring_info = &vdev->vrings_info[i];

		vring_alloc = &vring_info->info;
		if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
			size_t offset;
			struct metal_io_region *io = vring_info->io;

			offset = metal_io_virt_to_offset(io,
							 vring_alloc->vaddr);
			metal_io_block_set(io, offset, 0,
					   vring_size(vring_alloc->num_descs,
						      vring_alloc->align));
		}
		ret = virtqueue_create(vdev, i, names[i], vring_alloc,
				       callbacks[i], vdev->func->notify,
				       vring_info->vq);
		if (ret)
			return ret;
	}
	return 0;
}

