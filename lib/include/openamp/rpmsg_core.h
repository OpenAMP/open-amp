/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#ifndef _RPMSG_CORE_H_
#define _RPMSG_CORE_H_

#if defined __cplusplus
extern "C" {
#endif

#if (RPMSG_DEBUG == true)
#define RPMSG_ASSERT(_exp, _msg) do { \
		if (!(_exp)) { \
			openamp_print("FATAL: %s - "_msg, __func__); \
			while (1); \
		} \
	} while (0)
#else
#define RPMSG_ASSERT(_exp, _msg) if (!(_exp)) while (1)
#endif

int rpmsg_get_address(unsigned long *bitmap, int size);
int rpmsg_set_address(unsigned long *bitmap, int size, int addr);
int rpmsg_release_address(unsigned long *bitmap, int size, int addr);
int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr);

void *rpmsg_get_tx_buffer(struct rpmsg_virtio_device *rpmsgv,
			  unsigned long *len, unsigned short *idx);
void *rpmsg_get_rx_buffer(struct rpmsg_virtio_device *rpmsgv,
			  unsigned long *len, unsigned short *idx);
int rpmsg_wait_remote_ready(struct rpmsg_virtio_device *rpmsg_vdev);

struct rpmsg_channel *rpmsg_get_chnl_from_id(struct rpmsg_virtio_device *rpmsgv,
					     char *ch_name, unsigned int addr);
void rpmsg_tx_callback(struct virtqueue *vq);
void rpmsg_rx_callback(struct virtqueue *vq);
void rpmsg_ns_callback(struct rpmsg_endpoint *ept, void *data,
		       size_t len, uint32_t src, void *priv);

int rpmsg_send_ns_message(struct rpmsg_virtio_device *rpmsgv,
			  struct rpmsg_endpoint *ept, unsigned long flags);

int rpmsg_register_endpoint(struct rpmsg_virtio_device *rpmsgv,
			    struct rpmsg_endpoint *ept);

#if defined __cplusplus
}
#endif

#endif /* _RPMSG_CORE_H_ */
