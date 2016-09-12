/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include "rsc_table.h"
#include <stdio.h>

#define SHUTDOWN_MSG	0xEF56A55A

//#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)


/* Internal functions */
static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
static void rpmsg_read_cb(struct rpmsg_channel *, void *, int, void *,
			  unsigned long);

/* Globals */
static struct rpmsg_endpoint *rp_ept;
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
static int evt_chnl_deleted = 0;

/* External functions */
extern void init_system(void);
extern void cleanup_system(void);
extern struct hil_proc *platform_create_proc(int proc_index);
extern void *get_resource_table (int rsc_id, int *len);

/* Application entry point */
int app(struct hil_proc *hproc)
{
	int status = 0;

	//rsc_info.rsc_tab = (struct resource_table *)&resources;
	//rsc_info.size = sizeof(resources);

	/* Initialize RPMSG framework */
	LPRINTF("Try to init remoteproc resource\n");
	status =
	    remoteproc_resource_init(&rsc_info, hproc,
				     rpmsg_channel_created,
				     rpmsg_channel_deleted, rpmsg_read_cb,
				     &proc, 0);
	LPRINTF("init remoteproc resource done\n");

	if (RPROC_SUCCESS != status) {
		return -1;
	}

	do {
		hil_poll(proc->proc, 0);
	} while (!evt_chnl_deleted);

	/* disable interrupts and free resources */
	remoteproc_resource_deinit(proc);

	cleanup_system();

	return 0;
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
	rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
				  RPMSG_ADDR_ANY);
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{
	(void)rp_chnl;

	rpmsg_destroy_ept(rp_ept);
	rp_ept = NULL;
}

static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			  void *priv, unsigned long src)
{
	(void)priv;
	(void)src;

	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		evt_chnl_deleted = 1;
		return;
	}

	/* Send data back to master */
	rpmsg_send(rp_chnl, data, len);
}

int main(int argc, char *argv[])
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
	struct hil_proc *hproc;

	/* Initialize HW system components */
	init_system();

	if (argc >= 2) {
		proc_id = strtoul(argv[1], NULL, 0);
	}

	if (argc >= 3) {
		rsc_id = strtoul(argv[2], NULL, 0);
	}

	hproc = platform_create_proc(proc_id);
	if (!hproc) {
		LPRINTF("Failed to create proc platform data.\n");
		return -1;
	}
	rsc_info.rsc_tab = get_resource_table(
		(int)rsc_id, &rsc_info.size);
	if (!rsc_info.rsc_tab) {
		LPRINTF("Failed to get resource table data.\n");
		return -1;
	}

	return app(hproc);
}

