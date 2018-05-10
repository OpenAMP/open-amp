/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/rpmsg.h>
#include <openamp/rpmsg_core.h>
#include <metal/alloc.h>
#include <metal/sleep.h>

struct rpmsg_endpoint *rpmsg_create_ept(struct rpmsg_device *rdev,
					const char *name, uint32_t src,
					uint32_t dest, rpmsg_ept_cb cb,
					rpmsg_ept_destroy_cb destroy_cb)
{
	struct rpmsg_endpoint *ept;
	int status;

	metal_mutex_acquire(&rdev->lock);
	if (src != RPMSG_ADDR_ANY) {
		if (!rpmsg_is_address_set
		    (rdev->bitmap, RPMSG_ADDR_BMP_SIZE, src))
			/* Mark the address as used in the address bitmap. */
			rpmsg_set_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE,
					  src);
		else
			goto ret_err;
	} else {
		src = rpmsg_get_address(rdev->bitmap, RPMSG_ADDR_BMP_SIZE);
	}

	ept = rpmsg_get_endpoint(rdev, name, src, dest);
	if (!ept)
		ept = metal_allocate_memory(sizeof(*ept));
	if (!ept)
		goto ret_err;
	ept->addr = src;
	ept->dest_addr = dest;
	ept->cb = cb;
	ept->destroy_cb = destroy_cb;
	strcpy(ept->name, name);

	status = rpmsg_register_endpoint(rdev, ept);
	if (status < 0)
		goto reg_err;

	metal_mutex_release(&rdev->lock);

	if (ept->dest_addr == RPMSG_ADDR_ANY) {
		/* Send NS announcement to remote processor */
		status = rpmsg_send_ns_message(ept, RPMSG_NS_CREATE);
		if(status)
			goto ns_err;
	}

	return ept;

ns_err:
	rpmsg_unregister_endpoint(ept);
reg_err:
	metal_free_memory(ept);

ret_err:
	metal_mutex_release(&rdev->lock);
	return NULL;
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
	metal_free_memory(ept);
}

int rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data, int size,
			      int wait)
{
	struct rpmsg_device *rdev;

	if (!ept)
		return -1;

	rdev = ept->rdev;
	if (rdev->ops->send_offchannel_raw)
		return rdev->ops->send_offchannel_raw(rdev, src, dst, data,
						      size, wait);
	else
		return -1;
}
