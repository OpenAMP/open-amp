/*
 * Copyright (c) 2014, Mentor Graphics Corporation. All rights reserved.
 * Copyright (c) 2017 - 2018 Xilinx, Inc. All rights reserved.
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

/* SGIs */
#define SGI_TO_NOTIFY		15 /* SGI to notify the remote */
#define SGI_NOTIFICATION	14 /* SGI from the remote */

/* Memory attributes */
#define NORM_NONCACHE 0x11DE2	/* Normal Non-cacheable */
#define STRONG_ORDERED 0xC02	/* Strongly ordered */
#define DEVICE_MEMORY 0xC06	/* Device memory */
#define RESERVED 0x0		/* reserved memory */

/* Shared memory */
#define SHARED_MEM_PA  0x3e800000UL
#define SHARED_MEM_SIZE 0x80000UL
#define SHARED_BUF_OFFSET 0x80000UL

/* Zynq CPU ID mask */
#define ZYNQ_CPU_ID_MASK 0x1UL

/* Remoteproc private data struct */
struct remoteproc_priv {
	const char *gic_name; /* SCUGIC device name */
	const char *gic_bus_name; /* SCUGIC bus name */
	struct metal_device *gic_dev; /* pointer to SCUGIC device */
	struct metal_io_region *gic_io; /* pointer to SCUGIC i/o region */
	unsigned int irq_to_notify; /* SCUGIC IRQ vector to notify the
				     * other end.
				     */
	unsigned int irq_notification; /* SCUGIC IRQ vector received from
					* other end.
					*/
	unsigned int cpu_id; /* CPU ID */
	atomic_int nokick; /* 0 for kick from other side */
};

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H_ */
