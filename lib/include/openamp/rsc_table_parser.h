/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RSC_TABLE_PARSER_H
#define RSC_TABLE_PARSER_H

#include <openamp/remoteproc.h>

#if defined __cplusplus
extern "C" {
#endif

#define RSC_TAB_SUPPORTED_VERSION           1
#define RSC_TAB_HEADER_SIZE                 12
#define RSC_TAB_MAX_VRINGS                  2

/* Standard control request handling. */
typedef int (*rsc_handler) (struct remoteproc * rproc, void *rsc);

/* Function prototypes */
int handle_rsc_table(struct remoteproc *rproc,
		     struct resource_table *rsc_table, int len);
int handle_carve_out_rsc(struct remoteproc *rproc, void *rsc);
int handle_trace_rsc(struct remoteproc *rproc, void *rsc);
int handle_dev_mem_rsc(struct remoteproc *rproc, void *rsc);
int handle_vdev_rsc(struct remoteproc *rproc, void *rsc);
int handle_rproc_mem_rsc(struct remoteproc *rproc, void *rsc);
int handle_fw_chksum_rsc(struct remoteproc *rproc, void *rsc);
int handle_mmu_rsc(struct remoteproc *rproc, void *rsc);
int handle_vendor_rsc(struct remoteproc *rproc, void *rsc);

/**
 * find_rsc
 *
 * find out location of a resource type in the resource table.
 *
 * @rsc_table - pointer to the resource table
 * @rsc_type - type of the resource
 * @index - index of the resource of the specified type
 *
 * return the offset to the resource on success, or 0 on failure
 */
size_t find_rsc(void *rsc_table, unsigned int rsc_type, unsigned int index);

#if defined __cplusplus
}
#endif

#endif				/* RSC_TABLE_PARSER_H */
