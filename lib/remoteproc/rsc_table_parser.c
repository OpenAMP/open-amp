/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "openamp/rsc_table_parser.h"

/* Resources handler */
rsc_handler rsc_handler_table[] = {
	handle_carve_out_rsc,
	handle_trace_rsc,
	handle_dev_mem_rsc,
	handle_vdev_rsc,
	handle_mmu_rsc
};

/**
 * handle_rsc_table
 *
 * This function parses resource table.
 *
 * @param rproc     - pointer to remote remote_proc
 * @param rsc_table - resource table to parse
 * @param size      -  size of rsc table
 *
 * @returns - execution status
 *
 */
int handle_rsc_table(struct remote_proc *rproc,
		     struct resource_table *rsc_table, int size)
{

	unsigned char *rsc_start;
	unsigned int *rsc_offset;
	unsigned int rsc_type;
	int idx, status = 0;

	/* Validate rsc table header fields */

	/* Minimum rsc table size */
	if (sizeof(struct resource_table) > size) {
		return (RPROC_ERR_RSC_TAB_TRUNC);
	}

	/* Supported version */
	if (rsc_table->ver != RSC_TAB_SUPPORTED_VERSION) {
		return (RPROC_ERR_RSC_TAB_VER);
	}

	/* Offset array */
	if (sizeof(struct resource_table)
	    + rsc_table->num * sizeof(rsc_table->offset[0]) > size) {
		return (RPROC_ERR_RSC_TAB_TRUNC);
	}

	/* Reserved fields - must be zero */
	if ((rsc_table->reserved[0] != 0 || rsc_table->reserved[1]) != 0) {
		return RPROC_ERR_RSC_TAB_RSVD;
	}

	rsc_start = (unsigned char *)rsc_table;

	/* Loop through the offset array and parse each resource entry */
	for (idx = 0; idx < rsc_table->num; idx++) {
		rsc_offset =
		    (unsigned int *)(rsc_start + rsc_table->offset[idx]);
		rsc_type = *rsc_offset;
		status =
		    rsc_handler_table[rsc_type] (rproc, (void *)rsc_offset);
		if (status != RPROC_SUCCESS) {
			break;
		}
	}

	return status;
}

/**
 * handle_carve_out_rsc
 *
 * Carveout resource handler.
 *
 * @param rproc - pointer to remote remote_proc
 * @param rsc   - pointer to carveout resource
 *
 * @returns - execution status
 *
 */
int handle_carve_out_rsc(struct remote_proc *rproc, void *rsc)
{
	struct fw_rsc_carveout *carve_rsc = (struct fw_rsc_carveout *)rsc;

	/* Validate resource fields */
	if (!carve_rsc) {
		return RPROC_ERR_RSC_TAB_NP;
	}

	if (carve_rsc->reserved) {
		return RPROC_ERR_RSC_TAB_RSVD;
	}

	if (rproc->role == RPROC_MASTER) {
		/* Map memory region for loading the image */
		env_map_memory(carve_rsc->da, carve_rsc->da, carve_rsc->len,
			       (SHARED_MEM | UNCACHED));
	}

	return RPROC_SUCCESS;
}

/**
 * handle_trace_rsc
 *
 * Trace resource handler.
 *
 * @param rproc - pointer to remote remote_proc
 * @param rsc   - pointer to trace resource
 *
 * @returns - execution status
 *
 */
int handle_trace_rsc(struct remote_proc *rproc, void *rsc)
{
	return RPROC_ERR_RSC_TAB_NS;
}

/**
 * handle_dev_mem_rsc
 *
 * Device memory resource handler.
 *
 * @param rproc - pointer to remote remote_proc
 * @param rsc   - pointer to device memory resource
 *
 * @returns - execution status
 *
 */
int handle_dev_mem_rsc(struct remote_proc *rproc, void *rsc)
{
	return RPROC_ERR_RSC_TAB_NS;
}

/**
 * handle_vdev_rsc
 *
 * Virtio device resource handler
 *
 * @param rproc - pointer to remote remote_proc
 * @param rsc   - pointer to virtio device resource
 *
 * @returns - execution status
 *
 */
int handle_vdev_rsc(struct remote_proc *rproc, void *rsc)
{

	struct fw_rsc_vdev *vdev_rsc = (struct fw_rsc_vdev *)rsc;
	struct fw_rsc_vdev_vring *vring;
	struct proc_vdev *vdev;
	struct proc_vring *vring_table;
	int idx;

	if (!vdev_rsc) {
		return RPROC_ERR_RSC_TAB_NP;
	}

	/* Maximum supported vrings per Virtio device */
	if (vdev_rsc->num_of_vrings > RSC_TAB_MAX_VRINGS) {
		return RPROC_ERR_RSC_TAB_VDEV_NRINGS;
	}

	/* Reserved fields - must be zero */
	if (vdev_rsc->reserved[0] || vdev_rsc->reserved[1]) {
		return RPROC_ERR_RSC_TAB_RSVD;
	}

	/* Get the Virtio device from HIL proc */
	vdev = hil_get_vdev_info(rproc->proc);

	/* Initialize HIL Virtio device resources */
	vdev->num_vrings = vdev_rsc->num_of_vrings;
	vdev->dfeatures = vdev_rsc->dfeatures;
	vdev->gfeatures = vdev_rsc->gfeatures;
	vring_table = &vdev->vring_info[0];

	for (idx = 0; idx < vdev_rsc->num_of_vrings; idx++) {
		vring = &vdev_rsc->vring[idx];

		/* Initialize HIL vring resources */
		vring_table[idx].phy_addr = (void *)vring->da;
		vring_table[idx].num_descs = vring->num;
		vring_table[idx].align = vring->align;

		/* Enable access to vring memory regions */
		env_map_memory(vring->da, vring->da,
			       vring_size(vring->num, vring->align),
			       (SHARED_MEM | UNCACHED));
	}

	return RPROC_SUCCESS;
}

/**
 * handle_mmu_rsc
 *
 * This function parses mmu resource , requested by the peripheral.
 *
 * @param rproc - pointer to remote remote_proc
 * @param rsc   - pointer to mmu resource
 *
 * @returns - execution status
 *
 */
int handle_mmu_rsc(struct remote_proc *rproc, void *rsc)
{
	return RPROC_ERR_RSC_TAB_NS;
}
