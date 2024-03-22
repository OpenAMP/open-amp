/*
 * Copyright (C) 2023 Texas Instruments Incorporated - https://www.ti.com/
 *	Andrew Davis <afd@ti.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This file populates resource table for the remote core
 * for use by the Linux host
 */

#include <openamp/open_amp.h>

#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/io.h>
#include <metal/alloc.h>

#include "rsc_table.h"
#include "helper.h"

/* Place resource table in special ELF section */
#define __section_t(S)          __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)

struct remote_resource_table __resource resource_table =
{
	/* table header information */
	1U, /* we're the first version that implements this */
	NO_RESOURCE_ENTRIES, /* number of entries */
	{ 0U, 0U, }, /* reserved, must be zero */

	/* offsets to the entries */
	{
		offsetof(struct remote_resource_table, rpmsg_vdev),
		offsetof(struct remote_resource_table, trace),
	},

	/* vdev entry */
	{ RSC_VDEV, VIRTIO_ID_RPMSG_, 31U, RPMSG_VDEV_DFEATURES, 0U, 0U, 0U, NUM_VRINGS, {0U, 0U}, },
	/* the two vrings */
	{RING_TX, VRING_ALIGN, VRING_SIZE, 1U, 0U},
	{RING_RX, VRING_ALIGN, VRING_SIZE, 2U, 0U},

	/* trace buffer entry */
	{ RSC_TRACE, (uint32_t)debug_log_memory, DEBUG_LOG_SIZE, 0, "trace:r5fss0_0", },
};

void *get_resource_table (int rsc_id, int *len)
{
	(void) rsc_id;
	*len = sizeof(resource_table);
	return &resource_table;
}

