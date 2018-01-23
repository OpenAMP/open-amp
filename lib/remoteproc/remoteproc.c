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
#include <openamp/rsc_table_parser.h>
#include <openamp/sh_mem.h>

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

static void *remoteproc_datova_from_mems(struct remoteproc *rproc,
					 metal_phys_addr_t da, size_t size,
					 struct metal_io_region **io)
{
	struct metal_list *node;

	metal_list_for_each(&rproc->mems, node) {
		metal_phys_addr_t da_start, da_end, pa;
		struct remoteproc_mem *mem;
		struct metal_io_region *tmpio;

		mem = metal_container_of(node, struct remoteproc_mem, node);
		tmpio = mem->io;
		da_start = mem->da;
		da_end = da_start + mem->size;
		if (da >= da_start && (da + size) <= da_end) {
			pa = mem->pa + (da - da_start);
			*io = tmpio;
			return metal_io_phys_to_virt(mem->io, pa);
		}
	}
	return NULL;
}

static void *remoteproc_patova_from_mems(struct remoteproc *rproc,
					 metal_phys_addr_t pa, size_t size,
					 struct metal_io_region **io)
{
	struct metal_list *node;

	metal_list_for_each(&rproc->mems, node) {
		metal_phys_addr_t pa_start, pa_end;
		struct remoteproc_mem *mem;
		struct metal_io_region *tmpio;

		mem = metal_container_of(node, struct remoteproc_mem, node);
		tmpio = mem->io;
		pa_start = mem->pa;
		pa_end = pa_start + mem->size;
		if (pa >= pa_start && (pa + size) <= pa_end) {
			*io = tmpio;
			return metal_io_phys_to_virt(mem->io, pa);
		}
	}
	return NULL;
}

/**
 * remoteproc_remove_mems
 *
 * Remove memory resources from remote processor mems list.
 * This function expect the caller have mutex protection.
 *
 * @rproc - pointer to the remote processor instance
 *
 */
static void remoteproc_remove_mems(struct remoteproc *rproc)
{
	struct metal_list *node;

	metal_list_for_each(&rproc->mems, node) {
		struct remoteproc_mem *mem;
		struct metal_list *tmpnode;

		mem = metal_container_of(node, struct remoteproc_mem, node);
		tmpnode = node->next;
		metal_list_del(node);
		metal_free_memory(mem);
		node = tmpnode;
	}
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

	ret = handle_rsc_table(rproc, rsc_table, rsc_len);
	if (ret < 0) {
		rsc_table = RPROC_ERR_PTR(ret);
		goto error;
	}
	return rsc_table;

error:
	metal_free_memory(rsc_table);
	return rsc_table;
}

struct remoteproc *remoteproc_init(struct remoteproc_ops *ops, void *priv)
{
	struct remoteproc *rproc;

	rproc = ops->init(ops, priv);
	if (rproc) {
		rproc->state = RPROC_OFFLINE;
		metal_mutex_init(&rproc->lock);
	}
	return rproc;
}

void remoteproc_remove(struct remoteproc *rproc)
{
	if (rproc) {
		metal_mutex_acquire(&rproc->lock);
		if (rproc->state != RPROC_OFFLINE) {
			metal_mutex_release(&rproc->lock);
			(void)remoteproc_shutdown(rproc);
			metal_mutex_acquire(&rproc->lock);
			rproc->state = RPROC_OFFLINE;
		}
		metal_mutex_release(&rproc->lock);

		/* After this call, the rproc pointer is suposed to be
		 * already freed.
		 */
		rproc->ops->remove(rproc);
	}
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
				ret = rproc->ops->stop(rproc);
				rproc->state = RPROC_STOPPED;
			}
			if (!ret) {
				ret = rproc->ops->shutdown(rproc);
				rproc->state = RPROC_OFFLINE;
				remoteproc_remove_mems(rproc);
			}
		}
		metal_mutex_release(&rproc->lock);
	}
	return ret;
}

void *remoteproc_datova(struct remoteproc *rproc,
			metal_phys_addr_t da, size_t size,
			struct metal_io_region **io)
{
	void *va;

	va = remoteproc_datova_from_mems(rproc, da, size, io);
	if (va)
		return va;
	if (rproc->ops->mmap) {
		metal_phys_addr_t pa = METAL_BAD_PHYS;
		va = rproc->ops->mmap(rproc, &pa, &da, size,
				      0, io);
		return va;
	}
	return NULL;
}

void *remoteproc_mmap(struct remoteproc *rproc,
		      metal_phys_addr_t *pa, metal_phys_addr_t *da,
		      size_t size, unsigned int attribute,
		      struct metal_io_region **io)
{
	void *va = NULL;
	metal_phys_addr_t lpa, lda;

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
	if (lpa != METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		va = remoteproc_patova_from_mems(rproc, lpa, size, io);
	else if (lda != METAL_BAD_PHYS && lpa == METAL_BAD_PHYS)
		va = remoteproc_datova_from_mems(rproc, lda, size, io);

	if (va == NULL && rproc->ops->mmap) {
		va = rproc->ops->mmap(rproc, &lpa, &lda, size, attribute, io);
		if (pa)
			*pa  = lpa;
		if (da)
			*da = lda;

	}
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
		metal_log(METAL_LOG_ERROR, "load store failure: invalid rproc state %d.\n",
			      rproc->state);
		return -RPROC_EINVAL;
	}

	if (!store_ops) {
		metal_log(METAL_LOG_ERROR, "load store failure: loader ops is not set.\n");
		return -RPROC_EINVAL;
	}

	/* Open firmware to get ready to parse */
	metal_log(METAL_LOG_DEBUG, "%s: open firmware image\n\r", __func__);
	ret = store_ops->open(store);
	if (ret) {
		metal_log(METAL_LOG_ERROR, "load store failure: failed to open firmware.\n");
		return ret;
	}

	/* Check firmware format to select a parser */
	loader_ops = rproc->loader_ops;
	if (!loader_ops) {
		metal_log(METAL_LOG_DEBUG, "%s: check loader \n\r", __func__);
		loader_ops = remoteproc_check_fw_format(store, store_ops);
		if (!loader_ops) {
			metal_log(METAL_LOG_ERROR, "load store failure: failed to get store ops.\n");
			ret = -RPROC_EINVAL;
			goto error1;
		}
		rproc->loader_ops = loader_ops;
	}

	/* parse the firmware, get the headers */
	metal_log(METAL_LOG_DEBUG, "%s: parse firmware\n\r", __func__);
	ldata = loader_ops->parse(store, store_ops);
	if (!ldata) {
		metal_log(METAL_LOG_ERROR, "load store failure: failed to parse firmware.\n");
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
		metal_log(METAL_LOG_ERROR, "load store failure: failed to load firmware.\n");
		goto error3;
	}

	/* Update resource table */
	if (rsc_len && rsc_da != METAL_BAD_PHYS) {
		void *rsc_table_cp = rsc_table;
		struct metal_io_region *io;

		metal_log(METAL_LOG_DEBUG, "%s: update resource table\n\r", __func__);
		rsc_table = remoteproc_mmap(rproc, NULL, &rsc_da,
					    rsc_len, 0, &io);
		if (!rsc_table) {
			/* FIX me: Should use metal_io_block_write */
			memcpy(rsc_table, rsc_table_cp, rsc_len);
			rproc->rsc_table = rsc_table;
			rproc->rsc_len = rsc_len;
		} else {
			metal_log(METAL_LOG_WARNING, "load store: not able to update rsc table.\n");
		}
		metal_free_memory(rsc_table_cp);
		/* So that the rsc_table will not get released */
		rsc_table = NULL;
	}

	metal_log(METAL_LOG_DEBUG, "%s: successfully load firmware\n\r", __func__);
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
