/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <openamp/open_amp.h>
#include <openamp/virtio.h>
#include "rsc_table.h"
#include "platform_info.h"

#define SHUTDOWN_MSG	0xEF56A55A

//#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* Local variables */
static struct remoteproc *rproc;
static struct rpmsg_virtio_device *rpmsg_vdev;
static struct rsc_table_info rsc_info;

static void virtio_rst_cb(struct virtio_device *vdev)
{
	/* TODO: need to be implemented */
	(void)id;
}

/*-----------------------------------------------------------------------------*
 *  RPMSG callbacks setup by remoteproc_resource_init()
 *-----------------------------------------------------------------------------*/
static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			  void *priv, unsigned long src)
{
	(void)priv;
	(void)src;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		evt_chnl_deleted = 1;
		return;
	}

	/* Send data back to master */
	if (rpmsg_send(rp_chnl, data, len) < 0) {
		LPERROR("rpmsg_send failed\n");
	}
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
	(void)rp_chnl;
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{
	(void)rp_chnl;

	evt_chnl_deleted = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_virtio_device *rpmsg_vdev)
{
	int status = 0;


	/* disable interrupts and free resources */
	LPRINTF("De-initializating remoteproc resource\n");
	remoteproc_resource_deinit(proc);

	return 0;
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
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
	if (!rproc)
		return -1;
	rpmsg_vdev = platform_create_rpmsg_vdev(rproc,
						0, VIRTIO_DEV_HOST,
						virtio_rst_cb);
	if (!rpmsg_vdev) {
		remoteproc_remove(rproc);
		status = -1;
	}
	app(rpmsg_vdev);

	LPRINTF("Stopping application...\n");
	cleanup_system();

	return status;
}

