/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_core.h>
#include <metal/utilities.h>

struct rpmsg_endpoint *rpmsg_get_endpoint(struct rpmsg_device *rdev,
					  const char *name, uint32_t addr,
					  uint32_t dest_addr)
{
	struct metal_list *node;
	struct rpmsg_endpoint *ept;

	metal_list_for_each(&rdev->endpoints, node) {
		int name_match = 0;

		ept = metal_container_of(node, struct rpmsg_endpoint, node);
		if (addr != RPMSG_ADDR_ANY && ept->addr == addr)
			return ept;
		if (name)
			name_match = !strncmp(ept->name, name,
					      sizeof(ept->name));
		if (dest_addr == RPMSG_ADDR_ANY &&
		    ept->addr == RPMSG_ADDR_ANY &&
		    name_match)
			return ept;
		if (addr == RPMSG_ADDR_ANY &&
		    ept->dest_addr == RPMSG_ADDR_ANY &&
		    name_match)
			return ept;
		if (addr == ept->addr && dest_addr == ept->dest_addr &&
		    name_match)
			return ept;
	}
	return NULL;
}

struct rpmsg_endpoint *
rpmsg_get_ept_from_remote_addr(struct rpmsg_device *rdev,
			       uint32_t dest_addr)
{
	return rpmsg_get_endpoint(rdev, NULL, RPMSG_ADDR_ANY, dest_addr);
}

struct rpmsg_endpoint *
rpmsg_get_ept_from_addr(struct rpmsg_device *rdev, uint32_t addr)
{
	return rpmsg_get_endpoint(rdev, NULL, addr, RPMSG_ADDR_ANY);
}

void rpmsg_unregister_endpoint(struct rpmsg_endpoint *ept)
{
	struct rpmsg_device *rdev;

	if (!ept)
		return;

	rdev = ept->rdev;

	metal_mutex_acquire(&rdev->lock);
	if(ept->addr != RPMSG_ADDR_ANY)
		rpmsg_release_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
				      ept->addr);
	metal_list_del(&ept->node);
	metal_mutex_release(&rdev->lock);
}

int rpmsg_register_endpoint(struct rpmsg_device *rdev,
			    struct rpmsg_endpoint *ept)
{
	ept->rdev = rdev;

	metal_list_add_tail(&rdev->endpoints, &ept->node);
	return RPMSG_SUCCESS;
}

int rpmsg_send_ns_message(struct rpmsg_endpoint *ept, unsigned long flags)
{
	struct rpmsg_ns_msg ns_msg;
	int ret;

	ns_msg.flags = flags;
	ns_msg.addr = ept->addr;
	strncpy(ns_msg.name, ept->name, sizeof(ns_msg.name));
	ret = rpmsg_send_offchannel_raw(ept, RPMSG_NS_EPT_ADDR,
					RPMSG_NS_EPT_ADDR,
					&ns_msg, sizeof(ns_msg), 1);
	if (ret < 0)
		return ret;
	else
		return RPMSG_SUCCESS;
}

/**
 * rpmsg_get_address
 *
 * This function provides unique 32 bit address.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 *
 * return - a unique address
 */
int rpmsg_get_address(unsigned long *bitmap, int size)
{
	int addr = -1;
	int i, tmp32;

	/* Find first available buffer */
	for (i = 0; i < size; i++) {
		tmp32 = get_first_zero_bit(bitmap[i]);

		if (tmp32 < 32) {
			addr = tmp32 + (i * 32);
			bitmap[i] |= (1 << tmp32);
			break;
		}
	}

	return addr;
}

/**
 * rpmsg_release_address
 *
 * Frees the given address.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - none
 */
int rpmsg_release_address(unsigned long *bitmap, int size, int addr)
{
	unsigned int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the addr as available */
	i = addr / 32;
	j = addr % 32;

	mask = mask << j;
	bitmap[i] = bitmap[i] & (~mask);

	return RPMSG_SUCCESS;
}

/**
 * rpmsg_is_address_set
 *
 * Checks whether address is used or free.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - TRUE/FALSE
 */
int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr)
{
	int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the id as available */
	i = addr / 32;
	j = addr % 32;
	mask = mask << j;

	return (bitmap[i] & mask);
}

/**
 * rpmsg_set_address
 *
 * Marks the address as consumed.
 *
 * @param bitmap - bit map for addresses
 * @param size   - size of bitmap
 * @param addr   - address to free
 *
 * return - none
 */
int rpmsg_set_address(unsigned long *bitmap, int size, int addr)
{
	int i, j;
	unsigned long mask = 1;

	if (addr >= size * 32)
		return -1;

	/* Mark the id as available */
	i = addr / 32;
	j = addr % 32;
	mask = mask << j;
	bitmap[i] |= mask;

	return RPMSG_SUCCESS;
}

