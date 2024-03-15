/*
 * Copyright (c) 2016 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This file populates resource table for BM remote
 * for use by the Linux host
 */

#ifndef PLATFORM_INFO_H_
#define PLATFORM_INFO_H_

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>

#include "platform_info_common.h"

#if defined __cplusplus
extern "C" {
#endif

struct remoteproc_priv {
	const char *ipi_name; /**< IPI device name */
	const char *ipi_bus_name; /**< IPI bus name */
	const char *rsc_name; /**< rsc device name */
	const char *rsc_bus_name; /**< rsc bus name */
	const char *shm_name; /**< shared memory device name */
	const char *shm_bus_name; /**< shared memory bus name */
	struct metal_device *ipi_dev; /**< pointer to IPI device */
	struct metal_io_region *ipi_io; /**< pointer to IPI i/o region */
	struct metal_device *shm_dev; /**< pointer to shared memory device */
	struct metal_io_region *shm_io; /**< pointer to sh mem i/o region */

	struct remoteproc_mem shm_mem; /**< shared memory */
	unsigned int ipi_chn_mask; /**< IPI channel mask */
	atomic_int ipi_nokick;
#ifdef RPMSG_NO_IPI
	const char *shm_poll_name; /**< shared memory device name */
	const char *shm_poll_bus_name; /**< shared memory bus name */
	struct metal_device *shm_poll_dev; /**< pointer to poll mem device */
	struct metal_io_region *shm_poll_io; /**< pointer to poll mem i/o */
#endif /* RPMSG_NO_IPI */

};

#ifdef RPMSG_NO_IPI
#ifndef POLL_DEV_NAME
#define POLL_DEV_NAME        "3ee40000.poll" /* shared device name */
#endif /* !POLL_DEV_NAME */
#define POLL_STOP 0x1U
#endif /* RPMSG_NO_IPI */

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H_ */

