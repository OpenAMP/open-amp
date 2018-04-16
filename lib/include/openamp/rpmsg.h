/*
 * Remote processor messaging
 *
 * Copyright (C) 2011 Texas Instruments, Inc.
 * Copyright (C) 2011 Google, Inc.
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RPMSG_H_
#define _RPMSG_H_

#include <openamp/compiler.h>
#include <openamp/rpmsg_virtio.h>
#include <openamp/sh_mem.h>
#include <metal/mutex.h>
#include <metal/list.h>

#if defined __cplusplus
extern "C" {
#endif

/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS 0 /* RP supports name service notifications */

/* Configurable parameters */
#ifndef RPMSG_BUFFER_SIZE
#define RPMSG_BUFFER_SIZE	(512)
#endif
#define RPMSG_NAME_SIZE		(32)

#define RPMSG_NS_EPT_ADDR	(0x35)
#define RPMSG_ADDR_ANY		0xFFFFFFFF

/* RPMSG channel states. */
#define RPMSG_CHNL_STATE_IDLE	0
#define RPMSG_CHNL_STATE_NS	1
#define RPMSG_CHNL_STATE_ACTIVE	2

/* Definitions for device types , null pointer, etc.*/
#define RPMSG_SUCCESS	0
#define RPMSG_NULL	(void *)0
#define RPMSG_REMOTE	0
#define RPMSG_MASTER	1

#define RPMSG_TRUE	1
#define RPMSG_FALSE	0

/* Error macros. */
#define RPMSG_ERROR_BASE	-2000
#define RPMSG_ERR_NO_MEM	(RPMSG_ERROR_BASE - 1)
#define RPMSG_ERR_NO_BUFF	(RPMSG_ERROR_BASE - 2)
#define RPMSG_ERR_MAX_VQ	(RPMSG_ERROR_BASE - 3)
#define RPMSG_ERR_PARAM		(RPMSG_ERROR_BASE - 4)
#define RPMSG_ERR_DEV_STATE	(RPMSG_ERROR_BASE - 5)
#define RPMSG_ERR_BUFF_SIZE	(RPMSG_ERROR_BASE - 6)
#define RPMSG_ERR_INIT	(RPMSG_ERROR_BASE - 7)
#define RPMSG_ERR_ADDR	(RPMSG_ERROR_BASE - 8)

#define RPMSG_BUF_HELD (1U << 31) /* Flag to suggest to hold the buffer */

#define RPMSG_LOCATE_DATA(p) ((unsigned char *)p + sizeof(struct rpmsg_hdr))

/* remoteproc_vshm_pool is a place holder til we figure out how to
 * replace it */
/**
 * struct remoteproc_vshm - remoteproc vring structure
 * @va logical address of the start of the shared memory
 * @pa physical address of the start of the shared memory
 * @size size of the shared memory pool
 * @io metal I/O region of the shared memory, can be NULL
 */
struct remoteproc_vshm_pool {
	void *va;
	metal_phys_addr_t pa;
	int size;
	struct metal_io_region *io;
};

/**
 * enum rpmsg_ns_flags - dynamic name service announcement flags
 *
 * @RPMSG_NS_CREATE: a new remote service was just created
 * @RPMSG_NS_DESTROY: a known remote service was just destroyed
 * @RPMSG_NS_CREATE_WITH_ACK: a new remote service was just created waiting
 *                            acknowledgment.
 */
enum rpmsg_ns_flags {
	RPMSG_NS_CREATE = 0,
	RPMSG_NS_DESTROY = 1,
};

/**
 * struct rpmsg_hdr - common header for all rpmsg messages
 * @src: source address
 * @dst: destination address
 * @reserved: reserved for future use
 * @len: length of payload (in bytes)
 * @flags: message flags
 *
 * Every message sent(/received) on the rpmsg bus begins with this header.
 */
OPENAMP_PACKED_BEGIN
struct rpmsg_hdr {
	uint32_t src;
	uint32_t dst;
	uint32_t reserved;
	uint16_t len;
	uint16_t flags;
} OPENAMP_PACKED_END;

/**
 * struct rpmsg_ns_msg - dynamic name service announcement message
 * @name: name of remote service that is published
 * @addr: address of remote service that is published
 * @flags: indicates whether service is created or destroyed
 *
 * This message is sent across to publish a new service, or announce
 * about its removal. When we receive these messages, an appropriate
 * rpmsg channel (i.e device) is created/destroyed. In turn, the ->probe()
 * or ->remove() handler of the appropriate rpmsg driver will be invoked
 * (if/as-soon-as one is registered).
 */
OPENAMP_PACKED_BEGIN
struct rpmsg_ns_msg {
	char name[RPMSG_NAME_SIZE];
	uint32_t addr;
	uint32_t flags;
} OPENAMP_PACKED_END;

struct rpmsg_endpoint;

/**
 * struct rpmsg_endpoint - binds a local rpmsg address to its user
 * @name:name of the service supported
 * @rvdev: pointer to the rpmsg virtio device
 * @addr: local address of the endpoint
 * @dest_addr: address of the default remote endpoint binded.
 * @cb: user rx callback
 * @destroy_cb: user end point detsroy callback
 * @node: end point node.
 * @addr: local rpmsg address
 * @priv: private data for the driver's use
 *
 * In essence, an rpmsg endpoint represents a listener on the rpmsg bus, as
 * it binds an rpmsg address with an rx callback handler.
 */
struct rpmsg_endpoint {
	char name[RPMSG_NAME_SIZE];
	struct rpmsg_virtio_device *rvdev;
	uint32_t addr;
	uint32_t dest_addr;
	void (*cb)(struct rpmsg_endpoint *ept, void *data, size_t len,
		   uint32_t src, void *priv);
	void (*destroy_cb)(struct rpmsg_endpoint *ept);
	struct metal_list node;
	void *priv;
};

int rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data, int size,
			      int wait);

/**
 * rpmsg_send() - send a message across to the remote processor
 * @ept: the rpmsg endpoint
 * @data: payload of the message
 * @len: length of the payload
 *
 * This function sends @data of length @len based on the @ept.
 * The message will be sent to the remote processor which the channel belongs
 * to, using @ept's source and destination addresses.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_send(struct rpmsg_endpoint *ept, const void *data, int len)
{
	if (ept->dest_addr == RPMSG_ADDR_ANY)
		return RPMSG_ERR_ADDR;
	return rpmsg_send_offchannel_raw(ept, ept->addr, ept->dest_addr, data,
					 len, RPMSG_TRUE);
}

/**
 * rpmsg_sendto() - send a message across to the remote processor, specify dst
 * @ept: the rpmsg endpoint
 * @data: payload of message
 * @len: length of payload
 * @dst: destination address
 *
 * This function sends @data of length @len to the remote @dst address.
 * The message will be sent to the remote processor which the @ept
 * channel belongs to, using @ept's source address.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_sendto(struct rpmsg_endpoint *ept, const void *data,
			int len, uint32_t dst)
{
	return rpmsg_send_offchannel_raw(ept, ept->addr, dst, data, len,
					 RPMSG_TRUE);
}

/**
 * rpmsg_send_offchannel() - send a message using explicit src/dst addresses
 * @ept: the rpmsg endpoint
 * @src: source address
 * @dst: destination address
 * @data: payload of message
 * @len: length of payload
 *
 * This function sends @data of length @len to the remote @dst address,
 * and uses @src as the source address.
 * The message will be sent to the remote processor which the @ept
 * channel belongs to.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_send_offchannel(struct rpmsg_endpoint *ept,
					uint32_t src, uint32_t dst,
					const void *data, int len)
{
	return rpmsg_send_offchannel_raw(ept, src, dst, data, len, RPMSG_TRUE);
}

/**
 * rpmsg_trysend() - send a message across to the remote processor
 * @ept: the rpmsg endpoint
 * @data: payload of message
 * @len: length of payload
 *
 * This function sends @data of length @len on the @ept channel.
 * The message will be sent to the remote processor which the @ept
 * channel belongs to, using @ept's source and destination addresses.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysend(struct rpmsg_endpoint *ept, const void *data,
				int len)
{
	if (ept->dest_addr == RPMSG_ADDR_ANY)
		return RPMSG_ERR_ADDR;
	return rpmsg_send_offchannel_raw(ept, ept->addr, ept->dest_addr, data,
					 len, RPMSG_FALSE);
}

/**
 * rpmsg_trysendto() - send a message across to the remote processor,
 * specify dst
 * @ept: the rpmsg endpoint
 * @data: payload of message
 * @len: length of payload
 * @dst: destination address
 *
 * This function sends @data of length @len to the remote @dst address.
 * The message will be sent to the remote processor which the @ept
 * channel belongs to, using @ept's source address.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysendto(struct rpmsg_endpoint *ept, const void *data,
				  int len, uint32_t dst)
{
	return rpmsg_send_offchannel_raw(ept, ept->addr, dst, data, len,
					 RPMSG_FALSE);
}

/**
 * rpmsg_trysend_offchannel() - send a message using explicit src/dst addresses
 * @ept: the rpmsg endpoint
 * @src: source address
 * @dst: destination address
 * @data: payload of message
 * @len: length of payload
 *
 * This function sends @data of length @len to the remote @dst address,
 * and uses @src as the source address.
 * The message will be sent to the remote processor which the @ept
 * channel belongs to.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * Can only be called from process context (for now).
 *
 * Returns number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysend_offchannel(struct rpmsg_endpoint *ept,
					   uint32_t src, uint32_t dst,
					   const void *data, int len)
{
	return rpmsg_send_offchannel_raw(ept, src, dst, data, len,
					 RPMSG_FALSE);
}

void (*rpmsg_endpoint_destroy_callback)(struct rpmsg_endpoint *ept, void *priv);

/**
 * struct rpmsg_endpoint - binds a local rpmsg address to its user
 * @rpmsgv: rpmsg virtio device
 * @name: service name associated to the endpoint
 * @ept: eendpoint created
 *
 * In essence, an rpmsg endpoint represents a listener on the rpmsg bus, as
 * it binds an rpmsg address with an rx callback handler.
 *
 * Rpmsg client should create an endpoint to discuss with remote. rpmsg client
 * provide at least a channel name, a callback for message notification and by
 * default endpoint source address should be set to RPMSG_ADDR_ANY.
 *
 * As an option Some rpmsg clients can specify an endpoint with a specific
 * source address.
 */
int rpmsg_create_ept(struct rpmsg_virtio_device *rpmsgv,
		     struct rpmsg_endpoint *ept);

void rpmsg_destroy_ept(struct rpmsg_endpoint *ept);

int rpmsg_init_vdev(struct rpmsg_virtio_device *rpmsg_vdev,
		    struct virtio_device *vdev, void *shm, int len);

void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev);

#if defined __cplusplus
}
#endif

#endif				/* _RPMSG_H_ */
