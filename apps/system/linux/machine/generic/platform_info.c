/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Xilinx, Inc.
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
 *       This file implements APIs to get platform specific
 *       information for OpenAMP.
 *
 **************************************************************************/

#include <openamp/hil.h>
#include <metal/io.h>
#include <metal/atomic.h>

#define IPI_CHN_BITMASK                   0x01000000 /* IPI channel bit mask APU<->RPU0 */
#define REMOTE_CPU_ID                     0

struct vring_ipi_info {
	/* Socket file path */
	char *path;
	int fd;
	struct metal_io_region *vring_io;
	atomic_int sync;
};

struct rproc_info {
	unsigned long rproc_id;
	char *vring0_path;
	char *vring0_bus;
	char *vring1_path;
	char *vring1_bus;
	char *shm_path;
	char *shm_bus;
	int shm_size;
	void *ipi0;
	void *ipi1;
	char *rpmsg_chnl;
};

extern struct hil_platform_ops linux_proc_ops;

static struct vring_ipi_info ipi_table[] = {
	{ "unixs:/tmp/openamp.event.0", -1, NULL, 0 },
	{ "unixs:/tmp/openamp.event.1", -1, NULL, 0 },
	{ "unix:/tmp/openamp.event.0", -1, NULL, 0 },
	{ "unix:/tmp/openamp.event.1", -1, NULL, 0 },
};

static struct rproc_info rproc_table [] = {
	{
		0,
		"openamp.vrings",
		NULL,
		"openamp.vrings",
		NULL,
		"openamp.shm",
		NULL,
		0x40000,
		(void *)&ipi_table[0],
		(void *)&ipi_table[1],
		"rpmsg-openamp-demo-channel",
	},
	{
		0,
		"openamp.vrings",
		NULL,
		"openamp.vrings",
		NULL,
		"openamp.shm",
		NULL,
		0x40000,
		(void *)&ipi_table[2],
		(void *)&ipi_table[3],
		"rpmsg-openamp-demo-channel",
	},
};

const struct firmware_info fw_table[] =
{
	{"unknown",
	 0,
	 0}
};

const int fw_table_size = sizeof(fw_table)/sizeof(struct firmware_info);

struct hil_proc *platform_create_proc(int proc_index)
{

	struct hil_proc *proc;
	struct rproc_info *proc_data;
	if (sizeof(rproc_table)/sizeof(struct rproc_info) <=
		(unsigned long)proc_index){
		fprintf(stderr, "ERROR: CPU id is invalid: %d.\n", proc_index);
		return NULL;
	}
	proc_data = &rproc_table[proc_index];
	proc = hil_create_proc(&linux_proc_ops, proc_data->rproc_id, NULL);
	if (!proc)
		return NULL;

	/* Set VirtIO device nofication private data.
	 * At the moment, the resource table is local, it will need to be
	 * changed to stay in shared memory.
	 */
	hil_set_vdev_ipi(proc, 0, (-1U), proc_data->ipi0);
	/* Setup IPI info */
	hil_set_vring_ipi(proc, 0, (unsigned int)(-1), proc_data->ipi0);
	hil_set_vring_ipi(proc, 1, (unsigned int)(-1), proc_data->ipi1);
	/* Setup vring info */
	hil_set_vring(proc, 0, proc_data->vring0_bus, proc_data->vring0_path,
			0, 0x8000);
	hil_set_vring(proc, 1, proc_data->vring1_bus, proc_data->vring1_path,
			0, 0x8000);
	/* Setup shared memory info */
	hil_set_shm (proc, proc_data->shm_bus, proc_data->shm_path,
		0, proc_data->shm_size);
	/* Setup RPMSG channel info */
	hil_set_rpmsg_channel(proc, 0, proc_data->rpmsg_chnl);

	return proc;
}

