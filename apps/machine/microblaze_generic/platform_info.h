/*
 * Copyright (c) 2020 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

/* MicroBlaze memory attributes. Depends on MB memory the design */
#define DEVICE_SHARED       0x00000001U /* device, shareable */
#define DEVICE_NONSHARED    0x00000010U /* device, non shareable */
#define NORM_NSHARED_NCACHE 0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE  0x0000000CU /* Non cacheable shareable */
#define PRIV_RW_USER_RW     (0x00000003U << 8U) /* Full Access */

#define POLL_SHM_LOCATION 0x3EE40000
#define POLL_STOP 0x1U

struct remoteproc_priv {
	const char *ipi_name; /**< IPI device name */
	const char *ipi_bus_name; /**< IPI bus name */
	struct metal_device *ipi_dev; /**< pointer to IPI device */
	struct metal_io_region *ipi_io; /**< pointer to IPI i/o region */
	unsigned int ipi_chn_mask; /**< IPI channel mask */
	atomic_int ipi_nokick;
};

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H_ */
