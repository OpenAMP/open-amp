/* This is a test application that runs baremetal code on the remote core 
 and responds to commands from master core to test the usage of rpmsg APIs. */

/* Including required headers */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openamp/open_amp.h>
#include "rsc_table.h"
#include "test_suite.h"

#define EPT_ADDR        59

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

/* Application provided callbacks */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			   void *pric, unsigned long src);
void rpmsg_read_ept_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
		       void *pric, unsigned long src);

static void sleep();

/* Globals */
static volatile int intr_flag = 0;
static struct rpmsg_endpoint *rp_ept;
static struct rpmsg_channel *app_rp_chnl;
static unsigned int Src;
static unsigned int Len;
static char firmware_name[] = "baremetal-fn-test-suite-remote-firmware";
static char r_buffer[512];
static struct rsc_table_info rsc_info;

/* External functions */
extern int init_system();
extern void cleanup_system();
extern struct hil_proc *platform_create_proc(int proc_index);
extern void *get_resource_table (int rsc_id, int *len);

int app (struct hil_proc *hproc)
{
	struct remote_proc *proc;
	int uninit = 0;
	struct ept_cmd_data *ept_data;

	/* This API creates the virtio devices for this remote node and initializes
	   other relevant resources defined in the resource table */
	remoteproc_resource_init(&rsc_info, hproc,
				 rpmsg_channel_created,
				 rpmsg_channel_deleted, rpmsg_read_default_cb,
				 &proc, 0);

	for (;;) {

		if (intr_flag) {
			struct command *cmd = (struct command *)r_buffer;
			if (cmd->comm_start == CMD_START) {
				unsigned int cm_code = cmd->comm_code;
				void *data = cmd->data;

				switch (cm_code) {
				case CREATE_EPT:
					ept_data = (struct ept_cmd_data *)data;
					rp_ept =
					    rpmsg_create_ept(app_rp_chnl,
							     rpmsg_read_ept_cb,
							     RPMSG_NULL,
							     ept_data->dst);
					if (rp_ept) {
						/* Send data back to ack. */
						rpmsg_sendto(app_rp_chnl,
							     r_buffer, Len,
							     Src);
					}
					break;
				case DELETE_EPT:
					rpmsg_destroy_ept(rp_ept);
					rpmsg_sendto(app_rp_chnl, r_buffer, Len,
						     Src);

					break;
				case CREATE_CHNL:
					break;
				case DELETE_CHNL:
					rpmsg_sendto(app_rp_chnl, r_buffer, Len,
						     Src);
					remoteproc_resource_deinit(proc);
					uninit = 1;
					break;
				case QUERY_FW_NAME:
					rpmsg_send(app_rp_chnl,
						   &firmware_name[0],
						   strlen(firmware_name) + 1);
					break;
				default:
					rpmsg_sendto(app_rp_chnl, r_buffer, Len,
						     Src);
					break;
				}
			} else {
				rpmsg_sendto(app_rp_chnl, r_buffer, Len, Src);
			}
			intr_flag = 0;
			if (uninit)
				break;
		}

		hil_poll(proc, 0);
	}

	cleanup_system();
	return 0;
}

/* This callback gets invoked when the remote chanl is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
	app_rp_chnl = rp_chnl;
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{

}

void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			   void *priv, unsigned long src)
{
	memcpy(r_buffer, data, len);
	Src = src;
	Len = len;
	intr_flag = 1;
}

void rpmsg_read_ept_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
		       void *priv, unsigned long src)
{
	rpmsg_send_offchannel(rp_chnl, rp_ept->addr, src, data, len);
}

void sleep()
{
	int i;
	for (i = 0; i < 1000; i++) ;
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

	/* Create HIL proc */
	hproc = platform_create_proc(proc_id);
	if (!hproc) {
		LPERROR("Failed to create hil proc.\n");
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

