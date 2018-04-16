/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <metal/alloc.h>
#include <metal/log.h>
#include <openamp/elf_loader.h>
#include <openamp/remoteproc.h>

static int elf_is_64(const void *elf_info)
{
	const unsigned char *tmp = elf_info;

	if (tmp[EI_CLASS] == ELFCLASS64)
		return 1;
	else
		return 0;
}

static long elf_load_data(struct image_store_ops *ops, void *store,
			     size_t offset, void *dest,
			     size_t size, struct metal_io_region *io,
			     int block)
{
	long lsize;
	long total_lsize;

	total_lsize = (long)size;
	while (size > 0) {
		do {
			lsize = ops->load(store, offset, dest, size, io, block);
			if (lsize == -RPROC_EAGAIN && ops->load_finish)
				while(!ops->load_finish(store));
		} while (lsize == -RPROC_EAGAIN);
		if (lsize < 0)
			return lsize;
		else if (!lsize)
			break;
		size -= (size_t)lsize;
		offset += (metal_phys_addr_t)lsize;
		dest += lsize;
	}
	total_lsize -= size;
	return total_lsize;
}

static void elf64_parse_segment(void *elf_phdr, unsigned int *p_type,
				size_t *p_offset,
				metal_phys_addr_t *p_vaddr,
				metal_phys_addr_t *p_paddr,
				size_t *p_filesz, size_t *p_memsz)
{
	Elf64_Phdr *phdr = elf_phdr;

	*p_type = phdr->p_type;
	*p_offset = phdr->p_offset;
	*p_vaddr = (metal_phys_addr_t)phdr->p_vaddr;
	*p_paddr = (metal_phys_addr_t)phdr->p_paddr;
	*p_filesz = phdr->p_filesz;
	*p_memsz = phdr->p_memsz;
}

static void elf32_parse_segment(void *elf_phdr, unsigned int *p_type,
				size_t *p_offset,
				metal_phys_addr_t *p_vaddr,
				metal_phys_addr_t *p_paddr,
				size_t *p_filesz, size_t *p_memsz)
{
	Elf32_Phdr *phdr = elf_phdr;

	*p_type = phdr->p_type;
	*p_offset = phdr->p_offset;
	*p_vaddr = (metal_phys_addr_t)phdr->p_vaddr;
	*p_paddr = (metal_phys_addr_t)phdr->p_paddr;
	*p_filesz = phdr->p_filesz;
	*p_memsz = phdr->p_memsz;
}

static void *elf64_get_section(void *elf_info, const char *name)
{
	struct elf64_info *elf64_info = elf_info;
	Elf64_Ehdr *ehdr = &elf64_info->ehdr;
	Elf64_Shdr *shdr = elf64_info->shdrs;
	const char *name_table = elf64_info->shstrtab;
	unsigned int i;

	if (!shdr || !name_table)
		return NULL;
	for (i = 0; i < ehdr->e_shnum; i++, shdr++) {
		if (strcmp(name, name_table + shdr->sh_name))
			continue;
		return shdr;
	}
	return NULL;
}

static void *elf32_get_section(void *elf_info, const char *name)
{
	struct elf32_info *elf32_info = elf_info;
	Elf32_Ehdr *ehdr = &elf32_info->ehdr;
	Elf32_Shdr *shdr = elf32_info->shdrs;
	const char *name_table = elf32_info->shstrtab;
	unsigned int i;

	if (!shdr || !name_table)
		return NULL;
	for (i = 0; i < ehdr->e_shnum; i++, shdr++) {
		if (strcmp(name, name_table + shdr->sh_name))
			continue;
		return shdr;
	}
	return NULL;
}

static long elf_load_sections_headers(void *elf_info, void *store,
					 struct image_store_ops *ops)
{
	void *dest;
	size_t offset, size;
	long lsize;
	struct elf64_info *elf64_info = NULL;
	struct elf32_info *elf32_info = NULL;
	Elf64_Ehdr *elf64_ehdr;
	Elf32_Ehdr *elf32_ehdr;

	int is_elf64 = elf_is_64(elf_info);

	if (is_elf64) {
		elf64_info = elf_info;
		dest = elf64_info->shdrs;
		elf64_ehdr = &elf64_info->ehdr;
		offset = elf64_ehdr->e_shoff;
		size = elf64_ehdr->e_shentsize * elf64_ehdr->e_shnum;
	} else {
		elf32_info = elf_info;
		dest = elf32_info->shdrs;
		elf32_ehdr = &elf32_info->ehdr;
		dest = elf32_info->shdrs;
		offset = elf32_ehdr->e_shoff;
		size = elf32_ehdr->e_shentsize * elf32_ehdr->e_shnum;
	}
	lsize = elf_load_data(ops, store, offset, dest, size, NULL,
			      SYNC_LOAD);
	if (lsize != (long)size) {
		metal_log(METAL_LOG_ERROR, "Elf load shd: Failed to load section hds.\n");
		return -RPROC_EINVAL;
	}

	/* Load section header names */
	if (is_elf64) {
		Elf64_Shdr *shdr = &elf64_info->shdrs[elf64_ehdr->e_shstrndx];
		offset = shdr->sh_offset;
		size = shdr->sh_size;
		elf64_info->shstrtab = metal_allocate_memory(size);
		dest = elf64_info->shstrtab;
	} else {
		Elf32_Shdr *shdr = &elf32_info->shdrs[elf32_ehdr->e_shstrndx];
		offset = shdr->sh_offset;
		size = shdr->sh_size;
		elf32_info->shstrtab = metal_allocate_memory(size);
		dest = elf32_info->shstrtab;
	}
	if (!dest) {
		metal_log(METAL_LOG_ERROR, "Elf load shd: Failed to allocate memory.\n");
		return -RPROC_ENOMEM;
	}
	lsize = elf_load_data(ops, store, offset, dest, size, NULL,
			      SYNC_LOAD);
	if (lsize != (long)size) {
		metal_log(METAL_LOG_ERROR, "Elf Open: Failed to load section names.\n");
		metal_free_memory(dest);
		if (is_elf64)
			elf64_info->shstrtab = NULL;
		else
			elf32_info->shstrtab = NULL;
		return -RPROC_EINVAL;
	}

	return lsize;
}

int elf_identify(void *store_head)
{
	if (memcmp(store_head, ELFMAG, SELFMAG))
		return 0;
	else
		return 1;
}

void *elf_parse(void *store, struct image_store_ops *ops)
{
	unsigned char tmp[EI_NIDENT];
	size_t size;
	long lsize;
	size_t elf_info_size;
	void *elf_info = NULL, *dest;
	size_t offset, phdrs_size, shdrs_size;
	Elf64_Ehdr elf64_ehdr;
	Elf32_Ehdr elf32_ehdr;
	struct elf64_info *elf64_info;
	struct elf32_info *elf32_info;
	int is_elf64;

	if (!ops)
		return NULL;
	metal_log(METAL_LOG_DEBUG, "%s: open image file\n", __func__);
	if (ops->open(store)) {
		metal_log(METAL_LOG_ERROR, "ELF Open: failed to open firmware\n");
		return NULL;
	}
	metal_log(METAL_LOG_DEBUG, "%s: load image header EIDENT\n", __func__);
	lsize = ops->load(store, 0, tmp, sizeof(tmp), NULL, SYNC_LOAD);
	if (lsize <= 0) {
		metal_log(METAL_LOG_ERROR, "Elf Open: failed to load header\n");
		return NULL;
	}
	if (memcmp(tmp, ELFMAG, SELFMAG)) {
		metal_log(METAL_LOG_ERROR, "ELF Open: not an ELF image\n");
		return NULL;
	}
	is_elf64 = elf_is_64(tmp);
	if (is_elf64) {
		dest =  (void *)(&elf64_ehdr) + EI_NIDENT;
		size = sizeof(elf64_ehdr) - EI_NIDENT;
	} else {
		dest =  (void *)(&elf32_ehdr) + EI_NIDENT;
		size = sizeof(elf32_ehdr) + EI_NIDENT;
	}
	/* Load ELF header */
	metal_log(METAL_LOG_DEBUG, "%s: load the rest of image header\n", __func__);
	lsize = elf_load_data(ops, store, EI_NIDENT, dest, size, NULL,
			      SYNC_LOAD);
	if (lsize != (long)size) {
		metal_log(METAL_LOG_ERROR, "Elf Open: Failed to load ELF header.\n");
		goto error;
	}

	if (is_elf64) {
		phdrs_size = elf64_ehdr.e_phentsize * elf64_ehdr.e_phnum;
		shdrs_size = elf64_ehdr.e_shentsize * elf64_ehdr.e_shnum;
		elf_info_size = sizeof(*elf64_info);
	} else {
		phdrs_size = elf32_ehdr.e_phentsize * elf32_ehdr.e_phnum;
		shdrs_size = elf32_ehdr.e_shentsize * elf32_ehdr.e_shnum;
		elf_info_size = sizeof(*elf32_info);
	}
	size = elf_info_size + phdrs_size + shdrs_size;
	elf_info = metal_allocate_memory(size);
	if (!elf_info) {
		metal_log(METAL_LOG_ERROR, "Elf Open: Failed to alloc memory.\n");
		return NULL;
	}

	/* Store ELF headers */
	memcpy(elf_info, tmp, sizeof(tmp));
	if (is_elf64) {
		elf64_info = elf_info;
		elf64_info->phdrs = elf_info + sizeof(*elf64_info);
		elf64_info->shdrs = (void *)elf64_info->phdrs + phdrs_size;
		memcpy(elf64_info, &elf64_ehdr, sizeof(elf64_ehdr));
	} else {
		elf32_info = elf_info;
		elf32_info->phdrs = elf_info + sizeof(*elf32_info);
		elf32_info->shdrs = (void *)elf32_info->phdrs + phdrs_size;
		memcpy(elf32_info, &elf32_ehdr, sizeof(elf32_ehdr));
	}
	memcpy(elf_info, tmp, sizeof(tmp));

	/* Load ELF program headers */
	if (is_elf64) {
		dest = elf64_info->phdrs;
		offset = elf64_ehdr.e_phoff;
	} else {
		dest = elf32_info->phdrs;
		offset = elf32_ehdr.e_phoff;
	}
	metal_log(METAL_LOG_DEBUG, "%s: load program header\n", __func__);
	lsize = elf_load_data(ops, store, offset, dest, phdrs_size, NULL,
			      SYNC_LOAD);
	if (lsize < 0 || (size_t)lsize != phdrs_size) {
		metal_log(METAL_LOG_ERROR, "Elf Open: Failed to load program headers.\n");
		goto error;
	}

	/* Load ELF sections headers */
	if (shdrs_size && (ops->features & SUPPORT_SEEK)) {
		metal_log(METAL_LOG_DEBUG, "%s: load section header\n", __func__);
		lsize = elf_load_sections_headers(elf_info, store, ops);
		if (lsize <0 ) {
			metal_log(METAL_LOG_ERROR, "Elf Open: Failed to load section headers.\n");
			goto error;
		}
	}

	return elf_info;
error:
	if (elf_info)
		metal_free_memory(elf_info);
	return NULL;
}

int elf_load(void *store, void *elf_info, struct remoteproc *rproc,
	     struct image_store_ops *ops)
{
	struct elf32_info *elf32_info = NULL;
	struct elf64_info *elf64_info = NULL;
	void *elf_phdr;
	int lsize;
	unsigned int phnum, i;

	if (!elf_info && !ops)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		elf64_info = elf_info;
		phnum = elf64_info->ehdr.e_phnum;
		elf_phdr = elf64_info->phdrs;
	} else {
		elf32_info = elf_info;
		phnum = elf32_info->ehdr.e_phnum;
		elf_phdr = elf32_info->phdrs;
	}
	metal_log(METAL_LOG_DEBUG, "%s: load segements\n", __func__);
	/* Load segments */
	for (i = 0; i < phnum; i++) {
		unsigned int p_type;
		size_t p_offset, p_filesz, p_memsz;
		metal_phys_addr_t p_vaddr, p_paddr, da;
		void *dest;
		struct metal_io_region *io = NULL;
		size_t size;

		metal_log(METAL_LOG_DEBUG, "%s: parse segment %d\n", __func__, i);
		if (elf64_info) {
			elf64_parse_segment(elf_phdr, &p_type, &p_offset,
					    &p_vaddr, &p_paddr, &p_filesz,
					    &p_memsz);
			elf_phdr += sizeof(Elf64_Phdr);
		} else {
			elf32_parse_segment(elf_phdr, &p_type, &p_offset,
					    &p_vaddr, &p_paddr, &p_filesz,
					    &p_memsz);
			elf_phdr += sizeof(Elf32_Phdr);
		}
		metal_log(METAL_LOG_DEBUG, "%s: parse segment %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n",
			   __func__, i, p_type, p_offset, p_vaddr, p_paddr,
			   p_filesz, p_memsz);
		if (!(p_type && PT_LOAD))
			continue;
		da = p_paddr;
		dest = remoteproc_mmap(rproc, NULL, &da, p_memsz, 0, &io);
		if (dest == NULL) {
			metal_log(METAL_LOG_ERROR, "Elf Load: mmap da 0x%lx failed.\n", da);
			return -RPROC_EINVAL;
		}
		metal_log(METAL_LOG_DEBUG, "%s: load segment %d, offset 0x%x to %p\n",
			    __func__, i, p_offset, dest);
		lsize = elf_load_data(ops, store, p_offset, dest,
				      p_filesz, io, ASYNC_LOAD);
		if (lsize < 0 || (size_t)lsize != p_filesz) {
			metal_log(METAL_LOG_ERROR, "Elf Load: load failed.\n");
			return -RPROC_EINVAL;
		}
		if (p_memsz > p_filesz) {
			size = p_memsz - p_filesz;
			da += p_filesz;
			dest = remoteproc_mmap(rproc, NULL, &da, size, 0, &io);
			if (io) {
				size_t offset;

				offset = metal_io_virt_to_offset(io, dest);
				metal_io_block_set(io, offset, 0, size);
			} else {
				memset(dest, 0, size);
			}
		}
	}
	/* Check if all the segements have been loaded successfully */
	if (ops->load_finish)
		while(!ops->load_finish(store));

	/* Load sections headers if it is not loaded yet */
	if (!(ops->features & SUPPORT_SEEK)) {
		/* If seek operation is supported, the section headers
		 * have been loaded when opening the ELF file.
		 * Only load section headers if seek operation is not
		 * supported such that it cannot be loaded when opening
		 * the ELF.
		 */
		lsize = elf_load_sections_headers(elf_info, store, ops);
		if (lsize < 0) {
			metal_log(METAL_LOG_ERROR, "Elf Load: Failed to load section headers.\n");
			return -RPROC_EINVAL;
		}
	}

	/* TBD section relocate */
	return 0;
}

void elf_close(void *store, void *elf_info, struct image_store_ops *ops)
{
	if (elf_is_64(elf_info)) {
		struct elf64_info *elf64_info = elf_info;

		metal_free_memory(elf64_info->shstrtab);
		elf64_info->shstrtab = NULL;
	} else {
		struct elf32_info *elf32_info = elf_info;

		metal_free_memory(elf32_info->shstrtab);
		elf32_info->shstrtab = NULL;
	}
	ops->close(store);
}

metal_phys_addr_t elf_get_entry(void *elf_info)
{

	if (!elf_info)
		return -RPROC_EINVAL;

	if (elf_is_64(elf_info)) {
		Elf64_Ehdr *elf64_ehdr;
		elf64_ehdr = elf_info;
		return elf64_ehdr->e_entry;
	} else {
		Elf32_Ehdr *elf32_ehdr;

		elf32_ehdr = elf_info;
		return elf32_ehdr->e_entry;
	}
}

long elf_get_segment_file_len(void *elf_info, int index)
{
	struct elf64_info *elf64_info;
	struct elf32_info *elf32_info;
	Elf64_Ehdr *elf64_ehdr;
	Elf32_Ehdr *elf32_ehdr;

	if (!elf_info)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		elf64_info = elf_info;
		elf64_ehdr = elf_info;
		if (index >= (int)elf64_ehdr->e_phnum)
			return -RPROC_EINVAL;
		return (long)elf64_info->phdrs[index].p_filesz;
	} else {
		elf32_info = elf_info;
		elf32_ehdr = elf_info;
		if (index >= (int)elf32_ehdr->e_phnum)
			return -RPROC_EINVAL;
		return (long)elf32_info->phdrs[index].p_filesz;
	}
}

long elf_copy_segment(void *elf_info, struct image_store_ops *ops,
			 void *store, int index, void *dest)
{
	struct elf64_info *elf64_info;
	struct elf32_info *elf32_info;
	Elf64_Ehdr *elf64_ehdr;
	Elf32_Ehdr *elf32_ehdr;
	metal_phys_addr_t p_vaddr, p_paddr;
	unsigned int p_type;
	size_t p_offset, p_filesz, p_memsz;
	int lsize;

	if (!elf_info)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		Elf64_Phdr *elf_phdr;

		elf64_info = elf_info;
		elf64_ehdr = &elf64_info->ehdr;
		if (index >= (int)elf64_ehdr->e_phnum)
			return -RPROC_EINVAL;
		elf_phdr = &elf64_info->phdrs[index];
		elf64_parse_segment(elf_phdr, &p_type, &p_offset,
				    &p_vaddr, &p_paddr, &p_filesz,
				    &p_memsz);
	} else {
		Elf32_Phdr *elf_phdr;

		elf32_info = elf_info;
		elf32_ehdr = &elf32_info->ehdr;
		if (index >= (int)elf32_ehdr->e_phnum)
			return -RPROC_EINVAL;
		elf_phdr = &elf32_info->phdrs[index];
		elf32_parse_segment(elf_phdr, &p_type, &p_offset,
				    &p_vaddr, &p_paddr, &p_filesz,
				    &p_memsz);
	}
	lsize = elf_load_data(ops, store, p_offset, dest, p_filesz, NULL,
			      SYNC_LOAD);
	if (lsize > 0 && (size_t)lsize != p_filesz)
		return -RPROC_EINVAL;
	return lsize;
}

long elf_get_section_size(void *elf_info, const char *name)
{
	if (!elf_info)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		Elf64_Shdr *shdr = elf64_get_section(elf_info, name);
		if (shdr)
			return shdr->sh_size;
	} else {
		Elf32_Shdr *shdr = elf32_get_section(elf_info, name);
		if (shdr)
			return shdr->sh_size;
	}
	return 0;
}

long elf_get_section_da(void *elf_info, const char *name,
			   metal_phys_addr_t *da)
{
	if (!elf_info)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		Elf64_Shdr *shdr = elf64_get_section(elf_info, name);
		if (shdr) {
			*da = shdr->sh_addr;
			return (long)shdr->sh_size;
		}
	} else {
		Elf32_Shdr *shdr = elf32_get_section(elf_info, name);
		if (shdr) {
			*da = shdr->sh_addr;
			return (long)shdr->sh_size;
		}
	}
	return 0;
}

long elf_copy_section_from_file(void *elf_info, struct image_store_ops *ops,
				   void *store, const char *name, void *dest)
{
	size_t size = 0;
	size_t offset;
	long lsize;

	if (!elf_info)
		return -RPROC_EINVAL;
	if (elf_is_64(elf_info)) {
		Elf64_Shdr *shdr = elf64_get_section(elf_info, name);
		if (shdr) {
			size = shdr->sh_size;
			offset = shdr->sh_offset;
		}
	} else {
		Elf32_Shdr *shdr = elf32_get_section(elf_info, name);
		if (shdr) {
			size = shdr->sh_size;
			offset = shdr->sh_offset;
		}
	}
	if (!size)
		return 0;
	lsize = elf_load_data(ops, store, offset, dest, size, NULL,
			      SYNC_LOAD);
	if (lsize > 0 && (size_t)lsize != size)
		return -RPROC_EINVAL;
	else
		return lsize;

}

long elf_get_rsc_table(void *elf_info, metal_phys_addr_t *da_ptr)
{
	long rsc_len;

	rsc_len = elf_get_section_da(elf_info, ".resource_table", da_ptr);
	return rsc_len;
}

void *elf_copy_rsc_table(void *store, void *elf_info,
			 struct image_store_ops *ops, void *rsc_table)
{
	long rsc_len;

	rsc_len = elf_copy_section_from_file(elf_info, ops, store,
					     ".resource_table", rsc_table);
	if (rsc_len < 0)
		return NULL;
	else
		return rsc_table;
}

struct loader_ops elf_ops = {
	.parse = elf_parse,
	.get_rsc_table = elf_get_rsc_table,
	.copy_rsc_table = elf_copy_rsc_table,
	.load = elf_load,
	.close = elf_close,
	.get_entry = elf_get_entry,
};
