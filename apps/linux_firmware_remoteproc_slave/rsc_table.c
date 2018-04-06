/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remotes
 * for use by the Linux Master */

#include <openamp/open_amp.h>
#include "rsc_table.h"

/* Place resource table in special ELF section */
#define __section_t(S)            __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)

#define RPMSG_IPU_C0_FEATURES       1

/* VirtIO rpmsg device id */
#define VIRTIO_ID_RPMSG_             7

/* Remote supports Name Service announcement */
#define VIRTIO_RPMSG_F_NS           0

/* Resource table entries */
#define ELF_START                   0x00000000
#define ELF_END                     0x08000000
#define NUM_VRINGS					0x02
#define VRING_ALIGN					0x1000
#define RING_TX                     0x08000000
#define RING_RX                     0x08004000
#define VRING_SIZE                  256

const struct remote_resource_table __resource resources = {
	/* Version */
	1,

	/* NUmber of table entries */
	2,
	/* reserved fields */
	{0, 0,},

	/* Offsets of rsc entries */
	{
	 offsetof(struct remote_resource_table, elf_cout),
	 offsetof(struct remote_resource_table, rpmsg_vdev),
	 },

	/* End of ELF file */
	{
	 RSC_CARVEOUT, ELF_START, ELF_START, ELF_END, 0, 0, "ELF_COUT",
	 },

	/* Virtio device entry */
	{RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_IPU_C0_FEATURES, 0, 0, 0,
	 NUM_VRINGS, {0, 0},
	 },

	/* Vring rsc entry - part of vdev rsc entry */
	{
	 RING_TX, VRING_ALIGN, VRING_SIZE, 1, 0},
	{
	 RING_RX, VRING_ALIGN, VRING_SIZE, 2, 0},
};
