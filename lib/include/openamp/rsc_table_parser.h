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

/**
 * @internal
 *
 * @brief This function parses resource table.
 *
 * @param rproc		Pointer to remote remoteproc
 * @param rsc_table	Resource table to parse
 * @param len		Size of rsc table
 * @param io		Pointer to the resource table I/O region
 *			It can be NULL if the resource table
 *			is in the local memory.
 *
 * @return Execution status
 */
int handle_rsc_table(struct remoteproc *rproc,
		     struct resource_table *rsc_table, size_t len,
		     struct metal_io_region *io);

/**
 * @internal
 *
 * @brief Find out location of a resource type in the resource table.
 *
 * @param rsc_table	Pointer to the resource table
 * @param rsc_type	Type of the resource
 * @param index		Index of the resource of the specified type
 *
 * @return The offset to the resource on success, or 0 on failure
 */
size_t find_rsc(void *rsc_table, unsigned int rsc_type, unsigned int index);

#if defined __cplusplus
}
#endif

#endif				/* RSC_TABLE_PARSER_H */
