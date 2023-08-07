/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2017 Xilinx, Inc.
 * Copyright (C) 2023 Texas Instruments Incorporated - https://www.ti.com/
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_INFO_H_
#define PLATFORM_INFO_H_

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>

#if defined __cplusplus
extern "C" {
#endif

/* Cortex R5 memory attributes */
#define DEVICE_SHARED       0x00000001U /* device, shareable */
#define DEVICE_NONSHARED    0x00000010U /* device, non shareable */
#define NORM_NSHARED_NCACHE 0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE  0x0000000CU /* Non cacheable shareable */
#define PRIV_RW_USER_RW     (0x00000003U<<8U) /* Full Access */
#define DDR_BASE_ADDR       0x80000000u

#define AM64_R5FSS1_0_MAILBOX 0x29040000U
#define RPMSG_BASE_ADDR       0xA2000000u
#define RSC_TABLE_BASE_ADDR   0xA2100000u

#define MAILBOX_BASE_ADDR   AM64_R5FSS1_0_MAILBOX
#define MAILBOX_NEW_MSG_INT 98

#ifndef RPMSG_NO_IPI
#define INT_BASE_ADDR       0x2FFF0000u
#endif

#define KICK_DEV_NAME         "mailbox"
#define KICK_BUS_NAME         "generic"

#ifndef SHARED_MEM_PA
#define SHARED_MEM_PA  RPMSG_BASE_ADDR
#endif /* !SHARED_MEM_PA */

#ifndef SHARED_MEM_SIZE
#define SHARED_MEM_SIZE 0x100000UL
#endif /* !SHARED_MEM_SIZE */

#ifndef SHARED_BUF_OFFSET
#define SHARED_BUF_OFFSET 0x8000UL
#endif /* !SHARED_BUF_OFFSET */

#ifndef RPMSG_NO_IPI
#define _rproc_wait() asm volatile("wfi")
#endif /* !RPMSG_NO_IPI */

extern uint32_t virtqueue_id;

struct remoteproc_priv {
	const char *kick_dev_name;
	const char *kick_dev_bus_name;
	struct metal_device *kick_dev;
	struct metal_io_region *kick_io;

	#ifndef RPMSG_NO_IPI
	atomic_int ipi_nokick;
	#endif
};

extern uint32_t virtqueue_id;

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
