/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/alloc.h>
#include <metal/log.h>
#include <metal/utilities.h>
#include <openamp/elf_loader.h>
#include <openamp/remoteproc.h>
#include <openamp/remoteproc_loader.h>
#include <openamp/remoteproc_virtio.h>
#include <openamp/rsc_table_parser.h>

/******************************************************************************
 *  static functions
 *****************************************************************************/
static struct loader_ops *
remoteproc_check_fw_format(void *fw, struct image_store_ops *store_ops)
{
	unsigned char header[64];
	long lsize;

	lsize = store_ops->load(fw, 0, header, sizeof(header), NULL,
				SYNC_LOAD);
	if (lsize <= 0)
		return NULL;
	else if (elf_identify(header))
		return &elf_ops;
	else
		return NULL;
}

static struct remoteproc_mem *
remoteproc_get_mem(struct remoteproc *rproc, const char *name,
		   metal_phys_addr_t pa, metal_phys_addr_t da,
		   void *va, size_t size)
{
	struct metal_list *node;
	struct remoteproc_mem *mem;

	metal_list_for_each(&rproc->mems, node) {
		mem = metal_container_of(node, struct remoteproc_mem, node);
		if (name) {
			if (!strncmp(name, mem->name, sizeof(mem->name)))
				return mem;
		} else if (pa != METAL_BAD_PHYS) {
			metal_phys_addr_t pa_start, pa_end;

			pa_start = mem->pa;
			pa_end = pa_start + mem->size;
			if (pa >= pa_start && (pa + size) <= pa_end)
				return mem;
		} else if (pa != METAL_BAD_PHYS) {
			metal_phys_addr_t da_start, da_end;

			da_start = mem->da;
			da_end = da_start + mem->size;
			if (da >= da_start && (da + size) <= da_end)
				return mem;
		} else if (va) {
			if (metal_io_virt_to_offset(mem->io, va) !=
			    METAL_BAD_OFFSET)
				return mem;

		} else {
			return NULL;
		}
	}
	return NULL;
}

static metal_phys_addr_t
remoteproc_datopa(struct remoteproc_mem *mem, metal_phys_addr_t da)
{
	metal_phys_addr_t pa;

	pa = mem->pa + da - mem->da;
	return pa;
}

static metal_phys_addr_t
remoteproc_patoda(struct remoteproc_mem *mem, metal_phys_addr_t pa)
{
	metal_phys_addr_t da;

	da = mem->da + pa - mem->pa;
	return da;
}

static void *remoteproc_get_rsc_table(struct remoteproc *rproc,
				      void *store, void *loader_info,
				      struct loader_ops *loader_ops,
				      struct image_store_ops *store_ops,
				      size_t *rsc_len_ptr,
				      metal_phys_addr_t *rsc_da_ptr)
{
	long rsc_len;
	int ret;
	void *rsc_table = NULL;

	/* no resource table is found */
	rsc_len = loader_ops->get_rsc_table(loader_info, rsc_da_ptr);
	if (rsc_len <= 0) {
		*rsc_len_ptr = 0;
		return NULL;
	}
	*rsc_len_ptr = (size_t)rsc_len;

	/* Copy the resource table to local memory,
	 * the caller should be responsible to release the memory
	 */
	rsc_table = metal_allocate_memory(rsc_len);
	if (!rsc_table) {
		return RPROC_ERR_PTR(-RPROC_ENOMEM);
	}
	if (!loader_ops->copy_rsc_table(store, loader_info, store_ops,
					rsc_table)) {
		rsc_table = RPROC_ERR_PTR(-RPROC_EINVAL);
		goto error;
	}

	ret = handle_rsc_table(rproc, rsc_table, rsc_len, NULL);
	if (ret < 0) {
		rsc_table = RPROC_ERR_PTR(ret);
		goto error;
	}
	return rsc_table;

error:
	metal_free_memory(rsc_table);
	return rsc_table;
}

int remoteproc_parse_rsc_table(struct remoteproc *rproc,
			       struct resource_table *rsc_table,
			       size_t rsc_size)
{
	struct metal_io_region *io;

	io = remoteproc_get_io_with_va(rproc, (void *)rsc_table);
	return handle_rsc_table(rproc, rsc_table, rsc_size, io);
}

int remoteproc_set_rsc_table(struct remoteproc *rproc,
			     struct resource_table *rsc_table,
			     size_t rsc_size)
{
	int ret;
	struct metal_io_region *io;

	io = remoteproc_get_io_with_va(rproc, (void *)rsc_table);
	if (!io)
		return -EINVAL;
	ret = remoteproc_parse_rsc_table(rproc, rsc_table, rsc_size);
	if (!ret) {
		rproc->rsc_table = rsc_table;
		rproc->rsc_len = rsc_size;
		rproc->rsc_io = io;
	}
	return ret;

}

struct remoteproc *remoteproc_init(struct remoteproc *rproc,
				   struct remoteproc_ops *ops, void *priv)
{
	if (rproc) {
		rproc->state = RPROC_OFFLINE;
		metal_mutex_init(&rproc->lock);
		metal_list_init(&rproc->mems);
		metal_list_init(&rproc->vdevs);
	}
	rproc = ops->init(rproc, ops, priv);
	return rproc;
}

int remoteproc_remove(struct remoteproc *rproc)
{
	int ret;

	if (rproc) {
		metal_mutex_acquire(&rproc->lock);
		if (rproc->state == RPROC_OFFLINE)
			rproc->ops->remove(rproc);
		else
			ret = -EBUSY;
		metal_mutex_release(&rproc->lock);
	} else {
		ret = -EINVAL;
	}
	return ret;
}

int remoteproc_set_bootaddr(struct remoteproc *rproc,
			    metal_phys_addr_t bootaddr)
{
	if (rproc) {
		metal_mutex_acquire(&rproc->lock);
		rproc->bootaddr = bootaddr;
		metal_mutex_release(&rproc->lock);
	}
	return 0;
}

int remoteproc_start(struct remoteproc *rproc)
{
	int ret = -RPROC_ENODEV;

	if (rproc) {
		metal_mutex_acquire(&rproc->lock);
		if (rproc->state == RPROC_READY) {
			ret = rproc->ops->start(rproc);
			rproc->state = RPROC_RUNNING;
		} else {
			ret = -RPROC_EINVAL;
		}
		metal_mutex_release(&rproc->lock);
	}
	return ret;
}

int remoteproc_stop(struct remoteproc *rproc)
{
	int ret = -RPROC_ENODEV;

	if (rproc) {
		metal_mutex_acquire(&rproc->lock);
		if (rproc->state != RPROC_STOPPED &&
		    rproc->state != RPROC_OFFLINE) {
			if (rproc->ops->stop)
				ret = rproc->ops->stop(rproc);
			rproc->state = RPROC_STOPPED;
		} else {
			ret = 0;
		}
		metal_mutex_release(&rproc->lock);
	}
	return ret;
}

int remoteproc_shutdown(struct remoteproc *rproc)
{
	int ret = -RPROC_ENODEV;

	if (rproc) {
		ret = 0;
		metal_mutex_acquire(&rproc->lock);
		if (rproc->state != RPROC_OFFLINE) {
			if (rproc->state != RPROC_STOPPED) {
				if (rproc->ops->stop)
					ret = rproc->ops->stop(rproc);
			}
			if (!ret) {
				if (rproc->ops->shutdown)
					ret = rproc->ops->shutdown(rproc);
				if (!ret) {
					rproc->state = RPROC_OFFLINE;
				}
			}
		}
		metal_mutex_release(&rproc->lock);
	}
	return ret;
}

struct metal_io_region *
remoteproc_get_io_with_name(struct remoteproc *rproc,
			    const char *name)
{
	struct remoteproc_mem *mem;

	mem = remoteproc_get_mem(rproc, name,
				 METAL_BAD_PHYS, METAL_BAD_PHYS, NULL, 0);
	if (mem)
		return mem->io;
	else
		return NULL;
}

struct metal_io_region *
remoteproc_get_io_with_pa(struct remoteproc *rproc,
			  metal_phys_addr_t pa)
{
	struct remoteproc_mem *mem;

	mem = remoteproc_get_mem(rproc, NULL, pa, METAL_BAD_PHYS, NULL, 0);
	if (mem)
		return mem->io;
	else
		return NULL;
}

struct metal_io_region *
remoteproc_get_io_with_da(struct remoteproc *rproc,
			  metal_phys_addr_t da,
			  unsigned long *offset)
{
	struct remoteproc_mem *mem;

	mem = remoteproc_get_mem(rproc, NULL, METAL_BAD_PHYS, da, NULL, 0);
	if (mem) {
		struct metal_io_region *io;
		metal_phys_addr_t pa;

		io = mem->io;
		pa = remoteproc_datopa(mem, da);
		*offset = metal_io_phys_to_offset(io, pa);
		return io;
	} else {
		return NULL;
	}
}

struct metal_io_region *
remoteproc_get_io_with_va(struct remoteproc *rproc, void *va)
{
	struct remoteproc_mem *mem;

	mem = remoteproc_get_mem(rproc, NULL, METAL_BAD_PHYS, METAL_BAD_PHYS,
				 va, 0);
	if (mem)
		return mem->io;
	else
		return NULL;
}

void *remoteproc_mmap(struct remoteproc *rproc,
		      metal_phys_addr_t *pa, metal_phys_addr_t *da,
		      size_t size, unsigned int attribute,
		      struct metal_io_region **io)
{
	void *va = NULL;
	metal_phys_addr_t lpa, lda;
	struct remoteproc_mem *mem;

	if (!rproc)
		return NULL;
	else if (!pa && !da)
		return NULL;
	if (pa)
		lpa = *pa;
	else
		lpa = METAL_BAD_PHYS;
	if (da)
		lda =  *da;
	else
		lda = METAL_BAD_PHYS;
	mem = remoteproc_get_mem(rproc, NULL, lpa, lda, NULL, size);
	if (mem) {
		if (lpa != METAL_BAD_PHYS)
			lda = remoteproc_patoda(mem, lpa);
		else if (lda != METAL_BAD_PHYS)
			lpa = remoteproc_datopa(mem, lda);
		if (io)
			*io = mem->io;
		va = metal_io_phys_to_virt(mem->io, lpa);
	} else if (rproc->ops->mmap) {
		va = rproc->ops->mmap(rproc, &lpa, &lda, size, attribute, io);
	}

	if (pa)
		*pa  = lpa;
	if (da)
		*da = lda;
	return va;
}

int remoteproc_load(struct remoteproc *rproc, void *store, void **loader_data,
		    struct image_store_ops *store_ops)
{
	int ret;
	struct loader_ops *loader_ops;
	void *ldata;
	metal_phys_addr_t entry;
	metal_phys_addr_t rsc_da;
	size_t rsc_len;
	void *rsc_table;

	if (!rproc)
		return -RPROC_ENODEV;

	metal_mutex_acquire(&rproc->lock);
	metal_log(METAL_LOG_DEBUG, "%s: check remoteproc status\n\r", __func__);
	/* If remoteproc is running, cannot load firmware */
	if (rproc->state == RPROC_RUNNING || rproc->state == RPROC_ERROR) {
		metal_log(METAL_LOG_ERROR,
			  "load store failure: invalid rproc state %d.\n",
			  rproc->state);
		return -RPROC_EINVAL;
	}

	if (!store_ops) {
		metal_log(METAL_LOG_ERROR,
			  "load store failure: loader ops is not set.\n");
		return -RPROC_EINVAL;
	}

	/* Open firmware to get ready to parse */
	metal_log(METAL_LOG_DEBUG, "%s: open firmware image\n\r", __func__);
	ret = store_ops->open(store);
	if (ret) {
		metal_log(METAL_LOG_ERROR,
			  "load store failure: failed to open firmware.\n");
		return ret;
	}

	/* Check firmware format to select a parser */
	loader_ops = rproc->loader_ops;
	if (!loader_ops) {
		metal_log(METAL_LOG_DEBUG, "%s: check loader\n\r", __func__);
		loader_ops = remoteproc_check_fw_format(store, store_ops);
		if (!loader_ops) {
			metal_log(METAL_LOG_ERROR,
			       "load store failure: failed to get store ops.\n");
			ret = -RPROC_EINVAL;
			goto error1;
		}
		rproc->loader_ops = loader_ops;
	}

	/* parse the firmware, get the headers */
	metal_log(METAL_LOG_DEBUG, "%s: parse firmware\n\r", __func__);
	ldata = loader_ops->parse(store, store_ops);
	if (!ldata) {
		metal_log(METAL_LOG_ERROR,
			  "load store failure: failed to parse firmware.\n");
		ret = -RPROC_EINVAL;
		goto error2;
	}

	/* Try to get the resource table */
	metal_log(METAL_LOG_DEBUG, "%s: get resource table\n\r", __func__);
	rsc_table = remoteproc_get_rsc_table(rproc, store, ldata,
					     loader_ops, store_ops,
					     &rsc_len, &rsc_da);
	if (RPROC_IS_ERR(rsc_table)) {
		ret = RPROC_PTR_ERR(rsc_table);
		rsc_table = NULL;
		goto error2;
	}

	/* load firmware */
	metal_log(METAL_LOG_DEBUG, "%s: load firmware data\n\r", __func__);
	ret = loader_ops->load(store, ldata, rproc, store_ops);
	if (ret) {
		metal_log(METAL_LOG_ERROR,
			  "load store failure: failed to load firmware.\n");
		goto error3;
	}

	/* Update resource table */
	if (rsc_len && rsc_da != METAL_BAD_PHYS) {
		void *rsc_table_cp = rsc_table;
		struct metal_io_region *io;

		metal_log(METAL_LOG_DEBUG,
			  "%s: update resource table\n\r", __func__);
		rsc_table = remoteproc_mmap(rproc, NULL, &rsc_da,
					    rsc_len, 0, &io);
		if (rsc_table) {
			/* FIX me: Should use metal_io_block_write */
			memcpy(rsc_table, rsc_table_cp, rsc_len);
			rproc->rsc_table = rsc_table;
			rproc->rsc_len = rsc_len;
		} else {
			metal_log(METAL_LOG_WARNING,
				  "load store: not able to update rsc table.\n");
		}
		metal_free_memory(rsc_table_cp);
		/* So that the rsc_table will not get released */
		rsc_table = NULL;
	}

	metal_log(METAL_LOG_DEBUG, "%s: successfully load firmware\n\r",
		  __func__);
	/* get entry point from the firmware */
	entry = loader_ops->get_entry(ldata);
	rproc->bootaddr = entry;
	rproc->state = RPROC_READY;

	metal_mutex_release(&rproc->lock);
	if (loader_data)
		*loader_data = ldata;
	else
		loader_ops->close(store, ldata, store_ops);
	store_ops->close(store);
	return ret;

error3:
	if (rsc_table)
		metal_free_memory(rsc_table);
error2:
	loader_ops->close(store, ldata, store_ops);
error1:
	store_ops->close(store);
	rproc->ops->shutdown(rproc);
	metal_mutex_release(&rproc->lock);
	return ret;
}

unsigned int remoteproc_allocate_id(struct remoteproc *rproc,
				    unsigned int start,
				    unsigned int end)
{
	unsigned int notifyid;

	if (start == RSC_NOTIFY_ID_ANY)
		start = 0;
	if (end == RSC_NOTIFY_ID_ANY)
		end = METAL_BITS_PER_ULONG;
	notifyid = metal_bitmap_next_set_bit(&rproc->bitmap,
					     start, end);
	if (notifyid != end)
		metal_bitmap_set_bit(&rproc->bitmap, notifyid);
	return notifyid;
}

static int remoteproc_virtio_notify(void *priv, uint32_t id)
{
	struct remoteproc *rproc = priv;

	return rproc->ops->notify(rproc, id);
}

struct virtio_device *
remoteproc_create_virtio(struct remoteproc *rproc,
			 int vdev_id, unsigned int role,
			 void (*rst_cb)(struct virtio_device *vdev))
{
	void *rsc_table;
	struct fw_rsc_vdev *vdev_rsc;
	struct metal_io_region *vdev_rsc_io;
	struct virtio_device *vdev;
	struct remoteproc_virtio *rpvdev;
	size_t vdev_rsc_offset;
	unsigned int notifyid;
	unsigned int num_vrings, i;
	struct metal_list *node;

	metal_assert(rproc);
	metal_mutex_acquire(&rproc->lock);
	rsc_table = rproc->rsc_table;
	vdev_rsc_io = rproc->rsc_io;
	vdev_rsc_offset = find_rsc(rsc_table, RSC_VDEV, vdev_id);
	if (!vdev_rsc_offset) {
		metal_mutex_release(&rproc->lock);
		return NULL;
	}
	vdev_rsc = rsc_table + vdev_rsc_offset;
	notifyid = vdev_rsc->notifyid;
	/* Check if the virtio device is already created */
	metal_list_for_each(&rproc->vdevs, node) {
		rpvdev = metal_container_of(node, struct remoteproc_virtio,
					    node);
		if (rpvdev->vdev.index == notifyid)
			return &rpvdev->vdev;
	}
	vdev = rproc_virtio_create_vdev(role, notifyid,
					vdev_rsc, vdev_rsc_io, rproc,
					remoteproc_virtio_notify,
					rst_cb);
	num_vrings = vdev_rsc->num_of_vrings;
	/* set the notification id for vrings */
	for (i = 0; i < num_vrings; i++) {
		struct fw_rsc_vdev_vring *vring_rsc;
		metal_phys_addr_t da;
		unsigned int num_descs, align;
		struct metal_io_region *io;
		void *va;
		size_t size;
		int ret;

		vring_rsc = &vdev_rsc->vring[i];
		notifyid = vring_rsc->notifyid;
		da = vring_rsc->da;
		num_descs = vring_rsc->num;
		align = vring_rsc->align;
		size = vring_size(num_descs, align);
		va = remoteproc_mmap(rproc, NULL, &da, size, 0, &io);
		if (!va)
			goto err1;
		ret = rproc_virtio_init_vring(vdev, i, notifyid,
					      va, io, num_descs, align);
		if (ret)
			goto err1;
	}
	rpvdev = metal_container_of(vdev, struct remoteproc_virtio, vdev);
	metal_list_add_tail(&rproc->vdevs, &rpvdev->node);

	metal_mutex_release(&rproc->lock);
	return vdev;

err1:
	remoteproc_remove_virtio(rproc, vdev);
	metal_mutex_release(&rproc->lock);
	return NULL;
}

void remoteproc_remove_virtio(struct remoteproc *rproc,
			      struct virtio_device *vdev)
{
	struct remoteproc_virtio *rpvdev;

	(void)rproc;
	metal_assert(vdev);
	rpvdev = metal_container_of(vdev, struct remoteproc_virtio, vdev);
	metal_list_del(&rpvdev->node);
	rproc_virtio_remove_vdev(&rpvdev->vdev);
}

int remoteproc_get_notification(struct remoteproc *rproc, uint32_t notifyid)
{
	struct remoteproc_virtio *rpvdev;
	struct metal_list *node;
	int ret;

	metal_list_for_each(&rproc->vdevs, node) {
		rpvdev = metal_container_of(node, struct remoteproc_virtio,
					    node);
		ret = rproc_virtio_notified(&rpvdev->vdev, notifyid);
		if (ret)
			return ret;
	}
	return 0;
}
