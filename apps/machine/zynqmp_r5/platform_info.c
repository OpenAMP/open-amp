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
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>
#include "platform_info.h"
#include "rsc_table.h"

#define IPI_DEV_NAME         "ipi_dev"
#define IPI_BUS_NAME         "generic"
#define IPI_BASE_ADDR        XPAR_XIPIPSU_0_BASE_ADDRESS /* IPI base address*/
#define IPI_CHN_BITMASK      0x01000000 /* IPI channel bit mask for IPI from/to
					   APU */
/* IPI REGs OFFSET */
#define IPI_TRIG_OFFSET          0x00000000    /* IPI trigger register offset */
#define IPI_OBS_OFFSET           0x00000004    /* IPI observation register offset */
#define IPI_ISR_OFFSET           0x00000010    /* IPI interrupt status register offset */
#define IPI_IMR_OFFSET           0x00000014    /* IPI interrupt mask register offset */
#define IPI_IER_OFFSET           0x00000018    /* IPI interrupt enable register offset */
#define IPI_IDR_OFFSET           0x0000001C    /* IPI interrupt disable register offset */

/* Cortex R5 memory attributes */
#define DEVICE_SHARED		0x00000001U /*device, shareable*/
#define DEVICE_NONSHARED	0x00000010U /*device, non shareable*/
#define NORM_NSHARED_NCACHE	0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE	0x0000000CU /* Non cacheable shareable */
#define	PRIV_RW_USER_RW		(0x00000003U<<8U) /* Full Access */

#define SHARED_BUF_PA  0x3ED40000UL
#define SHARED_BUF_SIZE 0x100000UL

#define _rproc_wait() asm volatile("wfi")

/* IPI information used by remoteproc operations.
 */
static metal_phys_addr_t ipi_phys_addr = IPI_BASE_ADDR;
struct metal_device ipi_device = {
	.name = "ipi_dev",
	.bus = NULL,
	.num_regions = 1,
	.regions = {
		{
			.virt = (void*)IPI_BASE_ADDR,
			.physmap = &ipi_phys_addr,
			.size = 0x1000,
			.page_shift = -1UL,
			.page_mask = -1UL,
			.mem_flags = DEVICE_NONSHARED | PRIV_RW_USER_RW,
			.ops = {NULL},
		}
	},
	.node = {NULL},
	.irq_num = 1,
	.irq_info = (void *)IPI_IRQ_VECT_ID,
};

struct ipi_info {
	const char *name; /* IPI device name */
	const char *bus_name; /* IPI bus name */
	uint32_t ipi_chn_mask; /* IPI channel mask */
};

struct ipi_info ipi_info = {
	.name = IPI_DEV_NAME,
	.bus_name = IPI_BUS_NAME,
	.ipi_chn_mask = IPI_CHN_BITMASK,
};

struct remoteproc_priv {
	struct remoteproc rproc;
	struct metal_device *ipi_dev;
	struct metal_io_region *ipi_io;
	unsigned int ipi_chn_mask;
	atomic_int ipi_nokick;
};

static int zynqmp_r5_a53_proc_irq_handler(int vect_id, void *data)
{
	struct remoteproc_priv *prproc = data;
	unsigned int ipi_intr_status;

	(void)vect_id;
	if (!prproc)
		return METAL_IRQ_NOT_HANDLED;
	ipi_intr_status = (unsigned int)metal_io_read32(prproc->ipi_io,
							IPI_ISR_OFFSET);
	if (ipi_intr_status & prproc->ipi_chn_mask) {
		atomic_flag_clear(&prproc->ipi_nokick);
		metal_io_write32(prproc->ipi_io, IPI_ISR_OFFSET,
				 prproc->ipi_chn_mask);
		return METAL_IRQ_HANDLED;
	}
	return METAL_IRQ_NOT_HANDLED;
}

static struct remoteproc *
zynqmp_r5_a53_proc_init(struct remoteproc_ops *ops, void *arg)
{
	struct ipi_info *ipi = arg;
	struct remoteproc_priv *prproc;
	struct metal_device *ipi_dev;
	unsigned int irq_vect;
	int ret;

	if (!ipi || !ops)
		return NULL;
	prproc = metal_allocate_memory(sizeof(*prproc));
	if (!prproc)
		return NULL;
	memset(prproc, 0, sizeof(*prproc));
	ret = metal_device_open(ipi->bus_name, ipi->name, &ipi_dev);
	if (ret) {
		xil_printf("failed to open ipi device: %d.\r\n", ret);
		goto err1;
	}
	prproc->ipi_dev = ipi_dev;
	prproc->ipi_io = metal_device_io_region(ipi_dev, 0);
	if (!prproc->ipi_io)
		goto err2;
	prproc->ipi_chn_mask = ipi->ipi_chn_mask;
	atomic_store(&prproc->ipi_nokick, 1);
	prproc->rproc.ops = ops;

	/* Register interrupt handler and enable interrupt */
	irq_vect = IPI_IRQ_VECT_ID;
	metal_irq_register(irq_vect, zynqmp_r5_a53_proc_irq_handler,
			   ipi_dev, prproc);
	metal_irq_enable(irq_vect);
	metal_io_write32(prproc->ipi_io, IPI_IER_OFFSET,
			 prproc->ipi_chn_mask);
	return &prproc->rproc;
err2:
	metal_device_close(ipi_dev);
err1:
	metal_free_memory(prproc);
	return NULL;
}

static void zynqmp_r5_a53_proc_remove(struct remoteproc *rproc)
{
	struct remoteproc_priv *prproc;

	if (!rproc)
		return;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	metal_io_write32(prproc->ipi_io, IPI_IDR_OFFSET, prproc->ipi_chn_mask);
	metal_irq_disable(IPI_IRQ_VECT_ID);
	metal_irq_unregister(IPI_IRQ_VECT_ID, NULL, NULL, NULL);
	if (prproc->ipi_dev)
		metal_device_close(prproc->ipi_dev);
	metal_free_memory(prproc);
}

static void *
zynqmp_r5_a53_proc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
			metal_phys_addr_t *da, size_t size,
			unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_mem *mem;
	metal_phys_addr_t lpa, lda;
	struct metal_io_region *tmpio;

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
	if (!mem)
		return NULL;
	mem->pa = lpa;
	mem->da = lda;
	mem->size = size;
	tmpio = metal_allocate_memory(sizeof(*tmpio));
	if (!tmpio) {
		metal_free_memory(mem);
		return NULL;
	}
	/* va is the same as pa in this platform */
	metal_io_init(tmpio, (void *)mem->pa, &mem->pa, size,
		      sizeof(metal_phys_addr_t)<<3, attribute, NULL);
	mem->io = tmpio;
	metal_list_add_tail(&rproc->mems, &mem->node);
	*pa = lpa;
	*da = lda;
	*io = tmpio;
	return metal_io_phys_to_virt(tmpio, mem->pa);
}

static int zynqmp_r5_a53_proc_notify(struct remoteproc *rproc, uint32_t id)
{
	struct remoteproc_priv *prproc;

	(void)id;
	if (!rproc)
		return -1;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);

	/* TODO: use IPI driver instead and pass ID */
	metal_io_write32(prproc->ipi_io, IPI_TRIG_OFFSET,
			  prproc->ipi_chn_mask);
	return 0;
}

/* processor operations from r5 to a53. It defines
 * notification operation and remote processor managementi operations. */
static struct remoteproc_ops zynqmp_r5_a53_proc_ops = {
	.init = zynqmp_r5_a53_proc_init,
	.remove = zynqmp_r5_a53_proc_remove,
	.mmap = zynqmp_r5_a53_proc_mmap,
	.notify = zynqmp_r5_a53_proc_notify,
	.start = NULL,
	.stop = NULL,
	.shutdown = NULL,
};


struct remoteproc *platform_create_proc(int proc_index, int rsc_index)
{
	struct remoteproc *rproc;
	void *rsc_table;
	int rsc_size;
	int ret;
	metal_phys_addr_t pa;

	(void) proc_index;
	rsc_table = get_resource_table(rsc_index, &rsc_size);

	/* Register IPI device */
	(void)metal_register_generic_device(&ipi_device);
	/* Initialize remoteproc instance */
	rproc = remoteproc_init(&zynqmp_r5_a53_proc_ops, &ipi_info);
	if (!rproc) {
		xil_printf("Failed to intialize remoteproc\r\n");
		return NULL;
	}

	/*
	 * Mmap shared memories
	 * Or shall we constraint that they will be set as carved out
	 * in the resource table?
	 */
	/* mmap resource table */
	pa = (metal_phys_addr_t)rsc_table;
	(void *)remoteproc_mmap(rproc, &pa,
				NULL, rsc_size,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				&rproc->rsc_io);
	/* mmap shared memory */
	pa = SHARED_BUF_PA;
	(void *)remoteproc_mmap(rproc, &pa,
				NULL, SHARED_BUF_SIZE,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				NULL);

	/* parse resource table to remoteproc */
	ret = remoteproc_set_rsc_table(rproc, rsc_table, rsc_size);
	if (ret) {
		xil_printf("Failed to parse resource table\r\n");
		remoteproc_remove(rproc);
		return NULL;
	}
	xil_printf("Successfully parse resource table\r\n");

	return rproc;
}

struct  rpmsg_device *
platform_create_rpmsg_vdev(struct remoteproc *rproc, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev))
{
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct virtio_device *vdev;
	void *shbuf;
	struct metal_io_region *shbuf_io;
	int ret;

	rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
	if (!rpmsg_vdev)
		return NULL;
	shbuf_io = remoteproc_get_io_with_pa(rproc, SHARED_BUF_PA);
	if (!shbuf_io)
		return NULL;
	shbuf = metal_io_phys_to_virt(shbuf_io, SHARED_BUF_PA);

	/* TODO: can we have a wrapper for the following two functions? */
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev) {
		xil_printf("failed remoteproc_create_virtio\r\n");
		goto err1;
	}

	ret =  rpmsg_init_vdev(rpmsg_vdev, vdev, NULL, shbuf_io, shbuf,
			       SHARED_BUF_SIZE);
	if (ret) {
		xil_printf("failed rpmsg_init_vdev\r\n");
		goto err2;
	}
	return rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);
err2:
	remoteproc_remove_virtio(rproc, vdev);
err1:
	metal_free_memory(rpmsg_vdev);
	return NULL;
}

int platform_poll(void *priv)
{
	struct remoteproc *rproc = priv;
	struct remoteproc_priv *prproc;
	unsigned int flags;

	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	while(1) {
		flags = metal_irq_save_disable();
		if (!(atomic_flag_test_and_set(&prproc->ipi_nokick))) {
			metal_irq_restore_enable(flags);
			remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
		}
		_rproc_wait();
		metal_irq_restore_enable(flags);
	}
}
