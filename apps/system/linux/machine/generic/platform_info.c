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

#include <metal/alloc.h>
#include <metal/atomic.h>
#include <metal/io.h>
#include <metal/irq.h>
#include <metal/shmem.h>
#include <metal/utilities.h>
#include <openamp/remoteproc.h>
#include <openamp/rpmsg_virtio.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include "rsc_table.h"

#define IPI_CHAN_NUMS 2
#define IPI_CHAN_SEND 0
#define IPI_CHAN_RECV 1
#define UNIX_PREFIX "unix:"
#define UNIXS_PREFIX "unixs:"

#define RSC_MEM_PA  0x0UL
#define SHARED_BUF_PA   0x10000UL
#define SHARED_BUF_SIZE 0x40000UL

#define _rproc_wait() metal_cpu_yield()

struct vring_ipi_info {
	/* Socket file path */
	const char *path;
	int fd;
	atomic_int sync;
};

struct remote_info {
	const char *shm_file;
	int shm_size;
	const char *ipi;
};

struct remoteproc_priv {
	struct remoteproc rproc;
	struct metal_io_region *shm_old_io;
	struct metal_io_region shm_new_io;
	struct remoteproc_mem shm;
	struct vring_ipi_info ipi;
};

static struct remote_info remote_info_table [] = {
	{
		.shm_file = "openamp.shm",
		.shm_size = 0x100000,
		.ipi = "unixs:/tmp/openamp.event.0",
	},
	{
		.shm_file = "openamp.shm",
		.shm_size = 0x100000,
		.ipi = "unix:/tmp/openamp.event.0",
	},
};

static int linux_proc_block_read(struct metal_io_region *io,
				 unsigned long offset,
				 void *restrict dst,
				 memory_order order,
				 int len)
{
	void *src = metal_io_virt(io, offset);

	(void)order;
	(void)memcpy(dst, src, len);
	return len;
}

static int linux_proc_block_write(struct metal_io_region *io,
				  unsigned long offset,
				  const void *restrict src,
				  memory_order order,
				  int len)
{
	void *dst = metal_io_virt(io, offset);

	(void)order;
	(void)memcpy(dst, src, len);
	return len;
}

static void linux_proc_block_set(struct metal_io_region *io,
				unsigned long offset,
				unsigned char value,
				memory_order order,
				int len)
{
	void *dst = metal_io_virt(io, offset);

	(void)order;
	(void)memset(dst, value, len);
	return;
}

static struct metal_io_ops linux_proc_io_ops = {
	.write = NULL,
	.read = NULL,
	.block_read = linux_proc_block_read,
	.block_write = linux_proc_block_write,
	.block_set = linux_proc_block_set,
	.close = NULL,
};

static int sk_unix_client(const char *descr)
{
	struct sockaddr_un addr;
	int fd;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	memset(&addr, 0, sizeof addr);
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, descr + strlen(UNIX_PREFIX),
		sizeof addr.sun_path);
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
		printf("connected to %s\n", descr + strlen(UNIX_PREFIX));
		return fd;
	}

	close(fd);
	return -1;
}

static int sk_unix_server(const char *descr)
{
	struct sockaddr_un addr;
	int fd, nfd;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, descr + strlen(UNIXS_PREFIX),
		sizeof addr.sun_path);
	unlink(addr.sun_path);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		goto fail;
	}

	listen(fd, 5);
	printf("Waiting for connection on %s\n", addr.sun_path);
	nfd = accept(fd, NULL, NULL);
	close(fd);
	return nfd;
fail:
	close(fd);
	return -1;
}

static inline int is_sk_unix_server(const char *descr)
{
	if (memcmp(UNIXS_PREFIX, descr, strlen(UNIXS_PREFIX)))
		return 0;
	else
		return 1;
}

static int event_open(const char *descr)
{
	int fd = -1;
	int i;

	if (descr == NULL) {
		return fd;
	}

	if (!is_sk_unix_server(descr)) {
		/* UNIX client.  Retry to connect a few times to give the peer
		 *  a chance to setup.  */
		for (i = 0; i < 100 && fd == -1; i++) {
			fd = sk_unix_client(descr);
			if (fd == -1)
				usleep(i * 10 * 1000);
		}
	} else {
		/* UNIX server. */
		fd = sk_unix_server(descr);
	}
	printf("Open IPI: %s\n", descr);
	return fd;
}

static int linux_proc_irq_handler(int vect_id, void *data)
{
	char dummy_buf[32];
	struct vring_ipi_info *ipi = data;

	read(vect_id, dummy_buf, sizeof(dummy_buf));
	atomic_flag_clear(&ipi->sync);
	return 0;
}

static struct remoteproc *
linux_proc_init(struct remoteproc_ops *ops, void *arg)
{
	struct remote_info *rinfo = arg;
	struct remoteproc_priv *prproc;
	struct metal_io_region *io;
	struct remoteproc_mem *shm;
	struct vring_ipi_info *ipi;
	int ret;

	if (!rinfo)
		return NULL;
	prproc = metal_allocate_memory(sizeof(*prproc));
	if (!prproc) {
		fprintf(stderr, "Failed to allocate memory for rproc.\r\n");
		return NULL;
	}
	/* Create shared memory io */
	ret = metal_shmem_open(rinfo->shm_file, rinfo->shm_size, &io);
	if (ret) {
		printf("Failed to init rproc, failed to open shm %s.\n",
		       rinfo->shm_file);
		return NULL;
	}
	prproc->shm_old_io = io;
	shm = &prproc->shm;
	shm->pa = 0;
	shm->da = 0;
	shm->size = rinfo->shm_size;
	metal_io_init(&prproc->shm_new_io, io->virt, &shm->pa,
		      shm->size, -1, 0, &linux_proc_io_ops);
	shm->io = &prproc->shm_new_io;

	/* Open IPI */
	prproc->ipi.path = rinfo->ipi;
	ipi = &prproc->ipi;
	ipi->fd = event_open(ipi->path);
	if (ipi->fd < 0) {
		fprintf(stderr,
			"ERROR: Failed to open sock %s for IPI.\n",
			ipi->path);
		goto err;
	}
	metal_irq_register(ipi->fd, linux_proc_irq_handler, NULL, ipi);
	prproc->rproc.ops = ops;
	return &prproc->rproc;

err:
	metal_free_memory(prproc);
	return NULL;
}


static void linux_proc_remove(struct remoteproc *rproc)
{
	struct remoteproc_priv *prproc;
	struct vring_ipi_info *ipi;
	struct metal_io_region *io;

	if (!rproc)
		return;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);

	/* Close IPI */
	ipi = &prproc->ipi;
	if (ipi->fd >= 0) {
		metal_irq_unregister(ipi->fd, 0, NULL, ipi);
		close(ipi->fd);
	}

	/* Close shared memory */
	io = prproc->shm_old_io;
	if (io && io->ops.close) {
		io->ops.close(io);
		prproc->shm_old_io = NULL;
	}
	metal_free_memory(prproc);
}

static void *
linux_proc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
		metal_phys_addr_t *da, size_t size,
		unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_mem *mem;
	struct remoteproc_priv *prproc;
	metal_phys_addr_t lpa, lda;
	void *va;

	(void)attribute;
	(void)size;
	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;

	if (!rproc)
		return NULL;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	mem = &prproc->shm;
	va = metal_io_phys_to_virt(mem->io, lpa);
	if (va) {
		if (io)
			*io = mem->io;
		metal_list_add_tail(&rproc->mems, &mem->node);
	}
	return va;
}

static int linux_proc_notify(struct remoteproc *rproc, uint32_t id)
{
	struct remoteproc_priv *prproc;
	struct vring_ipi_info *ipi;
	char dummy = 1;

	(void)id;
	if (!rproc)
		return -1;
	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	ipi = &prproc->ipi;
	send(ipi->fd, &dummy, 1, MSG_NOSIGNAL);
	return 0;
}

/* processor operations from r5 to a53. It defines
 * notification operation and remote processor managementi operations. */
static struct remoteproc_ops linux_proc_ops = {
	.init = linux_proc_init,
	.remove = linux_proc_remove,
	.mmap = linux_proc_mmap,
	.notify = linux_proc_notify,
	.start = NULL,
	.stop = NULL,
	.shutdown = NULL,
};

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

struct remoteproc *platform_create_proc(int proc_index, int rsc_index)
{
	struct remoteproc *rproc;
	struct remote_info *rinfo;
	void *rsc_table, *rsc_table_shm;
	int rsc_size;
	int ret;
	metal_phys_addr_t pa;

	(void)proc_index;
	rsc_table = get_resource_table(rsc_index, &rsc_size);

	/* Initialize remoteproc instance */
	rinfo = &remote_info_table[proc_index];
	rproc = remoteproc_init(&linux_proc_ops, rinfo);
	if (!rproc)
		return NULL;

	/* Mmap resource table */
	pa = RSC_MEM_PA;
	rsc_table_shm = remoteproc_mmap(rproc, &pa, NULL, rsc_size,
					0, &rproc_inst.rsc_io);
	/* Setup resource table
	 * This step can be done out of the application.
	 * Assumes the unix server side setup resource table. */
	if (is_sk_unix_server(rinfo->ipi))
		memcpy(rsc_table_shm, rsc_table, rsc_size);
	else
		/* Sleep to wait for the other side to finish initializing rsc */
		sleep(1);

	/* parse resource table to remoteproc */
	ret = remoteproc_set_rsc_table(rproc, rsc_table_shm, rsc_size);
	if (ret) {
		printf("Failed to set resource table to remoteproc\r\n");
		remoteproc_remove(rproc);
		return NULL;
	}
	printf("Initialize remoteproc successfully.\r\n");
	return rproc;
}

struct  rpmsg_device *
platform_create_rpmsg_vdev(struct remoteproc *rproc, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ept_create_cb new_endpoint_cb)
{
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct virtio_device *vdev;
	void *shbuf;
	struct metal_io_region *shbuf_io;
	int ret;

	/* Setup resource table */
	rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
	if (!rpmsg_vdev)
		return NULL;
	shbuf_io = remoteproc_get_io_with_pa(rproc, SHARED_BUF_PA);
	if (!shbuf_io)
		return NULL;
	shbuf = metal_io_phys_to_virt(shbuf_io, SHARED_BUF_PA);

	printf("creating remoteproc virtio\r\n");
	/* TODO: can we have a wrapper for the following two functions? */
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev) {
		printf("failed remoteproc_create_virtio\r\n");
		goto err1;
	}

	printf("initializing rpmsg shared buffer pool\r\n");
	/* Only RPMsg virtio master needs to initialize the shared buffers pool */
	rpmsg_virtio_init_shm_pool(&shpool, shbuf, SHARED_BUF_SIZE);

	printf("initializing rpmsg vdev\r\n");
	/* RPMsg virtio slave can set shared buffers pool argument to NULL */
	ret =  rpmsg_init_vdev(rpmsg_vdev, vdev, new_endpoint_cb,
			       shbuf_io,
			       &shpool);
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
	struct vring_ipi_info *ipi;
	unsigned int flags;

	prproc = metal_container_of(rproc, struct remoteproc_priv, rproc);
	ipi = &prproc->ipi;
	while(1) {
		flags = metal_irq_save_disable();
		if (!(atomic_flag_test_and_set(&ipi->sync))) {
			metal_irq_restore_enable(flags);
			remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
			break;
		}
		_rproc_wait();
		metal_irq_restore_enable(flags);
	}
	return 0;
}
