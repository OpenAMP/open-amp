/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*This is a sample demonstration application that showcases usage of proxy
 *from the remote core.
 *This application is meant to run on the remote CPU running linux.
 *This application can print to the master console and perform file I/O through
 *rpmsg channels.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <openamp/open_amp.h>
#include <openamp/rpmsg_rpc_client_server.h>
#include "platform_info.h"
#include "rpmsg-rpc-demo.h"

#define REDEF_O_CREAT 100
#define REDEF_O_EXCL 200
#define REDEF_O_RDONLY 0
#define REDEF_O_WRONLY 1
#define REDEF_O_RDWR 2
#define REDEF_O_APPEND 2000
#define REDEF_O_ACCMODE 3

#define raw_printf(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...) raw_printf("Master> " format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static void *platform;
static struct rpmsg_device *rpdev;
static struct rpmsg_endpoint app_ept;

void linux_rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	rpmsg_destroy_ept(&app_ept);
	LPRINTF("Endpoint is destroyed\r\n");
	ept_deleted = 1;
}

void terminate_rpc_app(void)
{
	LPRINTF("Destroying endpoint.\r\n");
	if (!ept_deleted)
		rpmsg_destroy_ept(&app_ept);
}

void exit_action_handler(int signum)
{
	(void)signum;
	terminate_rpc_app();
}

void kill_action_handler(int signum)
{
	(void)signum;
	LPRINTF("RPC service killed !!\r\n");

	terminate_rpc_app();

	if (rpdev)
		platform_release_rpmsg_vdev(rpdev, platform);
	if (platform)
		platform_cleanup(platform);
}

/* Application entry point */
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret = 0;
	struct sigaction exit_action;
	struct sigaction kill_action;

	/* Initialize signalling infrastructure */
	memset(&exit_action, 0, sizeof(struct sigaction));
	memset(&kill_action, 0, sizeof(struct sigaction));
	exit_action.sa_handler = exit_action_handler;
	kill_action.sa_handler = kill_action_handler;
	sigaction(SIGTERM, &exit_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGKILL, &kill_action, NULL);
	sigaction(SIGHUP, &kill_action, NULL);

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\r\n");

	ret = rpmsg_create_ept(&app_ept, rdev, RPMSG_SERVICE_NAME,
			       RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
			       linux_rpmsg_endpoint_server_cb,
			       linux_rpmsg_service_unbind);
	if (ret) {
		LPERROR("Failed to create endpoint.\r\n");
		return -EINVAL;
	}

	LPRINTF("Successfully created rpmsg endpoint.\r\n");
	while (1) {
		platform_poll(priv);
		if (err_cnt) {
			LPERROR("Got error!\r\n");
			ret = -EINVAL;
			break;
		}
		/* we got a shutdown request, exit */
		if (ept_deleted || request_termination) {
			break;
		}
	}
	LPRINTF("\nRPC service exiting !!\r\n");

	terminate_rpc_app();
	return ret;
}

int main(int argc, char *argv[])
{
	int ret;

	LPRINTF("Starting application...\r\n");

	/* Initialize platform */
	ret = platform_init(argc, argv, &platform);
	if (ret) {
		LPERROR("Failed to initialize platform.\r\n");
		ret = -1;
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
						   VIRTIO_DEV_MASTER,
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

