/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2017 Xilinx, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

#define RPU_CPU_ID          0 /* RPU remote CPU Index. We only talk to
                               * one CPU in the exmaple. We set the CPU
                               * index to 0. */
#define IPI_CHN_BITMASK     0x00000100 /* IPI channel bit mask for IPI
					* from/to RPU0 */
#define DEV_BUS_NAME        "platform" /* device bus name. "platform" bus
                                        * is used in Linux kernel for generic
					* devices */
/* libmetal devices names used in the examples.
 * They are platform devices, you find them in Linux sysfs
 * sys/bus/platform/devices */
#define IPI_DEV_NAME        "ff340000.ipi" /* IPI device name */
#define VRING_DEV_NAME      "3ed40000.vring" /* vring device name */
#define SHM_DEV_NAME        "3ed80000.shm" /* shared device name */

/* IPI information used by remoteproc operations.
 */
struct ipi_info {
	const char *name; /* IPI device name */
	const char *bus_name; /* IPI bus name */
	struct meta_device *dev; /* IPI metal device */
	struct metal_io_region *io; /* IPI metal IO region */
	metal_phys_addr_t paddr; /* IPI registers base address */
	uint32_t ipi_chn_mask; /* IPI channel mask */
	atomic_int sync; /* used internally by RPU to APU remoteproc to mark
			  * if there is kick from the remote */
};

/* processor operations for hil_proc from APU to RPU. It defines
 * notification operation and remote processor management operations. */
extern struct hil_platform_ops zynqmp_a53_r5_proc_ops;

/* IPI information definition. It is used in the APU to RPU remoteproc
 * operations. The fields dev and io are NULL because they are set by
 * remoteproc operations internally later. */
static struct ipi_info chn_ipi_info[] = {
	{IPI_DEV_NAME, DEV_BUS_NAME, NULL, NULL, 0, IPI_CHN_BITMASK, 0},
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
	proc = hil_create_proc(&zynqmp_a53_r5_proc_ops, RPU_CPU_ID, NULL);
	if (!proc)
		return NULL;

	/*************************************************************
	 * Set VirtIO device and vrings notification private data to
	 * hil_proc.
	 *************************************************************/
	/* Set VirtIO device nofication private data. It will be used when it
	 * needs to notify the remote on the virtio device status change. */
	hil_set_vdev_ipi(proc, 0,
		(unsigned int)(-1), (void *)&chn_ipi_info[0]);
	/* Set vring 0 nofication private data. */
	hil_set_vring_ipi(proc, 0,
		(unsigned int)(-1), (void *)&chn_ipi_info[0]);
	/* Set vring 1 nofication private data. */
	hil_set_vring_ipi(proc, 1,
		(unsigned int)(-1), (void *)&chn_ipi_info[0]);
	/* Set TX/RX vrings memory device */
	hil_set_vring(proc, 0, DEV_BUS_NAME, VRING_DEV_NAME, 0, 0);
	hil_set_vring(proc, 1, DEV_BUS_NAME, VRING_DEV_NAME, 0, 0);
	/* Set shared buffers memory device */
	hil_set_shm (proc, DEV_BUS_NAME, SHM_DEV_NAME, 0, 0x40000);
	/* Set name of RPMsg channel 0 */
	hil_set_rpmsg_channel(proc, 0, RPMSG_CHAN_NAME);

	return proc;
}
