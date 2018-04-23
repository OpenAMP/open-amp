/* This is a sample demonstration application that showcases usage of rpmsg 
This application is meant to run on the remote CPU running baremetal code. 
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <$1>
#include <metal/alloc.h>
#include "rsc_table.h"

#define SHUTDOWN_MSG	0xEF56A55A
#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

struct _payload {
	unsigned long num;
	unsigned long size;
	unsigned char data[];
};

#define RPMSG_GET_KFIFO_SIZE 1
#define RPMSG_GET_AVAIL_DATA_SIZE 2
#define RPMSG_GET_FREE_SPACE 3

#define RPMSG_HEADER_LEN sizeof(struct rpmsg_hdr)
#define MAX_RPMSG_BUFF_SIZE (RPMSG_BUFFER_SIZE - RPMSG_HEADER_LEN)
#define PAYLOAD_MIN_SIZE	1
#define PAYLOAD_MAX_SIZE	(MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS		(PAYLOAD_MAX_SIZE/PAYLOAD_MIN_SIZE)
#define NS_PER_S        (1000 * 1000 * 1000)

/* Internal functions */
static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
static void rpmsg_read_cb(struct rpmsg_channel *, void *, int, void *,
			  unsigned long);
/* Globals */
static struct rpmsg_channel *app_rp_chnl;
static struct rpmsg_endpoint *rp_ept;
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
static struct _payload *i_payload;
static int rnum = 0;
static int err_cnt = 0;

/* External functions */
extern void init_system();
extern void cleanup_system();
extern struct hil_proc *platform_create_proc(int proc_index);
extern void *get_resource_table (int rsc_id, int *len);

/* Application entry point */
int app (struct hil_proc *hproc)
{
	int status = 0;
	int shutdown_msg = SHUTDOWN_MSG;
	int i;
	int size;
	int expect_rnum = 0;
	int r;
	struct timespec tp_start, tp_end;
	unsigned long long tstart, tend, tdiff;
	unsigned long long tdiff_avg_s = 0, tdiff_avg_ns = 0;

	LPRINTF(" 1 - Send data to remote core, retrieve the echo");
	LPRINTF(" and validate its integrity ..\n");

	i_payload =
	    (struct _payload *)metal_allocate_memory(2 * sizeof(unsigned long) +
				      PAYLOAD_MAX_SIZE);

	if (!i_payload) {
		LPERROR("memory allocation failed.\n");
		return -1;
	}

	/* Initialize RPMSG framework */
	status =
	    remoteproc_resource_init(&rsc_info, hproc,
				     rpmsg_channel_created,
				     rpmsg_channel_deleted, rpmsg_read_cb,
				     &proc, 1);

	if (status) {
		LPERROR("Failed to initialize remoteproc resource.\n");
		return -1;
	}

	LPRINTF("Remote proc resource initialized.\n");
	while (!app_rp_chnl) {
		hil_poll(proc->proc, 0);
	}

	LPRINTF("RPMSG channel has created.\n");
	i_payload->size = PAYLOAD_MAX_SIZE;

	/* Mark the data buffer. */
	memset(&(i_payload->data[0]), 0xA5, PAYLOAD_MAX_SIZE);
	r = clock_gettime(CLOCK_MONOTONIC, &tp_start);
	if (r == -1) {
		LPRINTF("ERROR: Bad clock_gettime!\n");
		return -1;
	}
//	for (i = 0, size = PAYLOAD_MIN_SIZE; i < (int)NUM_PAYLOADS; i++, size++) {
	for (i = 0; i < (int)0x1000; i++) {
		i_payload->num = i;

		//LPRINTF("try to send data %d...\n", i);
#if 0
		status = rpmsg_send(app_rp_chnl, i_payload,
			(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
		//LPRINTF("try to send data %d...\n", i);
		if (status < 0) {
			status = 0;
			while (!status && (rnum < expect_rnum) && !err_cnt && app_rp_chnl) {
				status = hil_poll(proc->proc, 0);
			}
			if (err_cnt || !app_rp_chnl)
				return -1;
			if (!status)
				status = rpmsg_send(app_rp_chnl, i_payload,
					(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
			if (status < 0) {
				LPRINTF("Error sending data...\n");
				return -1;
			}
		}
#else
		if (expect_rnum && !(expect_rnum%512)) {
			status = 0;
			while (!status && (rnum < expect_rnum) && !err_cnt && app_rp_chnl) {
				status = hil_poll(proc->proc, 0);
			}
			if (err_cnt || !app_rp_chnl)
				return -1;
		}
		status = rpmsg_send(app_rp_chnl, i_payload,
			(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
		if (status < 0) {
			LPRINTF("Error sending data...\n");
			return -1;
		}
#endif

		expect_rnum++;
		//LPRINTF("sent data %d, %d...\n", i, rnum);
	}

	while ((rnum < expect_rnum) && !err_cnt && app_rp_chnl) {
		hil_poll(proc->proc, 0);
	}

	r = clock_gettime(CLOCK_MONOTONIC, &tp_end);
	if (r == -1) {
		LPRINTF("ERROR: Bad clock_gettime!\n");
		return -1;
	}

	LPRINTF("**********************************\n");
	LPRINTF(" Test Results: Error count = %d \n", err_cnt);
	LPRINTF("**********************************\n");
	tstart = (tp_start.tv_sec * NS_PER_S) + tp_start.tv_nsec;
	tend = (tp_end.tv_sec * NS_PER_S) + tp_end.tv_nsec;
	tdiff = (tend - tstart)/rnum;
	tdiff_avg_s = tdiff / NS_PER_S;
	tdiff_avg_ns = tdiff % NS_PER_S;
	LPRINTF("Total packages: %d, time_avg = %lds, %ldns\n",
		rnum, (long int)tdiff_avg_s, (long int)tdiff_avg_ns);
	/* Send shutdown message to remote */
	rpmsg_send(app_rp_chnl, &shutdown_msg, sizeof(int));
	sleep(1);
	LPRINTF("Quitting application .. Echo test end\n");

	remoteproc_resource_deinit(proc);
	cleanup_system();
	metal_free_memory(i_payload);
	return 0;
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
	app_rp_chnl = rp_chnl;
	rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
				  RPMSG_ADDR_ANY);
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{
	(void)rp_chnl;
	rpmsg_destroy_ept(rp_ept);
	LPRINTF("%s\n", __func__);
	app_rp_chnl = NULL;
	rp_ept = NULL;
}

static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			  void *priv, unsigned long src)
{
	(void)rp_chnl;
	(void)src;
	(void)priv;
	int i;
	struct _payload *r_payload = (struct _payload *)data;

#if 0
	if (r_payload->size == 0) {
		LPERROR(" Invalid size of package is received.\n");
		err_cnt++;
		return;
	}
	if (r_payload->num != rnum) {
		LPERROR("Package sequence error: expect: %d, actual: %d\n",
			rnum, r_payload->num);
		return;
	}
	/* Validate data buffer integrity. */
	for (i = 0; i < (int)r_payload->size; i++) {
		if (r_payload->data[i] != 0xA5) {
			LPRINTF("Data corruption at index %d\n", i);
			err_cnt++;
			break;
		}
	}
#endif
	rnum = r_payload->num + 1;
	//LPRINTF("%s: %d.\n", __func__, rnum);
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

