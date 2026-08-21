/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * Copyright (c) 2018, Xilinx Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/io.h>
#include <metal/utilities.h>

#include "rsc_table_parser.h"

#define RSC_TAB_SUPPORTED_VERSION 1

/**
 * @internal
 *
 * @brief Carveout resource handler.
 *
 * @param rproc	Pointer to remote remoteproc
 * @param rsc	Pointer to carveout resource
 *
 * @return 0 for success, or negative value for failure
 */
static int handle_carve_out_rsc(struct remoteproc *rproc, void *rsc)
{
	struct fw_rsc_carveout *carve_rsc = rsc;
	metal_phys_addr_t da;
	metal_phys_addr_t pa;
	size_t size;
	unsigned int attribute;

	/* Validate resource fields */
	if (!carve_rsc) {
		return -RPROC_ERR_RSC_TAB_NP;
	}

	if (carve_rsc->reserved) {
		return -RPROC_ERR_RSC_TAB_RSVD;
	}
	pa = carve_rsc->pa;
	da = carve_rsc->da;
	size = carve_rsc->len;
	attribute = carve_rsc->flags;
	if (remoteproc_mmap(rproc, &pa, &da, size, attribute, NULL))
		return 0;
	else
		return -RPROC_EINVAL;
}

/**
 * @internal
 *
 * @brief Trace resource handler.
 *
 * @param rproc	Pointer to remote remoteproc
 * @param rsc	Pointer to trace resource
 *
 * @return No service error
 */
static int handle_trace_rsc(struct remoteproc *rproc, void *rsc)
{
	struct fw_rsc_trace *vdev_rsc = rsc;
	(void)rproc;

	if (vdev_rsc->da != FW_RSC_U32_ADDR_ANY && vdev_rsc->len != 0)
		return 0;
	/* FIXME: The host should allocated a memory used by remote */

	return -RPROC_ERR_RSC_TAB_NS;
}

static int handle_vdev_rsc(struct remoteproc *rproc, void *rsc)
{
	struct fw_rsc_vdev *vdev_rsc = rsc;
	int i, num_vrings;
	unsigned int notifyid;
	struct fw_rsc_vdev_vring *vring_rsc;

	/* only assign notification IDs but do not initialize vdev */
	notifyid = vdev_rsc->notifyid;
	notifyid = remoteproc_allocate_id(rproc,
					  notifyid,
					  notifyid == RSC_NOTIFY_ID_ANY ?
					  RSC_NOTIFY_ID_ANY : notifyid + 1);
	if (notifyid != RSC_NOTIFY_ID_ANY)
		vdev_rsc->notifyid = notifyid;
	else
		return -RPROC_ERR_RSC_TAB_NP;

	num_vrings = vdev_rsc->num_of_vrings;
	for (i = 0; i < num_vrings; i++) {
		vring_rsc = &vdev_rsc->vring[i];
		notifyid = vring_rsc->notifyid;
		notifyid = remoteproc_allocate_id(rproc,
						  notifyid,
						  notifyid == RSC_NOTIFY_ID_ANY ?
						  RSC_NOTIFY_ID_ANY : notifyid + 1);
		if (notifyid != RSC_NOTIFY_ID_ANY)
			vring_rsc->notifyid = notifyid;
		else
			goto err;
	}

	return 0;

err:
	for (i--; i >= 0; i--) {
		vring_rsc = &vdev_rsc->vring[i];
		metal_bitmap_clear_bit(&rproc->bitmap, vring_rsc->notifyid);
	}
	metal_bitmap_clear_bit(&rproc->bitmap, vdev_rsc->notifyid);

	return -RPROC_ERR_RSC_TAB_NP;
}

static int handle_vendor_rsc(struct remoteproc *rproc, void *rsc)
{
	if (rproc && rproc->ops->handle_rsc) {
		struct fw_rsc_vendor *vend_rsc = rsc;
		size_t len = vend_rsc->len;

		return rproc->ops->handle_rsc(rproc, rsc, len);
	}
	return -RPROC_ERR_RSC_TAB_NS;
}

/**
 * @internal
 *
 * @brief Dummy resource handler.
 *
 * @param rproc	Pointer to remote remoteproc
 * @param rsc	Pointer to trace resource
 *
 * @return No service error
 */
static int handle_dummy_rsc(struct remoteproc *rproc, void *rsc)
{
	(void)rproc;
	(void)rsc;

	return -RPROC_ERR_RSC_TAB_NS;
}

/* Standard control request handling. */
typedef int (*rsc_handler)(struct remoteproc *rproc, void *rsc);

/* Resources handler */
static const rsc_handler rsc_handler_table[] = {
	handle_carve_out_rsc, /**< carved out resource */
	handle_dummy_rsc, /**< IOMMU dev mem resource */
	handle_trace_rsc, /**< trace buffer resource */
	handle_vdev_rsc, /**< virtio resource */
};

/**
 * @brief Validate the complete extent of a resource table entry.
 *
 * @param hdr		Resource entry header.
 * @param available	Bytes remaining in the resource table.
 *
 * @return 0 on success, otherwise -RPROC_ERR_RSC_TAB_TRUNC.
 */
static int validate_rsc_entry(struct fw_rsc_hdr *hdr, size_t available)
{
	size_t entry_size;

	if (available < sizeof(*hdr))
		return -RPROC_ERR_RSC_TAB_TRUNC;

	switch (hdr->type) {
	case RSC_CARVEOUT:
		entry_size = sizeof(struct fw_rsc_carveout);
		break;
	case RSC_DEVMEM:
		entry_size = sizeof(struct fw_rsc_devmem);
		break;
	case RSC_TRACE:
		entry_size = sizeof(struct fw_rsc_trace);
		break;
	case RSC_VDEV: {
		struct fw_rsc_vdev *vdev_rsc = (void *)hdr;

		entry_size = sizeof(*vdev_rsc);
		if (entry_size > available)
			return -RPROC_ERR_RSC_TAB_TRUNC;
		if (vdev_rsc->num_of_vrings >
		    (available - entry_size) /
		    sizeof(struct fw_rsc_vdev_vring))
			return -RPROC_ERR_RSC_TAB_TRUNC;
		entry_size += vdev_rsc->num_of_vrings *
			      sizeof(struct fw_rsc_vdev_vring);
		if (vdev_rsc->config_len > available - entry_size)
			return -RPROC_ERR_RSC_TAB_TRUNC;
		entry_size += vdev_rsc->config_len;
		break;
	}
	default:
		if (hdr->type >= RSC_VENDOR_START &&
		    hdr->type <= RSC_VENDOR_END) {
			struct fw_rsc_vendor *vend_rsc = (void *)hdr;

			if (available < sizeof(*vend_rsc))
				return -RPROC_ERR_RSC_TAB_TRUNC;
			entry_size = vend_rsc->len;
			if (entry_size < sizeof(*vend_rsc))
				return -RPROC_ERR_RSC_TAB_TRUNC;
		} else {
			entry_size = sizeof(*hdr);
		}
		break;
	}

	if (entry_size > available)
		return -RPROC_ERR_RSC_TAB_TRUNC;

	return 0;
}

int handle_rsc_table(struct remoteproc *rproc,
		     struct resource_table *rsc_table, size_t size,
		     struct metal_io_region *io)
{
	struct fw_rsc_hdr *hdr;
	uint32_t rsc_type;
	uint32_t num_entries;
	unsigned int idx;
	size_t offset;
	size_t rsc_offset;
	int status = 0;

	if (!rsc_table)
		return -RPROC_EINVAL;

	/* Validate rsc table header fields */

	/* Minimum rsc table size */
	if (sizeof(struct resource_table) > size) {
		return -RPROC_ERR_RSC_TAB_TRUNC;
	}
	if (io &&
	    (metal_io_virt_to_offset(io, rsc_table) == METAL_BAD_OFFSET ||
	     metal_io_virt_to_offset(io, (char *)rsc_table + size - 1) ==
	     METAL_BAD_OFFSET))
		return -RPROC_ERR_RSC_TAB_TRUNC;

	/* Supported version */
	if (rsc_table->ver != RSC_TAB_SUPPORTED_VERSION) {
		return -RPROC_ERR_RSC_TAB_VER;
	}

	/* Offset array */
	num_entries = rsc_table->num;
	if (num_entries > (size - sizeof(struct resource_table)) /
			  sizeof(rsc_table->offset[0])) {
		return -RPROC_ERR_RSC_TAB_TRUNC;
	}
	offset = sizeof(struct resource_table) +
		 num_entries * sizeof(rsc_table->offset[0]);

	/* Reserved fields - must be zero */
	if (rsc_table->reserved[0] != 0 || rsc_table->reserved[1] != 0) {
		return -RPROC_ERR_RSC_TAB_RSVD;
	}

	/* Loop through the offset array and parse each resource entry */
	for (idx = 0; idx < num_entries; idx++) {
		rsc_offset = rsc_table->offset[idx];
		/* Keep writable resources outside the table metadata. */
		if (rsc_offset < offset || rsc_offset > size ||
		    size - rsc_offset < sizeof(*hdr))
			return -RPROC_ERR_RSC_TAB_TRUNC;
		hdr = (void *)((char *)rsc_table + rsc_offset);
		status = validate_rsc_entry(hdr, size - rsc_offset);
		if (status)
			return status;
		rsc_type = hdr->type;
		if (rsc_type < RSC_LAST)
			status = rsc_handler_table[rsc_type](rproc, hdr);
		else if (rsc_type >= RSC_VENDOR_START &&
			 rsc_type <= RSC_VENDOR_END)
			status = handle_vendor_rsc(rproc, hdr);
		if (status == -RPROC_ERR_RSC_TAB_NS) {
			status = 0;
			continue;
		} else if (status) {
			break;
		}
	}

	return status;
}

size_t find_rsc(void *rsc_table, unsigned int rsc_type, unsigned int index)
{
	struct resource_table *r_table = rsc_table;
	struct fw_rsc_hdr *hdr;
	unsigned int i, rsc_index;
	unsigned int lrsc_type;

	metal_assert(r_table);
	if (!r_table)
		return 0;

	/* Loop through the offset array and parse each resource entry */
	rsc_index = 0;
	for (i = 0; i < r_table->num; i++) {
		hdr = (void *)((char *)r_table + r_table->offset[i]);
		lrsc_type = hdr->type;
		if (lrsc_type == rsc_type) {
			if (rsc_index++ == index)
				return r_table->offset[i];
		}
	}
	return 0;
}
