/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**************************************************************************
 * FILE NAME
 *
 *       remoteproc_loader.h
 *
 * COMPONENT
 *
 *         OpenAMP stack.
 *
 * DESCRIPTION
 *
 *       This file provides definitions for remoteproc loader
 *
 *
 **************************************************************************/
#ifndef REMOTEPROC_LOADER_H_
#define REMOTEPROC_LOADER_H_

#include <metal/io.h>
#include <metal/list.h>
#include <openamp/remoteproc.h>

#if defined __cplusplus
extern "C" {
#endif

/* Loader macros */
#define SYNC_LOAD 1UL
#define ASYNC_LOAD 0UL

/* Loader feature macros */
#define SUPPORT_SEEK 1UL

/**
 * struct image_store_ops - user defined image store operations
 * @open: user defined callback to open the "firmware" to prepare loading
 * @close: user defined callback to close the "firmware" to clean up
 *         after loading
 * @load: user defined callback to load the firmware contents to target
 *           memory
 * @load_finish: user defined callback used to make sure all the async
 *               firmware loading finish
 * @features: loader supported features. e.g. seek
 */
struct image_store_ops {
	int (*open)(void *store);
	void (*close)(void *store);
	long (*load)(void *store, size_t offset, void *dest,
			size_t size, struct metal_io_region *io, int sync);
	int (*load_finish)(void *store);
	unsigned int features;
};

/**
 * struct loader_ops - loader oeprations
 * @identify: identify the firmware
 * @parse: define how to parse firmware and get the headers information
 * @get_rsc_table: define get the resource table length and target device
 * @copy_rsc_table: define how to copy the resource table from firmware to
 *                  local memory.
 * @load: define how to load the firmware
 * @close: define how to close the firmware
 * @get_entry: get entry address
 */
struct loader_ops {
	void *(*parse)(void *store, struct image_store_ops *ops);
	long (*get_rsc_table)(void *loader_info,
			      metal_phys_addr_t *da_ptr);
	void *(*copy_rsc_table)(void *fw, void *loader_info,
				struct image_store_ops *ops, void *rsc_table);
	int (*load)(void *fw, void *fw_info, struct remoteproc *rproc,
		    struct image_store_ops *ops);
	void (*close)(void *fw, void *loader_info,
		      struct image_store_ops *ops);
	metal_phys_addr_t (*get_entry)(void *loader_info);
};

#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_LOADER_H_ */
