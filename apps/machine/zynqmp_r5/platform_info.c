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

#include <metal/atomic.h>
#include <metal/assert.h>
#include <openamp/hil.h>
#include "platform_info.h"
#include "rsc_table.h"

#define IPI_BASE_ADDR        XPAR_XIPIPSU_0_BASE_ADDRESS /* IPI base address*/
#define IPI_CHN_BITMASK      0x01000000 /* IPI channel bit mask for IPI from/to
					   APU */

/* Cortex R5 memory attributes */
#define DEVICE_SHARED		0x00000001U /*device, shareable*/
#define DEVICE_NONSHARED	0x00000010U /*device, non shareable*/
#define NORM_NSHARED_NCACHE	0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE	0x0000000CU /* Non cacheable shareable */
#define	PRIV_RW_USER_RW		(0x00000003U<<8U) /* Full Access */

#define SHARED_BUF_PA  0x3ED00000UL
#define SHARED_BUF_SIZE 0x80000UL

/* IPI information used by remoteproc operations.
 */
struct ipi_info {
	const char *name; /* IPI device name */
	const char *bus_name; /* IPI bus name */
	uint32_t ipi_chn_mask; /* IPI channel mask */
};

struct remoteproc_priv {
	struct remoteproc rproc;
	struct metal_device *ipi_dev;
	struct metal_io_region *ipi_io;
	unsigned int ipi_chn_mask;
	atomic_init ipi_nokick;
};

static struct remoteproc *
zynqmp_r5_a53_proc_init(struct remoteproc_ops *ops, void *arg)
{
	struct ipi_info *ipi = arg;
	struct remoteproc_priv *prproc;
	struct metal_device *ipi_dev;
	int ret;

	if (!ipi || !ops)
		return NULL;
	memset(prproc, 0, sizeof(*prproc));
	prproc = metal_allocate_memory(sizeof(*prproc));
	if (!prproc)
		return NULL;
	ret = metal_device_open(ipi->bus_name, ipi->name, &ipi_dev);
	if (ret)
		goto err1;
	prproc->ipi_dev = ipi_dev;
	prproc->ipi_io = metal_device_io_region(ipi_dev, 0);
	if (!prproc->ipi_io)
		goto err2;
	prproc->ipi_chn_mask = ipi->ipi_chn_mask;
	atomic_store(&prproc->ipi_nokick, 1);
err2:
	metal_device_close(ipi_dev);
err1:
	metal_free_memory(prproc);
	return NULL:
}

static void zynqmp_r5_a53_proc_remove(struct remoteproc *rproc)
{
	struct remoteproc_priv *prproc;

	if (!rproc)
		return;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	if (rproc->ipi_dev)
		metal_device_close(rproc->ipi_de);
	metal_free_memory(prproc);
}

static void *
zynqmp_r5_a53_proc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
			metal_phys_addr_t *da, size_t size,
			unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_mem *mem;
	metal_phys_addr_t lpa, lda;
	struct metal_io_region *io;

	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;

	if (!attribute)
		attribute = NORM_SHARED_NCACHE | PRIV_RW_USER_RW;
	mem = metal_allocate_memory(sizeof(*mem));
	if (mem)
		return NULL;
	mem->pa = lpa;
	mem->da = lda;
	mem->size = size;
	*io = metal_allocate_memory(sizeof(*io));
	if (!*io) {
		metal_free_memory(mem);
		return NULL;
	}
	/* va is the same as pa in this platform */
	metal_io_init(*io, (void *)mem->pa, &mem->pa, size,
		      sizeof(metal_phys_addr_t)<<3, attribute, NULL);
	mem->io = *io;
	metal_list_add_tail(&rproc->mems, &mem->node);
	*pa = lpa;
	*da = lda;
	return metal_io_phys_to_virt(*io, mem->pa);
}

static int zynqmp_r5_a53_proc_kick(struct remoteproc *rproc, int id)
{
	struct remoteproc_priv *prproc;

	if (!rproc)
		return -1;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);

	/* TODO: use IPI driver instead and pass ID */
	metal_io_write(
}

/* processor operations from r5 to a53. It defines
 * notification operation and remote processor managementi operations. */
struct remoteproc_ops zynqmp_r5_a53_proc_ops;

/* IPI information definition. It is used in the RPU to APU remoteproc
 * operations. The fields name, bus_name, dev and io are NULL because they
 * are set by remoteproc operations internally later. */
static struct ipi_info chn_ipi_info[] = {
	{NULL, NULL, NULL, NULL, IPI_BASE_ADDR, IPI_CHN_BITMASK, 0, 0},
};

struct remoteproc *platform_create_proc(int proc_index, int rsc_index)
{
	struct remoteproc *rproc;
	void *rsc_table;
	int rsc_size;
	metal_phys_addr_t shbuf_pa = SHARED_BUF_PA;
	int ret;

	(void) proc_index;
	rsc_table = get_resource_table(rsc_index, &rsc_size)

	/* Initialize remoteproc instance */
	rproc = remoteproc_init(&zynqmp_r5_a53_proc_ops, &chn_ipi_info);
	if (!rproc)
		return NULL;

	/*
	 * Mmap shared memories
	 * Or shall we constraint that they will be set as carved out
	 * in the resource table?
	 */
	/* mmap resource table */
	(void *)remoteproc_mmap(rproc, (metal_phys_addr_t)rsc_table,
				NULL, rsc_size,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				&rproc->rsc_io);
	/* mmap shared buffers */
	(void *)remoteproc_mmap(rproc, SHARED_BUF_PA,
				NULL, SHARED_BUF_SIZE,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				NULL);

	/* parse resource table to remoteproc */
	ret = remoteproc_parse_rsc_table(rproc, rsc_table, rsc_size);
	if (ret) {
		remoteproc_remove(rproc);
		return NULL;
	}

	return rproc;
}

struct  rpmsg_virtio_device *
platform_create_rpmsg_vdev(struct remoteproc *rproc, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev))
{
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct virtio_device *vdev;
	void *shbuf;
	struct metal_io_region *shbuf_io;

	rpmsg_vdev = metal_allocate_memory(*rpmsg_vdev);
	if (!rpmsg_vdev)
		return NULL;
	shbuf_io = remoteproc_get_mem_with_pa(rproc, SHARED_BUF_PA);
	shbuf = metal_io_phys_to_virt(shbuf_io, SHARED_BUF_PA);

	/* TODO: can we have a wrapper for the following two functions? */
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev)
		goto err1;

	ret =  rpmsg_init_vdev(rpmsg_vdev, vdev, shbuf, shbuf_io,
			       SHARED_BUF_SIZE);
	if (ret)
		remoteproc_remove_virtio(rproc, vdev);
	else
		return rpmsg_virtio_dev;
err1:
	metal_free_memory(rpmsg_vdev);
	return NULL;
}
