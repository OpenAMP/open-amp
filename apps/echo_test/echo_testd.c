/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <openamp/open_amp.h>
#include "rsc_table.h"
#include "platform_info.h"

#define SHUTDOWN_MSG	0xEF56A55A

#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static struct rpmsg_endpoint *ept = NULL;
static int ept_deleted = 0;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/*-----------------------------------------------------------------------------*
 *  RPMSG callbacks setup by remoteproc_resource_init()
 *-----------------------------------------------------------------------------*/
static void rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			      uint32_t src, void *priv)
{
	(void)priv;
	(void)src;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		ept_deleted = 1;
		return;
	}

	/* Send data back to master */
	if (rpmsg_send(ept, data, len) < 0) {
		LPERROR("rpmsg_send failed\n");
	}
}

static void rpmsg_endpoint_destroy(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPERROR("Endpoint is destroyed\n");
	ept_deleted = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_virtio_device *rvdev, void *priv)
{
	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\n");

	ept = rpmsg_create_ept(rvdev, RPMSG_CHAN_NAME, 0, RPMSG_ADDR_ANY,
			       rpmsg_endpoint_cb, rpmsg_endpoint_destroy);
	if (!ept) {
		LPERROR("Failed to create endpoint.\n");
		return -1;
	}

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
	struct rpmsg_virtio_device *rvdev;
	int status = -1;

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
	} else {
		rvdev = platform_create_rpmsg_vdev(rproc, 0,
						   VIRTIO_DEV_SLAVE,
						   NULL);
		if (!rvdev) {
			LPERROR("Failed to create rpmsg virtio device.\n");
		} else {
			app(rvdev, (void *)rproc);
		}
	}

	LPRINTF("Stopping application...\n");
	remoteproc_remove(rproc);
	cleanup_system();

	return status;
}

