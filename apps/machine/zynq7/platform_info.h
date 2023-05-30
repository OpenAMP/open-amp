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
#include <xparameters.h>

#if defined __cplusplus
extern "C" {
#endif

#define RING_TX                     FW_RSC_U32_ADDR_ANY
#define RING_RX                     FW_RSC_U32_ADDR_ANY
#define NUM_VRINGS                  0x02
#define VRING_ALIGN                 0x1000

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

/* Another APU core ID. In this demo, the other APU core is 0. */
#define A9_CPU_ID	0UL

/* scugic device, used to raise soft irq */
#define SCUGIC_DEV_NAME	"scugic_dev"
#define SCUGIC_BUS_NAME	"generic"

/* scugic base address */
#define SCUGIC_PERIPH_BASE	0xF8F00000
#define SCUGIC_DIST_BASE	(SCUGIC_PERIPH_BASE + 0x00001000)

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

/**
 * platform_init - initialize the platform
 *
 * It will initialize the platform.
 *
 * @argc: number of arguments
 * @argv: array of the input arguments
 * @platform: pointer to store the platform data pointer
 *
 * return 0 for success or negative value for failure
 */
int platform_init(int argc, char *argv[], void **platform);

/**
 * platform_create_rpmsg_vdev - create rpmsg vdev
 *
 * It will create rpmsg virtio device, and returns the rpmsg virtio
 * device pointer.
 *
 * @platform: pointer to the private data
 * @vdev_index: index of the virtio device, there can more than one vdev
 *              on the platform.
 * @role: virtio driver or virtio device of the vdev
 * @rst_cb: virtio device reset callback
 * @ns_bind_cb: rpmsg name service bind callback
 *
 * return pointer to the rpmsg virtio device
 */
struct rpmsg_device *
platform_create_rpmsg_vdev(void *platform, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ns_bind_cb ns_bind_cb);

/**
 * platform_poll - platform poll function
 *
 * @platform: pointer to the platform
 *
 * return negative value for errors, otherwise 0.
 */
int platform_poll(void *platform);

/**
 * platform_release_rpmsg_vdev - release rpmsg virtio device
 *
 * @rpdev: pointer to the rpmsg device
 */
void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev, void *platform);

/**
 * platform_cleanup - clean up the platform resource
 *
 * @platform: pointer to the platform
 */
void platform_cleanup(void *platform);

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H_ */
