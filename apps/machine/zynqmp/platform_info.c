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
#include <metal/alloc.h>
#include <metal/atomic.h>
#include <metal/io.h>
#include <metal/irq.h>
#include <metal/device.h>
#include <metal/utilities.h>
#include <openamp/remoteproc.h>
#include <openamp/rpmsg_virtio.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/un.h>
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
#define RSC_DEV_NAME        "3ed00000.ddr_fw" /* RSC device name */
#define SHM_DEV_NAME        "3ed04000.shm" /* shared device name */

#define RSC_MEM_PA 0x3ED02000UL
#define RSC_MEM_SIZE 0x2000UL
#define VRING_MEM_PA  0x3ED40000UL
#define VRING_MEM_SIZE 0x8000UL
#define SHARED_BUF_PA 0x3ED48000UL
#define SHARED_BUF_SIZE 0x40000UL

/* IPI REGs OFFSET */
#define IPI_TRIG_OFFSET          0x00000000    /* IPI trigger register offset */
#define IPI_OBS_OFFSET           0x00000004    /* IPI observation register offset */
#define IPI_ISR_OFFSET           0x00000010    /* IPI interrupt status register offset */
#define IPI_IMR_OFFSET           0x00000014    /* IPI interrupt mask register offset */
#define IPI_IER_OFFSET           0x00000018    /* IPI interrupt enable register offset */
#define IPI_IDR_OFFSET           0x0000001C    /* IPI interrupt disable register offset */

struct remoteproc_priv {
	const char *ipi_name; /**< IPI device name */
	const char *ipi_bus_name; /**< IPI bus name */
	const char *rsc_name; /**< rsc device name */
	const char *rsc_bus_name; /**< rsc bus name */
	const char *shm_name; /**< shared memory device name */
	const char *shm_bus_name; /**< shared memory bus name */
	struct metal_device *ipi_dev; /**< pointer to IPI device */
	struct metal_io_region *ipi_io; /**< pointer to IPI i/o region */
	struct metal_device *rsc_dev; /**< pointer to rsc memory device */
	struct metal_io_region *rsc_io; /**< pointer to rsc memory i/o
					     region */
	struct remoteproc_mem rsc_mem; /**< rsc memory */
	struct metal_device *shm_dev; /**< pointer to shared memory device */
	struct metal_io_region *shm_io; /**< pointer to shared memory i/o
					     region */
	struct remoteproc_mem shm_mem; /**< shared memory */
	unsigned int ipi_chn_mask; /**< IPI channel mask */
	atomic_int ipi_nokick;
};

struct remoteproc_priv rproc_priv = {
	.ipi_name = IPI_DEV_NAME,
	.ipi_bus_name = DEV_BUS_NAME,
	.ipi_chn_mask = IPI_CHN_BITMASK,
	.rsc_name = RSC_DEV_NAME,
	.rsc_bus_name = DEV_BUS_NAME,
	.shm_name = SHM_DEV_NAME,
	.shm_bus_name = DEV_BUS_NAME,
};

static struct remoteproc rproc_inst;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

#define _rproc_wait() metal_cpu_yield()

static int zynqmp_linux_r5_proc_irq_handler(int vect_id, void *data)
{
	struct remoteproc *rproc = data;
	struct remoteproc_priv *prproc;
	unsigned int ipi_intr_status;

	(void)vect_id;
	if (!rproc)
		return METAL_IRQ_NOT_HANDLED;
	prproc = rproc->priv;
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
zynqmp_linux_r5_proc_init(struct remoteproc *rproc,
			  struct remoteproc_ops *ops, void *arg)
{
	struct remoteproc_priv *prproc = arg;
	struct metal_device *dev;
	unsigned int irq_vect;
	metal_phys_addr_t mem_pa;
	int ret;

	if (!rproc || !prproc || !ops)
		return NULL;
	rproc->priv = prproc;
	rproc->ops = ops;
	prproc->ipi_dev = NULL;
	prproc->rsc_dev = NULL;
	prproc->shm_dev = NULL;
	/* Get resource table memory device */
	ret = metal_device_open(prproc->rsc_bus_name, prproc->rsc_name,
				&dev);
	if (ret) {
		fprintf(stderr, "ERROR: failed to open rsc device: %d.\n", ret);
		return NULL;
	}
	prproc->rsc_dev = dev;
	prproc->rsc_io = metal_device_io_region(dev, 0);
	if (!prproc->rsc_io)
		goto err1;
	mem_pa = metal_io_phys(prproc->rsc_io, 0);
	remoteproc_init_mem(&prproc->rsc_mem, "rsc", mem_pa, mem_pa,
			    metal_io_region_size(prproc->rsc_io),
			    prproc->rsc_io);
	remoteproc_add_mem(rproc, &prproc->rsc_mem);
	printf("Successfully added rsc memory.\n");
	/* Get shared memory device */
	ret = metal_device_open(prproc->shm_bus_name, prproc->shm_name,
				&dev);
	if (ret) {
		fprintf(stderr, "ERROR: failed to open shm device: %d.\n", ret);
		goto err1;
	}
	printf("Successfully open shm device.\n");
	prproc->shm_dev = dev;
	prproc->shm_io = metal_device_io_region(dev, 0);
	if (!prproc->shm_io)
		goto err2;
	mem_pa = metal_io_phys(prproc->shm_io, 0);
	remoteproc_init_mem(&prproc->shm_mem, "shm", mem_pa, mem_pa,
			    metal_io_region_size(prproc->shm_io),
			    prproc->shm_io);
	remoteproc_add_mem(rproc, &prproc->shm_mem);
	printf("Successfully added shared memory\n");
	/* Get IPI device */
	ret = metal_device_open(prproc->ipi_bus_name, prproc->ipi_name,
				&dev);
	if (ret) {
		printf("failed to open ipi device: %d.\n", ret);
		goto err2;
	}
	prproc->ipi_dev = dev;
	prproc->ipi_io = metal_device_io_region(dev, 0);
	if (!prproc->ipi_io)
		goto err3;
	printf("Successfully probed IPI device\n");
	atomic_store(&prproc->ipi_nokick, 1);

	/* Register interrupt handler and enable interrupt */
	irq_vect = (uintptr_t)dev->irq_info;
	metal_irq_register(irq_vect, zynqmp_linux_r5_proc_irq_handler,
			   dev, rproc);
	metal_irq_enable(irq_vect);
	metal_io_write32(prproc->ipi_io, IPI_IER_OFFSET,
			 prproc->ipi_chn_mask);
	printf("Successfully initialized Linux r5 remoteproc.\n");
	return rproc;
err3:
	metal_device_close(prproc->ipi_dev);
err2:
	metal_device_close(prproc->shm_dev);
err1:
	metal_device_close(prproc->rsc_dev);
	return NULL;
}

static void zynqmp_linux_r5_proc_remove(struct remoteproc *rproc)
{
	struct remoteproc_priv *prproc;
	struct metal_device *dev;

	if (!rproc)
		return;
	prproc = rproc->priv;
	metal_io_write32(prproc->ipi_io, IPI_IDR_OFFSET, prproc->ipi_chn_mask);
	dev = prproc->ipi_dev;
	if (dev) {
		metal_irq_disable((uintptr_t)dev->irq_info);
		metal_irq_unregister((uintptr_t)dev->irq_info, NULL, NULL,
				     NULL);
		metal_device_close(dev);
	}
	if (prproc->shm_dev)
		metal_device_close(prproc->shm_dev);
	if (prproc->rsc_dev)
		metal_device_close(prproc->rsc_dev);
}

static void *
zynqmp_linux_r5_proc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
			  metal_phys_addr_t *da, size_t size,
			  unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_priv *prproc;
	metal_phys_addr_t lpa, lda;
	struct metal_io_region *tmpio;

	(void)attribute;
	(void)size;
	if (!rproc)
		return NULL;
	prproc = rproc->priv;
	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;
	tmpio = prproc->shm_io;
	if (!tmpio)
		return NULL;

	*pa = lpa;
	*da = lda;
	if (io)
		*io = tmpio;
	return metal_io_phys_to_virt(tmpio, lpa);
}

static int zynqmp_linux_r5_proc_notify(struct remoteproc *rproc, uint32_t id)
{
	struct remoteproc_priv *prproc;

	(void)id;
	if (!rproc)
		return -1;
	prproc = rproc->priv;

	/* TODO: use IPI driver instead and pass ID */
	metal_io_write32(prproc->ipi_io, IPI_TRIG_OFFSET,
			  prproc->ipi_chn_mask);
	return 0;
}

/* processor operations from r5 to a53. It defines
 * notification operation and remote processor managementi operations. */
static struct remoteproc_ops zynqmp_linux_r5_proc_ops = {
	.init = zynqmp_linux_r5_proc_init,
	.remove = zynqmp_linux_r5_proc_remove,
	.mmap = zynqmp_linux_r5_proc_mmap,
	.notify = zynqmp_linux_r5_proc_notify,
	.start = NULL,
	.stop = NULL,
	.shutdown = NULL,
};

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

static struct remoteproc *
platform_create_proc(int proc_index, int rsc_index)
{
	void *rsc_table;
	int rsc_size;
	int ret;
	metal_phys_addr_t pa;

	(void)proc_index;
	(void)rsc_index;
	rsc_size = RSC_MEM_SIZE;

	/* Initialize remoteproc instance */
	if (!remoteproc_init(&rproc_inst, &zynqmp_linux_r5_proc_ops,
			     &rproc_priv))
		return NULL;
	printf("Successfully initialized remoteproc\n");

	/* Mmap resource table */
	pa = RSC_MEM_PA;
	printf("Calling mmap resource table.\n");
	rsc_table = remoteproc_mmap(&rproc_inst, &pa, NULL, rsc_size,
				    0, NULL);
	if (!rsc_table) {
		fprintf(stderr, "ERROR: Failed to mmap resource table.\n");
		return NULL;
	}
	printf("Successfully mmap resource table.\n");
	/* parse resource table to remoteproc */
	ret = remoteproc_set_rsc_table(&rproc_inst, rsc_table, rsc_size);
	if (ret) {
		printf("Failed to intialize remoteproc\n");
		remoteproc_remove(&rproc_inst);
		return NULL;
	}
	printf("Successfully set resource table to remoteproc.\n");

	return &rproc_inst;
}

int platform_init(int argc, char *argv[], void **platform)
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
	struct remoteproc *rproc;

	if (!platform) {
		fprintf(stderr, "Failed to initialize platform, NULL pointer"
			"to store platform data.\n");
		return -EINVAL;
	}
	/* Initialize HW system components */
	init_system();

	if (argc >= 2) {
		proc_id = strtoul(argv[1], NULL, 0);
	}

	if (argc >= 3) {
		rsc_id = strtoul(argv[2], NULL, 0);
	}

	rproc = platform_create_proc(proc_id, rsc_id);
	if (!rproc) {
		fprintf(stderr, "Failed to create remoteproc device.\n");
		return -EINVAL;
	}
	*platform = rproc;
	return 0;
}

struct  rpmsg_device *
platform_create_rpmsg_vdev(void *platform, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_unbound_service_cb unbound_svc_cb)
{
	struct remoteproc *rproc = platform;
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
	shbuf = metal_io_phys_to_virt(shbuf_io,
				      SHARED_BUF_PA);

	printf("Creating virtio...\n");
	/* TODO: can we have a wrapper for the following two functions? */
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev) {
		printf("failed remoteproc_create_virtio\n");
		goto err1;
	}
	printf("Successfully created virtio device.\n");

	/* Only RPMsg virtio master needs to initialize the shared buffers pool */
	rpmsg_virtio_init_shm_pool(&shpool, shbuf, SHARED_BUF_SIZE);

	printf("initializing rpmsg vdev\r\n");
	/* RPMsg virtio slave can set shared buffers pool argument to NULL */
	ret = rpmsg_init_vdev(rpmsg_vdev, vdev, unbound_svc_cb,
			      shbuf_io, &shpool);
	if (ret) {
		printf("failed rpmsg_init_vdev\r\n");
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

	prproc = rproc->priv;
	while(1) {
		flags = metal_irq_save_disable();
		if (!(atomic_flag_test_and_set(&prproc->ipi_nokick))) {
			metal_irq_restore_enable(flags);
			remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
			break;
		}
		_rproc_wait();
		metal_irq_restore_enable(flags);
	}
	return 0;
}

void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev)
{
	(void)rpdev;
}

void platform_cleanup(void *platform)
{
	struct remoteproc *rproc = platform;

	if (rproc)
		remoteproc_remove(rproc);
	cleanup_system();
}
