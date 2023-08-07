/*
 * Copyright (C) 2023 Texas Instruments Incorporated - https://www.ti.com/
 *	Andrew Davis <afd@ti.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RSC_TABLE_H_
#define RSC_TABLE_H_

#include <stddef.h>
#include <openamp/open_amp.h>

#if defined __cplusplus
extern "C" {
#endif

#define NO_RESOURCE_ENTRIES 2

#define RPMSG_VDEV_DFEATURES        (1 << VIRTIO_RPMSG_F_NS)

/* VirtIO rpmsg device id */
#define VIRTIO_ID_RPMSG_             7

#define NUM_VRINGS                  0x02
#define VRING_ALIGN                 0x1000
#ifndef RING_TX
#define RING_TX                     FW_RSC_U32_ADDR_ANY
#endif /* !RING_TX */
#ifndef RING_RX
#define RING_RX                     FW_RSC_U32_ADDR_ANY
#endif /* RING_RX */
#define VRING_SIZE                  256U

/* Resource table for the given remote */
struct remote_resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NO_RESOURCE_ENTRIES];
	/* rpmsg vdev entry */
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring rpmsg_vring0;
	struct fw_rsc_vdev_vring rpmsg_vring1;
	/* trace buffer entry */
	struct fw_rsc_trace trace;
}__attribute__((packed, aligned(0x100)));

void *get_resource_table (int rsc_id, int *len);


#if defined __cplusplus
}
#endif

#endif /* RSC_TABLE_H_ */