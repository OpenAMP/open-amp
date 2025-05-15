/*
 * Copyright (c) 2022-2025 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <openamp/open_amp.h>
#include <openamp/virtqueue.h>
#include <openamp/virtio.h>
#include <openamp/virtio_net_drv.h>
#include <metal/errno.h>
#include <metal/irq.h>

#define LOG_MODULE_NAME "virtio_net"

static int virtio_net_rx_refill(struct virtio_net_data *pdata)
{
	struct metal_list *node = metal_list_first(&pdata->rx_free_list);
	struct virtio_net_rx_desc *desc = NULL;
	struct virtqueue_buf vb[1] = {0};
	int status = 0;

	while (!virtqueue_full(pdata->vqin)) {
		node = metal_list_first(&pdata->rx_free_list);
		if (!node) {
			VIRTIO_ASSERT(0, "should have one descriptor per VQ buffer");
			status = -ENOMEM;
			break;
		}

		desc = metal_container_of(node, struct virtio_net_rx_desc, node);
		memset(&desc->pkt->hdr, 0, sizeof(desc->pkt->hdr));

		vb[0].buf = desc->pkt;
		vb[0].len = pdata->hdrsize + NET_ETH_MTU;

		status = virtqueue_add_buffer(pdata->vqin, vb, 0, 1, (void *)desc);
		if (status != 0) {
			VIRTIO_ASSERT(0, "Should have one descriptor per VQ buffer.");
			metal_list_add_tail(&pdata->rx_free_list, &desc->node);
			break;
		}
	}

	return status;
}

int virtio_net_init(struct virtio_device *vdev, struct virtqueue **vqs, const char **vq_names,
		    void (**cbs)(void *), void **cb_args, int vq_count)
{
	uint32_t devid, features = 0;
	uint8_t status = 0;
	struct virtio_net_data *dev_data = NULL;
	int ret;
	int i;

	if (!vdev || !vdev->priv || !vqs || !vq_names) {
		return -EINVAL;
	}

	dev_data = vdev->priv;

	vdev->vrings_info = metal_allocate_memory(sizeof(struct virtio_vring_info) * vq_count);
	if (!vdev->vrings_info) {
		return -ENOMEM;
	}

	for (i = 0; i < vq_count; i++) {
		vdev->vrings_info[i].vq = vqs[i];
	}

	devid = virtio_get_devid(vdev);
	if (devid != VIRTIO_ID_NETWORK) {
		metal_log(METAL_LOG_ERROR, "%s: Expected devid %04x, got %04x\n",
			  LOG_MODULE_NAME, VIRTIO_ID_NETWORK, devid);
		metal_free_memory(vdev->vrings_info);
		return -ENODEV;
	}

	virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER);
	virtio_set_features(vdev, VIRTIO_NET_F_MAC/*VIRTIO_F_NOTIFY_ON_EMPTY*/);

	if (vdev->id.version > 1) {
		virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_FEATURES_OK);
	}

	(void)virtio_get_features(vdev, &features);
	metal_log(METAL_LOG_DEBUG, "features: %08x\n", features);

	(void)virtio_get_status(vdev, &status);
	metal_log(METAL_LOG_DEBUG, "status: %08x\n", status);

	if (vdev->id.version > 1 && (!(status & VIRTIO_CONFIG_STATUS_FEATURES_OK))) {
		metal_log(METAL_LOG_ERROR,
			  "Expected VIRTIO_CONFIG_STATUS_FEATURES_OK to be set, got 0\n");
		return -ENODEV;
	}

	ret = virtio_create_virtqueues(vdev, 0, vq_count, vq_names, (vq_callback *)cbs, cb_args);
	if (ret) {
		metal_free_memory(vdev->vrings_info);
		return ret;
	}

	virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);

	dev_data->vqin = vdev->vrings_info[0].vq;
	dev_data->vqout = vdev->vrings_info[1].vq;
	dev_data->hdrsize = sizeof(struct virtio_net_hdr);
	if (!(features & VIRTIO_NET_F_MRG_RXBUF)) {
		dev_data->hdrsize -= 2;
	}

	metal_list_init(&dev_data->rx_free_list);

	for (i = 0; i < RXDESC_COUNT; i++) {
		metal_log(METAL_LOG_DEBUG, "%s: rx %d at %p\n", "virtio_net", i,
			  &dev_data->rxdesc[i]);

		dev_data->rxdesc[i].pkt = &dev_data->rxbuf[i];
		if (features & VIRTIO_NET_F_MRG_RXBUF) {
			dev_data->rxdesc[i].data = dev_data->rxbuf[i].pkt;
		} else {
			dev_data->rxdesc[i].data = (uint8_t *)&dev_data->rxbuf[i].hdr.num_buffers;
		}
		metal_list_add_tail(&dev_data->rx_free_list, &dev_data->rxdesc[i].node);
	}

	ret = virtio_net_rx_refill(dev_data);
	if (ret != 0) {
		return ret;
	}

	metal_list_init(&dev_data->tx_free_list);

	for (i = 0; i < TXDESC_COUNT; i++) {
		metal_log(METAL_LOG_DEBUG, "%s: tx %d at %p\n", "virtio_net", i,
			  &dev_data->txdesc[i]);

		dev_data->txdesc[i].pkt = &dev_data->txbuf[i];
		if (features & VIRTIO_NET_F_MRG_RXBUF) {
			dev_data->txdesc[i].data = dev_data->txbuf[i].pkt;
		} else {
			dev_data->txdesc[i].data = (uint8_t *)&dev_data->txbuf[i].hdr.num_buffers;
		}
		metal_list_add_tail(&dev_data->tx_free_list, &dev_data->txdesc[i].node);
	}

	virtqueue_kick(dev_data->vqin);
	virtqueue_kick(dev_data->vqout);

	if (VIRTIO_NET_F_MAC & features) {
		virtio_read_config(vdev, 0, dev_data->mac_addr, 6);
	} else {
		VIRTIO_ASSERT(0, "should generate a MAC address");
	}

	return 0;
}

void virtio_net_vqin_cb(void *arg)
{
	struct virtio_device *vdev = arg;
	struct virtio_net_rx_desc *desc;
	void (*net_pkt_in_cb)(uint8_t *data, int length, void *arg);
	struct virtio_net_data *dev_data = vdev->priv;
	uint32_t length;

	net_pkt_in_cb = dev_data->net_pkt_in_cb;

	while ((desc = virtqueue_get_buffer(dev_data->vqin, &length, NULL))) {
		length -= dev_data->hdrsize;
		if (net_pkt_in_cb) {
			net_pkt_in_cb(desc->data, length, dev_data->net_pkt_in_cb_arg);
		}
		metal_list_add_tail(&dev_data->rx_free_list, &desc->node);
	}
	(void)virtio_net_rx_refill(dev_data);
}

void virtio_net_vqout_cb(void *arg)
{
	struct virtio_net_data *pdata = arg;
	struct virtio_net_tx_desc *desc;

	while ((desc = virtqueue_get_buffer(pdata->vqout, NULL, NULL))) {
		metal_list_add_tail(&pdata->tx_free_list, &desc->node);
	}
}

int virtio_net_send(const struct virtio_device *vdev, uint8_t *data, uint16_t length)
{
	struct metal_list *node;
	struct virtio_net_tx_desc *desc;
	uint16_t total_len;
	int key;
	int ret = -EIO;
	struct virtio_net_data *dev_data = vdev->priv;
	struct virtqueue_buf vb[1] = {0};

	total_len = length;
	if (total_len > NET_ETH_MAX_FRAME_SIZE || total_len == 0) {
		return -EINVAL;
	}

	key = metal_irq_save_disable();
	node = metal_list_first(&dev_data->tx_free_list);
	metal_irq_restore_enable(key);
	if (!node) {
		return ret;
	}

	desc = metal_container_of(node, struct virtio_net_tx_desc, node);

	memset(&desc->pkt->hdr, 0, sizeof(desc->pkt->hdr));
	memcpy(desc->data, data, length);

	vb[0].buf = desc->pkt;
	vb[0].len = total_len + dev_data->hdrsize;

	if (virtqueue_add_buffer(dev_data->vqout, vb, 1, 0, (void *)desc)) {
		goto recycle;
	}

	virtqueue_kick(dev_data->vqout);

	return 0;

recycle:
	key = metal_irq_save_disable();
	metal_list_add_tail(&dev_data->tx_free_list, &desc->node);
	metal_irq_restore_enable(key);

	return ret;
}
