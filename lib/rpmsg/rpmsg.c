/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>
#include <metal/alloc.h>
#include <metal/utilities.h>

#include "rpmsg_internal.h"

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
static int rpmsg_get_address(unsigned long *bitmap, int size)
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
static int rpmsg_release_address(unsigned long *bitmap, int size, int addr)
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
static int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr)
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
static int rpmsg_set_address(unsigned long *bitmap, int size, int addr)
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

/**
 * This function sends rpmsg "message" to remote device.
 *
 * @param ept     - pointer to end point
 * @param src     - source address of channel
 * @param dst     - destination address of channel
 * @param data    - data to transmit
 * @param size    - size of data
 * @param wait    - boolean, wait or not for buffer to become
 *                  available
 *
 * @return - size of data sent or negative value for failure.
 *
 */
int rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data, int size,
			      int wait)
{
	struct rpmsg_device *rdev;

	if (!ept || !data || (dst == RPMSG_ADDR_ANY))
		return RPMSG_ERR_PARAM;

	/* Get the associated rpmsg device for endpoint. */
	rdev = ept->rdev;
	if (rdev->ops.send_offchannel_raw)
		return rdev->ops.send_offchannel_raw(rdev, src, dst, data,
						      size, wait);

	return RPMSG_ERR_PARAM;
}

int rpmsg_send_ns_message(struct rpmsg_endpoint *ept, unsigned long flags)
{
	struct rpmsg_ns_msg ns_msg;
	int ret;

	ns_msg.flags = flags;
	ns_msg.addr = ept->addr;
	strncpy(ns_msg.name, ept->name, sizeof(ns_msg.name));
	ret = rpmsg_send_offchannel_raw(ept, ept->addr,
					RPMSG_NS_EPT_ADDR,
					&ns_msg, sizeof(ns_msg), true);
	if (ret < 0)
		return ret;
	else
		return RPMSG_SUCCESS;
}

struct rpmsg_endpoint *rpmsg_get_endpoint(struct rpmsg_device *rdev,
					  const char *name, uint32_t addr,
					  uint32_t dest_addr)
{
	struct metal_list *node;
	struct rpmsg_endpoint *ept;

	metal_list_for_each(&rdev->endpoints, node) {
		int name_match = 1;

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

static void rpmsg_unregister_endpoint(struct rpmsg_endpoint *ept)
{
	struct rpmsg_device *rdev;

	if (!ept)
		return;

	rdev = ept->rdev;

	if(ept->addr != RPMSG_ADDR_ANY)
		rpmsg_release_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
				      ept->addr);
	metal_list_del(&ept->node);
}

int rpmsg_register_endpoint(struct rpmsg_device *rdev,
			    struct rpmsg_endpoint *ept)
{
	ept->rdev = rdev;

	metal_list_add_tail(&rdev->endpoints, &ept->node);
	return RPMSG_SUCCESS;
}

int rpmsg_create_ept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev,
		     const char *name, uint32_t src, uint32_t dest,
		     rpmsg_ept_cb cb, rpmsg_ept_destroy_cb destroy_cb)
{
	int status = 0;
	uint32_t addr = src;

	if (!ept)
		return -EINVAL;

	metal_mutex_acquire(&rdev->lock);
	if (src != RPMSG_ADDR_ANY) {
		if (!rpmsg_is_address_set
		    (rdev->bitmap, RPMSG_ADDR_BMP_SIZE, src))
			/* Mark the address as used in the address bitmap. */
			rpmsg_set_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
					  src);
		else
			goto ret_status;
	} else {
		addr = rpmsg_get_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE);
	}

	rpmsg_init_ept(ept, name, addr, dest, cb, destroy_cb);

	status = rpmsg_register_endpoint(rdev, ept);
	if (status < 0)
		rpmsg_release_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE, addr);


	if (!status  && ept->dest_addr == RPMSG_ADDR_ANY) {
		/* Send NS announcement to remote processor */
		metal_mutex_release(&rdev->lock);
		status = rpmsg_send_ns_message(ept, RPMSG_NS_CREATE);
		metal_mutex_acquire(&rdev->lock);
		if(status)
			rpmsg_unregister_endpoint(ept);
	}

ret_status:
	metal_mutex_release(&rdev->lock);
	return status;
}

/**
 * rpmsg_destroy_ept
 *
 * This function deletes rpmsg endpoint and performs cleanup.
 *
 * @param ept - pointer to endpoint to destroy
 *
 */
void rpmsg_destroy_ept(struct rpmsg_endpoint *ept)
{
	if (!ept)
		return;

	rpmsg_unregister_endpoint(ept);
	if (ept->destroy_cb)
		ept->destroy_cb(ept);
}
