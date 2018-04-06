/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2017 Xilinx, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**************************************************************************
 * FILE NAME
 *
 *       platform_info.c
 *
 * DESCRIPTION
 *
 *       This file define platform specific data and implements APIs to set
 *       platform specific information for OpenAMP.
 *
 **************************************************************************/

#include <openamp/hil.h>
#include <metal/atomic.h>
#include "platform_info.h"

#define IPI_BASE_ADDR        XPAR_XIPIPSU_0_BASE_ADDRESS /* IPI base address*/
#define IPI_CHN_BITMASK      0x01000000 /* IPI channel bit mask for IPI from/to
					   APU */

#define APU_CPU_ID           0 /* APU remote CPU Index. We only talk to one CPU
				* in the example. We set the CPU index to 0. */

/* IPI information used by remoteproc operations.
 */
struct ipi_info {
	const char *name; /* IPI device name */
	const char *bus_name; /* IPI bus name */
	struct meta_device *dev; /* IPI metal device */
	struct metal_io_region *io; /* IPI metal IO region */
	metal_phys_addr_t paddr; /* IPI registers base address */
	uint32_t ipi_chn_mask; /* IPI channel mask */
	int registered; /* used internally by RPU to APU remoteproc to mark
			 * if the IPI interrup has been registered */
	atomic_int sync; /* used internally by RPU to APU remoteproc to mark
			  * if there is kick from the remote */
};

/* processor operations for hil_proc from r5 to a53. It defines
 * notification operation and remote processor managementi operations. */
extern struct hil_platform_ops zynqmp_r5_a53_proc_ops;

/* IPI information definition. It is used in the RPU to APU remoteproc
 * operations. The fields name, bus_name, dev and io are NULL because they
 * are set by remoteproc operations internally later. */
static struct ipi_info chn_ipi_info[] = {
	{NULL, NULL, NULL, NULL, IPI_BASE_ADDR, IPI_CHN_BITMASK, 0, 0},
};

/* Firmware_info and fw_table_size are required in current version (2017.3)
 * of OpenAMP library to pass compilation. Will be removed in next release
 * if the OpenAMP application is not used to boot the remote. */
const struct firmware_info fw_table[] =
{
	{"unknown",
	 0,
	 0}
};
const int fw_table_size = sizeof(fw_table)/sizeof(struct firmware_info);

struct hil_proc *platform_create_proc(int proc_index)
{
	(void) proc_index;

	/* structure to represent a remote processor. It encapsulates the
	 * shared memory and notification info required for inter processor
	 * communication. */
	struct hil_proc *proc;
	proc = hil_create_proc(&zynqmp_r5_a53_proc_ops, APU_CPU_ID, NULL);
	if (!proc)
		return NULL;

	/*************************************************************
	 * Set VirtIO device and vrings notification private data to
	 * hil_proc.
	 *************************************************************/
	/* Set VirtIO device nofication private data. It will be used when it
	 * needs to notify the remote on the virtio device status change. */
	hil_set_vdev_ipi(proc, 0,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);
	/* Set vring 0 nofication private data. */
	hil_set_vring_ipi(proc, 0,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);
	/* Set vring 1 nofication private data. */
	hil_set_vring_ipi(proc, 1,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);
	/* Set name of RPMsg channel 0 */
	hil_set_rpmsg_channel(proc, 0, RPMSG_CHAN_NAME);

	return proc;
}
