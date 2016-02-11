/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RPMSG_CORE_H_
#define _RPMSG_CORE_H_

#include "openamp/compiler.h"
#include "openamp/env.h"
#include "openamp/virtio.h"
#include "openamp/hil.h"
#include "openamp/sh_mem.h"
#include "openamp/llist.h"
#include "openamp/rpmsg.h"

/* Configurable parameters */
#define RPMSG_BUFFER_SIZE                       512
#define RPMSG_MAX_VQ_PER_RDEV                   2
#define RPMSG_NS_EPT_ADDR                       0x35
#define RPMSG_ADDR_BMP_SIZE                     4

/* Definitions for device types , null pointer, etc.*/
#define RPMSG_SUCCESS                           0
#define RPMSG_NULL                              (void *)0
#define RPMSG_REMOTE                            0
#define RPMSG_MASTER                            1
#define RPMSG_TRUE                              1
#define RPMSG_FALSE                             0

/* RPMSG channel states. */
#define RPMSG_CHNL_STATE_IDLE                   0
#define RPMSG_CHNL_STATE_NS                     1
#define RPMSG_CHNL_STATE_ACTIVE                 2

/* Remote processor/device states. */
#define RPMSG_DEV_STATE_IDLE                    0
#define RPMSG_DEV_STATE_ACTIVE                  1

/* Total tick count for 15secs - 1msec tick. */
#define RPMSG_TICK_COUNT                        15000

/* Time to wait - In multiple of 10 msecs. */
#define RPMSG_TICKS_PER_INTERVAL                10

/* Error macros. */
#define RPMSG_ERRORS_BASE                       -3000
#define RPMSG_ERR_NO_MEM                        (RPMSG_ERRORS_BASE - 1)
#define RPMSG_ERR_NO_BUFF                       (RPMSG_ERRORS_BASE - 2)
#define RPMSG_ERR_MAX_VQ                        (RPMSG_ERRORS_BASE - 3)
#define RPMSG_ERR_PARAM                         (RPMSG_ERRORS_BASE - 4)
#define RPMSG_ERR_DEV_STATE                     (RPMSG_ERRORS_BASE - 5)
#define RPMSG_ERR_BUFF_SIZE                     (RPMSG_ERRORS_BASE - 6)
#define RPMSG_ERR_DEV_ID                        (RPMSG_ERRORS_BASE - 7)
#define RPMSG_ERR_DEV_ADDR                      (RPMSG_ERRORS_BASE - 8)

struct rpmsg_channel;
typedef void (*rpmsg_rx_cb_t) (struct rpmsg_channel *, void *, int, void *,
			       unsigned long);
typedef void (*rpmsg_chnl_cb_t) (struct rpmsg_channel * rp_chl);
/**
 * remote_device
 *
 * This structure is maintained by RPMSG driver to represent remote device/core.
 *
 * @virtd_dev           - virtio device for remote core
 * @rvq                 - Rx virtqueue for virtio device
 * @tvq                 - Tx virtqueue for virtio device
 * @proc                - reference to remote processor
 * @rp_channels         - rpmsg channels list for the device
 * @rp_endpoints        - rpmsg endpoints list for the device
 * @mem_pool            - shared memory pool
 * @bitmap              - bitmap for channels addresses
 * @channel_created     - create channel callback
 * @channel_destroyed   - delete channel callback
 * @default_cb          - default callback handler for RX data on channel
 * @lock                - remote device mutex
 * @role                - role of the remote device, RPMSG_MASTER/RPMSG_REMOTE
 * @state               - remote device state, IDLE/ACTIVE
 * @support_ns          - if device supports name service announcement
 *
 */
struct remote_device {
	struct virtio_device virt_dev;
	struct virtqueue *rvq;
	struct virtqueue *tvq;
	struct hil_proc *proc;
	struct llist *rp_channels;
	struct llist *rp_endpoints;
	struct sh_mem_pool *mem_pool;
	unsigned long bitmap[RPMSG_ADDR_BMP_SIZE];
	rpmsg_chnl_cb_t channel_created;
	rpmsg_chnl_cb_t channel_destroyed;
	rpmsg_rx_cb_t default_cb;
	LOCK *lock;
	unsigned int role;
	unsigned int state;
	int support_ns;
};

/* Core functions */
int rpmsg_start_ipc(struct remote_device *rdev);
struct rpmsg_channel *_rpmsg_create_channel(struct remote_device *rdev,
					    char *name, unsigned long src,
					    unsigned long dst);
void _rpmsg_delete_channel(struct rpmsg_channel *rp_chnl);
struct rpmsg_endpoint *_create_endpoint(struct remote_device *rdev,
					rpmsg_rx_cb_t cb, void *priv,
					unsigned long addr);
void _destroy_endpoint(struct remote_device *rdev,
		       struct rpmsg_endpoint *rp_ept);
void rpmsg_send_ns_message(struct remote_device *rdev,
			   struct rpmsg_channel *rp_chnl, unsigned long flags);
int rpmsg_enqueue_buffer(struct remote_device *rdev, void *buffer,
			 unsigned long len, unsigned short idx);
void rpmsg_return_buffer(struct remote_device *rdev, void *buffer,
			 unsigned long len, unsigned short idx);
void *rpmsg_get_tx_buffer(struct remote_device *rdev, unsigned long *len,
			  unsigned short *idx);
void rpmsg_free_buffer(struct remote_device *rdev, void *buffer);
void rpmsg_free_channel(struct rpmsg_channel *rp_chnl);
void *rpmsg_get_rx_buffer(struct remote_device *rdev, unsigned long *len,
			  unsigned short *idx);
int rpmsg_get_address(unsigned long *bitmap, int size);
int rpmsg_release_address(unsigned long *bitmap, int size, int addr);
int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr);
int rpmsg_set_address(unsigned long *bitmap, int size, int addr);
void rpmsg_ns_callback(struct rpmsg_channel *server_chnl,
		       void *data, int len, void *priv, unsigned long src);

/* Remote device functions */
int rpmsg_rdev_init(struct remote_device **rdev, int dev_id, int role,
		    rpmsg_chnl_cb_t channel_created,
		    rpmsg_chnl_cb_t channel_destroyed,
		    rpmsg_rx_cb_t default_cb);
void rpmsg_rdev_deinit(struct remote_device *rdev);
struct llist *rpmsg_rdev_get_chnl_node_from_id(struct remote_device *rdev,
					       char *rp_chnl_id);
struct llist *rpmsg_rdev_get_chnl_from_addr(struct remote_device *rdev,
					    unsigned long addr);
struct llist *rpmsg_rdev_get_endpoint_from_addr(struct remote_device *rdev,
						unsigned long addr);
int rpmsg_rdev_notify(struct remote_device *rdev);
int rpmsg_rdev_create_virtqueues(struct virtio_device *dev, int flags, int nvqs,
				 const char *names[], vq_callback * callbacks[],
				 struct virtqueue *vqs[]);
unsigned char rpmsg_rdev_get_status(struct virtio_device *dev);

void rpmsg_rdev_set_status(struct virtio_device *dev, unsigned char status);

uint32_t rpmsg_rdev_get_feature(struct virtio_device *dev);

void rpmsg_rdev_set_feature(struct virtio_device *dev, uint32_t feature);

uint32_t rpmsg_rdev_negotiate_feature(struct virtio_device *dev,
				      uint32_t features);
/*
 * Read/write a variable amount from the device specific (ie, network)
 * configuration region. This region is encoded in the same endian as
 * the guest.
 */
void rpmsg_rdev_read_config(struct virtio_device *dev, uint32_t offset,
			    void *dst, int length);
void rpmsg_rdev_write_config(struct virtio_device *dev, uint32_t offset,
			     void *src, int length);
void rpmsg_rdev_reset(struct virtio_device *dev);

#endif				/* _RPMSG_CORE_H_ */
