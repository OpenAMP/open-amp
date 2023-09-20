/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIRTIO_NET_H
#define VIRTIO_NET_H

#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <metal/device.h>

#define VQIN_SIZE    4
#define RXDESC_COUNT 4
#define RXPOOL_SIZE  6

#define VQOUT_SIZE   4
#define TXDESC_COUNT 4

#define VIRTIO_NET_F_MAC (1 << 5)
#define VIRTIO_NET_F_MRG_RXBUF (1 << 15)

struct _net_eth_addr {
	uint8_t addr[6];
};

METAL_PACKED_BEGIN
struct _net_eth_hdr {
	struct _net_eth_addr dst;
	struct _net_eth_addr src;
	uint16_t type;
} METAL_PACKED_END;

#ifndef NET_ETH_MTU
#define NET_ETH_MTU 1500
#endif

#ifndef NET_ETH_MAX_FRAME_SIZE
#define NET_ETH_MAX_FRAME_SIZE	(NET_ETH_MTU + sizeof(struct _net_eth_hdr))
#endif
#ifndef NET_ETH_MAX_HDR_SIZE
#define NET_ETH_MAX_HDR_SIZE	(sizeof(struct _net_eth_hdr))
#endif

struct virtio_net_hdr {
	uint8_t flags;
	uint8_t gso_type;
	uint16_t hdr_len;
	uint16_t gso_size;
	uint16_t csum_start;
	uint16_t csum_offset;
	uint16_t num_buffers;
};

struct virtio_net_rx_pkt {
	struct virtio_net_hdr hdr;
	uint8_t pkt[NET_ETH_MAX_FRAME_SIZE];
};

struct virtio_net_tx_pkt {
	struct virtio_net_hdr hdr;
	uint8_t pkt[NET_ETH_MAX_FRAME_SIZE];
};

struct virtio_net_rx_desc {
	struct metal_list node;
	struct virtio_net_rx_pkt *pkt;
	uint8_t *data;
};

struct virtio_net_tx_desc {
	struct metal_list node;
	struct virtio_net_tx_pkt *pkt;
	uint8_t *data;
};

struct virtio_net_data {
	struct net_if *iface;
	/* Incoming packet processing hook */
	void (*net_pkt_in_cb)(uint8_t *data, int length, void *arg);
	void *net_pkt_in_cb_arg;
	uint8_t mac_addr[6];
	struct virtqueue *vqin, *vqout;
	int hdrsize;
	struct metal_list tx_free_list;
	struct metal_list rx_free_list;
	struct virtio_net_tx_pkt *txbuf;
	struct virtio_net_tx_desc *txdesc;
	struct virtio_net_rx_pkt *rxbuf;
	struct virtio_net_rx_desc *rxdesc;
};

void virtio_net_vqin_cb(void *arg);
void virtio_net_vqout_cb(void *arg);

typedef void (*net_funcptr)(void *);

/**
 * @brief Initialize a VIRTIO network device.
 *
 * @param vdev		Pointer to virtio_device structure.
 * @param vqs		Array of pointers to the virtqueues used by the device.
 * @param vq_names	Array of pointers to the virtqueues names.
 * @param cbs		Array of function pointers to call on virtqueue kick.
 * @param cb_args	Array of pointers to parameters for kick callbacks.
 * @param vq_count	Number of virtqueues the device uses.
 *
 * @return int 0 for success.
 */

int virtio_net_init(struct virtio_device *vdev, struct virtqueue **vqs, const char **vq_names,
		    net_funcptr *cbs, void **cb_args, int vq_count);

/**
 * @brief Send packet over VIRTIO network device
 *
 * @param vdev		Pointer to virtio_device structure.
 * @param data		Data buffer.
 * @param length	Data buffer length.
 *
 * @return int 0 for success.
 */

int virtio_net_send(const struct virtio_device *vdev, uint8_t *data, uint16_t length);

#endif /* VIRTIO_NET_H */

