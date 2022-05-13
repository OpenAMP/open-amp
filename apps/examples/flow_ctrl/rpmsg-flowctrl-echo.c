/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, STMicroelectronics. All rights reserved.
 */

/*
 * This is a sample demonstration application that showcases usage of rpmsg
 * This application is meant to run on the remote CPU running baremetal code.
 * This application echoes back data that was sent to it by the master core.
 */

#include <metal/alloc.h>
#include <metal/sleep.h>
#include <openamp/open_amp.h>
#include <stdio.h>

#include "platform_info.h"
#include "rpmsg-flowctrl.h"

#ifndef WITH_RPMSG_FLOW_CONTROL
#error "WITH_RPMSG_FLOW_CONTROL must be defined to build this example"
#endif

#define SHUTDOWN_MSG	0xEF56A55A

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static struct rpmsg_endpoint lept;
static int shutdown_req;

struct rpmsg_rcv_msg {
	void *data;
	size_t len;
	struct rpmsg_endpoint *ept;
	struct rpmsg_rcv_msg *next;
};

struct rpmsg_rcv_msg *rpmsg_list;
static int rnum;

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
	LPRINTF("unexpected Remote endpoint destroy\r\n");
	shutdown_req = 1;
}

static void rpmsg_name_service_bind_cb(struct rpmsg_device *rdev,
				       const char *name, uint32_t dest)
{
	LPRINTF("new endpoint notification is received.\r\n");
	if (strcmp(name, RPMSG_SERVICE_NAME)) {
		LPERROR("Unexpected name service %s.\r\n", name);
		return;
	}

	(void)rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
			       RPMSG_ADDR_ANY, dest,
			       rpmsg_endpoint_cb,
			       rpmsg_service_unbind);

	rpmsg_set_flow_control(&lept, RPMSG_EPT_FC_ON);
}

/*-----------------------------------------------------------------------------
 *  Application
 *-----------------------------------------------------------------------------
 */
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret;
	struct rpmsg_rcv_msg *rpmsg_node;

	(void)rdev;

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\r\n");

	rpmsg_list = NULL;
	rnum = 0;

	while (!is_rpmsg_ept_ready(&lept))
		platform_poll(priv);

	while (1) {
		platform_poll(priv);
		/* we got a shutdown request, exit */
		if (shutdown_req) {
			break;
		}

		while (rpmsg_list) {
			/* Make a pause very 10 messages */
			if (!(rnum % 10)) {
				LPRINTF("Pause the communication during 1 second\r\n");
				rpmsg_set_flow_control(rpmsg_list->ept, 0);
				metal_sleep_usec(1000000);
				LPRINTF("resume the communication\r\n");
				rpmsg_set_flow_control(rpmsg_list->ept, RPMSG_EPT_FC_ON);
			}
			/* Send data back to master */
			ret = rpmsg_send(rpmsg_list->ept, rpmsg_list->data,
					 rpmsg_list->len);
			if (ret < 0) {
				LPERROR("rpmsg_send failed\r\n");
				return ret;
			}
			rpmsg_release_rx_buffer(rpmsg_list->ept,
						rpmsg_list->data);
			rpmsg_node = rpmsg_list;
			rpmsg_list = rpmsg_list->next;
			metal_free_memory(rpmsg_node);
			rnum++;
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
						   NULL, rpmsg_name_service_bind_cb);
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
