/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * Copyright (c) 2018, Xilinx Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rsc_table_parser.h>
#include <metal/io.h>

static int handle_dummy_rsc(struct remoteproc *rproc, void *rsc);

/* Resources handler */
rsc_handler rsc_handler_table[] = {
	handle_carve_out_rsc, /**< carved out resource */
	handle_dummy_rsc, /**< IOMMU dev mem resource */
	handle_dummy_rsc, /**< trace buffer resource */
	handle_dummy_rsc, /**< virtio resource */
	handle_dummy_rsc, /**< rproc shared memory resource */
	handle_dummy_rsc, /**< firmware checksum resource */
};

/**
 * handle_rsc_table
 *
 * This function parses resource table.
 *
 * @param rproc     - pointer to remote remoteproc
 * @param rsc_table - resource table to parse
 * @param size      -  size of rsc table
 *
 * @returns - execution status
 *
 */
int handle_rsc_table(struct remoteproc *rproc,
		     struct resource_table *rsc_table, int size)
{

	void *rsc_start;
	unsigned int rsc_type;
	unsigned int idx;
	int status = 0;

	/* Validate rsc table header fields */

	/* Minimum rsc table size */
	if (sizeof(struct resource_table) > (unsigned int)size) {
		return -RPROC_ERR_RSC_TAB_TRUNC;
	}

	/* Supported version */
	if (rsc_table->ver != RSC_TAB_SUPPORTED_VERSION) {
		return -RPROC_ERR_RSC_TAB_VER;
	}

	/* Offset array */
	if (sizeof(struct resource_table)
	    + rsc_table->num * sizeof(rsc_table->offset[0]) > (unsigned int)size) {
		return -RPROC_ERR_RSC_TAB_TRUNC;
	}

	/* Reserved fields - must be zero */
	if ((rsc_table->reserved[0] != 0 || rsc_table->reserved[1]) != 0) {
		return -RPROC_ERR_RSC_TAB_RSVD;
	}

	rsc_start = rsc_table;

	/* Loop through the offset array and parse each resource entry */
	for (idx = 0; idx < rsc_table->num; idx++) {
		rsc_start = rsc_start + rsc_table->offset[idx];
		rsc_type = *((uint32_t *)rsc_start);
		if (rsc_type < RSC_LAST)
			status = rsc_handler_table[rsc_type](rproc,
							     rsc_start);
		else if (rsc_type >= RSC_VENDOR_START &&
			 rsc_type <= RSC_VENDOR_END)
			status = handle_vendor_rsc(rproc, rsc_start);
		if (status == -RPROC_ERR_RSC_TAB_NS)
			continue;
		else if (status)
			break;
	}

	return status;
}

/**
 * handle_carve_out_rsc
 *
 * Carveout resource handler.
 *
 * @param rproc - pointer to remote remoteproc
 * @param rsc   - pointer to carveout resource
 *
 * @returns - 0 for success, or negative value for failure
 *
 */
int handle_carve_out_rsc(struct remoteproc *rproc, void *rsc)
{
	struct fw_rsc_carveout *carve_rsc = (struct fw_rsc_carveout *)rsc;
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

int handle_vendor_rsc(struct remoteproc *rproc, void *rsc)
{
	if (rproc && rproc->ops->handle_rsc) {
		struct fw_rsc_vendor *vend_rsc = rsc;
		size_t len = vend_rsc->len;

		return rproc->ops->handle_rsc(rproc, rsc, len);
	}
	return -RPROC_ERR_RSC_TAB_NS;
}

/**
 * handle_dummy_rsc
 *
 * dummy resource handler.
 *
 * @param rproc - pointer to remote remoteproc
 * @param rsc   - pointer to trace resource
 *
 * @returns - no service error
 *
 */
static int handle_dummy_rsc(struct remoteproc *rproc, void *rsc)
{
	(void)rproc;
	(void)rsc;

	return -RPROC_ERR_RSC_TAB_NS;
}
