/*
 * Copyright (C) STMicroelectronics 2022 - All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/alloc.h>
#include <metal/cache.h>
#include <metal/sleep.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>
#include <openamp/virtqueue.h>

#include "rpmsg_internal.h"

/**
 * struct rpmsg_ept_msg - dynamic endpoint announcement message
 * @src: address of the endpoint that sned the message
 * @dest: address of the destination endpoint.
 * @flags: indicates the state of the endpoint based on @rpmsg_ept_flags enum.
 *
 * This message is sent across to inform the remote about the state of a local
 * endpoint associated with a remote endpoint:
 * - a RPMSG_EPT_OFF can be send to inform that a local endpoint is suspended.
 * - a RPMSG_EPT_ON can be send to inform that a local endpoint is ready to communicate.
 *
 * When we receive these messages, the appropriate endpoint is informed.
 */
METAL_PACKED_BEGIN
struct rpmsg_ept_msg {
	uint32_t src;
	uint32_t dst;
	uint32_t flags;
} METAL_PACKED_END;

/**
 * rpmsg_virtio_fc_callback
 *
 * This callback handles flow control from the remote device
 * and inform local endpoint.
 *
 * @param ept  - pointer to server channel control block.
 * @param data - pointer to received messages
 * @param len  - length of received data
 * @param priv - any private data
 * @param src  - source address
 *
 * @return - rpmag endpoint callback handled
 */
int rpmsg_fc_callback(struct rpmsg_endpoint *ept, void *data,
		      size_t len, uint32_t src, void *priv)
{
	struct rpmsg_device *rdev = ept->rdev;
	struct rpmsg_endpoint *_ept;
	struct rpmsg_ept_msg *fc_msg;

	(void)priv;
	(void)src;

	fc_msg = data;
	if (len != sizeof(*fc_msg))
		/* Returns as the message is corrupted */
		return RPMSG_ERR_PARAM;

	/* Check if a ept has been locally registered */
	metal_mutex_acquire(&rdev->lock);
	_ept = rpmsg_get_endpoint(rdev, NULL, fc_msg->dst, fc_msg->src);
	metal_mutex_release(&rdev->lock);

	if (!_ept)
		return RPMSG_ERR_PARAM;

	/*
	 * An NS announcement has been sent to the remote side which answer with flow control.
	 * Register the remote endpoint as default destination endpoint.
	 */
	if (_ept->dest_addr == RPMSG_ADDR_ANY)
		_ept->dest_addr = fc_msg->src;

	if (_ept->flow_ctrl_cb)
		_ept->flow_ctrl_cb(_ept, fc_msg->flags, fc_msg->src);

	return RPMSG_SUCCESS;
}

int rpmsg_set_flow_control(struct rpmsg_endpoint *ept, uint32_t flags)
{
	struct rpmsg_ept_msg  msg;

	msg.flags = flags;
	msg.src = ept->addr;
	msg.dst = ept->dest_addr;

	return  rpmsg_trysendto(ept, &msg, sizeof(msg), RPMSG_FC_EPT_ADDR);
}
