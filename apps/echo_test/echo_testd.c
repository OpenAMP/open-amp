/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "rsc_table.h"
#include "platform_info.h"

#define SHUTDOWN_MSG	0xEF56A55A

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static struct rpmsg_endpoint lept;
static int ept_deleted = 0;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	(void)priv;
	(void)src;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		LPRINTF("shutdown message is received.\n");
		rpmsg_destroy_ept(ept);
		return RPMSG_EPT_CB_HANDLED;
	}

	/* Send data back to master */
	if (rpmsg_send(ept, data, len) < 0) {
		LPERROR("rpmsg_send failed\n");
	}
	return RPMSG_EPT_CB_HANDLED;
}

static void rpmsg_endpoint_destroy(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPRINTF("Endpoint is destroyed\n");
	ept_deleted = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret;

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\n");

	ret = rpmsg_create_ept(&lept, rdev, RPMSG_CHAN_NAME, 0, RPMSG_ADDR_ANY,
			       rpmsg_endpoint_cb, rpmsg_endpoint_destroy);
	if (ret) {
		LPERROR("Failed to create endpoint.\n");
		return -1;
	}

	LPRINTF("Successfully created rpmsg endpoint.\n");
	while(1) {
		platform_poll(priv);
		/* we got a shutdown request, exit */
		if (ept_deleted) {
			break;
		}
	}

	return 0;
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
	struct remoteproc *rproc;
	struct rpmsg_device *rdev;
	int ret;

	LPRINTF("Starting application...\n");

	/* Initialize HW system components */
	init_system();

	if (argc >= 2) {
		proc_id = strtoul(argv[1], NULL, 0);
	}

	if (argc >= 3) {
		rsc_id = strtoul(argv[2], NULL, 0);
	}

	rproc = platform_create_proc(proc_id, rsc_id);
	if (!rproc) {
		LPERROR("Failed to create remoteproc device.\n");
		ret = -1;
	} else {
		rdev = platform_create_rpmsg_vdev(rproc, 0,
						   VIRTIO_DEV_SLAVE,
						   NULL, NULL);
		if (!rdev) {
			LPERROR("Failed to create rpmsg virtio device.\n");
			ret = -1;
		} else {
			app(rdev, (void *)rproc);
			ret = 0;
		}
	}

	LPRINTF("Stopping application...\n");
	remoteproc_remove(rproc);
	cleanup_system();

	return ret;
}

