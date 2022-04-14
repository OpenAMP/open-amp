/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, STMicroelectronics
 */

/*
 * This is a test application echoes back data that was sent to using "zero copy" mode.
 * It also tests the remote  part of the buffer recycler mechanism implemented
 * in the rpmsg virtio layer.
 */

#include <stdio.h>
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "platform_info.h"

#define SHUTDOWN_MSG		0xEF56A55A
#define RPMSG_SERVICE_NAME	"rpmsg-openamp-demo-channel"

#define LPRINTF(format, ...) printf("Echo thread: " format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

#define MAX_NB_TX_BUFF  10

static struct rpmsg_endpoint lept;
static int shutdown_req;

struct rpmsg_rcv_msg {
	void *data;
	size_t len;
	struct rpmsg_endpoint *ept;
	struct rpmsg_rcv_msg *next;
};

struct rpmsg_rcv_msg *rpmsg_list;

/*-----------------------------------------------------------------------------
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------
 */
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	(void)src;
	(void)priv;
	struct rpmsg_rcv_msg *rpmsg_node, *p_msg;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		LPRINTF("shutdown message is received.\r\n");
		shutdown_req = 1;
		return RPMSG_SUCCESS;
	}

	rpmsg_node = metal_allocate_memory(sizeof(*rpmsg_node));
	if (!rpmsg_node) {
		LPERROR("rpmsg_node allocation failed\r\n");
		return -1;
	}
	rpmsg_hold_rx_buffer(ept, data);
	rpmsg_node->ept = ept;
	rpmsg_node->data = data;
	rpmsg_node->len = len;
	rpmsg_node->next = NULL;

	if (!rpmsg_list)
		rpmsg_list = rpmsg_node;
	else {
		p_msg = rpmsg_list;
		while (p_msg->next)
			p_msg = p_msg->next;
		p_msg->next = rpmsg_node;
	}

	return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPRINTF(" Remote endpoint destroy request\r\n");
	shutdown_req = 1;
}

/*-----------------------------------------------------------------------------
 *  Application
 *-----------------------------------------------------------------------------
 */
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret, i;
	uint32_t max_size;
	struct rpmsg_rcv_msg *rpmsg_node;
	void *buff_list[MAX_NB_TX_BUFF];
	void *tx_msg;

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\r\n");

	rpmsg_list = NULL;
	ret = rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
			       RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
			       rpmsg_endpoint_cb,
			       rpmsg_service_unbind);
	if (ret) {
		LPERROR("Failed to create endpoint.\r\n");
		return -1;
	}

	LPRINTF("Successfully created rpmsg endpoint.\r\n");
	LPRINTF("RPMSG endpoint is binded with remote.\r\n");

	LPRINTF(" 1 - Get some TX buffers\r\n");
	for (i = 0; i < MAX_NB_TX_BUFF; i++)
		buff_list[i] = rpmsg_get_tx_payload_buffer(&lept, &max_size, 1);

	LPRINTF(" 2 - Release the unused Tx buffer\r\n");

	/*
	 * Test the reclaimer:
	 * Notice that, for test, the buff_list[MAX_NB_TX_BUFF - 1] is not released.
	 * Keeping a lost buffer allows to test the correct behavior of the library.
	 */
	for (i = 0; i < MAX_NB_TX_BUFF - 1; i++) {
		ret = rpmsg_release_tx_buffer(&lept, buff_list[i]);
		if (ret != RPMSG_SUCCESS) {
			if (ret) {
				LPERROR("Failed to release TX buffer.\r\n");
				return -1;
			}
		}
	}

	LPRINTF(" 3 - Send data to remote core, retrieve the echo\r\n");
	while (1) {
		platform_poll(priv);
		/* we got a shutdown request, exit */
		if (shutdown_req) {
			break;
		}
		while (rpmsg_list) {
			/* Send data back to master */
			ret = rpmsg_send(rpmsg_list->ept, rpmsg_list->data,
					 rpmsg_list->len);
			if (ret < 0) {
				LPERROR("rpmsg_send failed\r\n");
				return ret;
			}
			rpmsg_release_rx_buffer(rpmsg_list->ept,
						rpmsg_list->data);
			/* Get and discard an unused TX buffer every 13 sent */
			if (!(i++ % 13)) {
				tx_msg = rpmsg_get_tx_payload_buffer(rpmsg_list->ept,
								     &max_size, 1);
				if (!tx_msg) {
					LPERROR("Failed to get payload...\r\n");
					break;
				}
				if (tx_msg == buff_list[MAX_NB_TX_BUFF]) {
					LPERROR("error: got the lost buffer\r\n");
					break;
				}
				ret = rpmsg_release_tx_buffer(rpmsg_list->ept, tx_msg);
				if (ret < 0) {
					LPERROR("failed to release TX buffer...\r\n");
					break;
				}
			}
			rpmsg_node = rpmsg_list;
			rpmsg_list = rpmsg_list->next;
			metal_free_memory(rpmsg_node);
			/* Get and discard an unused TX buffer every 11 sent */
		}
	}
	rpmsg_destroy_ept(&lept);

	return 0;
}

/*-----------------------------------------------------------------------------
 *  Application entry point
 *-----------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	void *platform;
	struct rpmsg_device *rpdev;
	int ret;

	LPRINTF("Starting application...\r\n");

	/* Initialize platform */
	ret = platform_init(argc, argv, &platform);
	if (ret) {
		LPERROR("Failed to initialize platform.\r\n");
		ret = -1;
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
						   VIRTIO_DEV_DEVICE,
						   NULL, NULL);
		if (!rpdev) {
			LPERROR("Failed to create rpmsg virtio device.\r\n");
			ret = -1;
		} else {
			app(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev, platform);
			ret = 0;
		}
	}

	LPRINTF("Stopping application...\r\n");
	platform_cleanup(platform);

	return ret;
}
