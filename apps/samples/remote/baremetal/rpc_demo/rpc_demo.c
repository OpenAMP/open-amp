/* This is a sample demonstration application that showcases usage of proxy from the remote core. 
 This application is meant to run on the remote CPU running baremetal.
 This applicationr can print to to master console and perform file I/O using proxy mechanism. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "baremetal.h"
#include "rpmsg_retarget.h"

/* Internal functions */
static void init_system();
static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
static void rpmsg_read_cb(struct rpmsg_channel *, void *, int, void *, unsigned long);
static void shutdown_cb(struct rpmsg_channel *rp_chnl);

/* Globals */
static struct rpmsg_channel *app_rp_chnl;
volatile int chnl_cb_flag = 0;
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;

#define REDEF_O_CREAT 100
#define REDEF_O_EXCL 200
#define REDEF_O_RDONLY 0
#define REDEF_O_WRONLY 1
#define REDEF_O_RDWR 2
#define REDEF_O_APPEND 2000
#define REDEF_O_ACCMODE 3

#define RPC_CHANNEL_READY_TO_CLOSE "rpc_channel_ready_to_close"

/* Application entry point */
int main() {
	int fd, bytes_written, bytes_read;
	char fname[] = "remote.file";
	char wbuff[50];
	char rbuff[1024];
	char ubuff[50];
	float fdata;
	int idata;
	int ret;
	int status;

#ifdef ZYNQ_A9
	SWITCH_TO_SYS_MODE();
#endif

	/* Initialize HW system components */
	init_system();

	rsc_info.rsc_tab = (struct resource_table *) &resources;
	rsc_info.size = sizeof(resources);

	/* Initialize RPMSG framework */
	status = remoteproc_resource_init(&rsc_info, rpmsg_channel_created,
					rpmsg_channel_deleted, rpmsg_read_cb, &proc);
	if (status < 0) {
		return -1;
	}

	while (!chnl_cb_flag) {
		__asm__ ( "\
			wfi\n\t" \
		);
	}

	chnl_cb_flag = 0;

	rpmsg_retarget_init(app_rp_chnl, shutdown_cb);

	printf("\r\nRemote>Baremetal Remote Procedure Call (RPC) Demonstration\r\n");
	printf("\r\nRemote>***************************************************\r\n");
	printf("\r\nRemote>Rpmsg based retargetting to proxy initialized..\r\n");

	/* Remote performing file IO on Master */
	printf("\r\nRemote>FileIO demo ..\r\n");

	printf("\r\nRemote>Creating a file on master and writing to it..\r\n");
	fd = open(fname, REDEF_O_CREAT | REDEF_O_WRONLY | REDEF_O_APPEND,
			S_IRUSR | S_IWUSR);
	printf("\r\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);

	sprintf(wbuff, "This is a test string being written to file..");
	bytes_written = write(fd, wbuff, strlen(wbuff));
	printf("\r\nRemote>Wrote to fd = %d, size = %d, content = %s\r\n", fd,
			bytes_written, wbuff);
	close(fd);
	printf("\r\nRemote>Closed fd = %d\r\n", fd);

	/* Remote performing file IO on Master */
	printf("\r\nRemote>Reading a file on master and displaying its contents..\r\n");
	fd = open(fname, REDEF_O_RDONLY, S_IRUSR | S_IWUSR);
	printf("\r\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);
	bytes_read = read(fd, rbuff, 1024);
	*(char*) (&rbuff[0] + bytes_read + 1) = 0;
	printf( "\r\nRemote>Read from fd = %d, size = %d, printing contents below .. %s\r\n",
			fd, bytes_read, rbuff);
	close(fd);
	printf("\r\nRemote>Closed fd = %d\r\n", fd);

	while (1) {
		/* Remote performing STDIO on Master */
		printf("\r\nRemote>Remote firmware using scanf and printf ..\r\n");
		printf("\r\nRemote>Scanning user input from master..\r\n");
		printf("\r\nRemote>Enter name\r\n");
		ret = scanf("%s", ubuff);
		if (ret) {
			printf("\r\nRemote>Enter age\r\n");
			ret = scanf("%d", &idata);
			if(ret) {
				printf("\r\nRemote>Enter value for pi\r\n");
				ret = scanf("%f", &fdata);
				if(ret) {
					printf("\r\nRemote>User name = '%s'\r\n", ubuff);
					printf("\r\nRemote>User age = '%d'\r\n", idata);
					printf("\r\nRemote>User entered value of pi = '%f'\r\n", fdata);
				}
			}
		}
		if(!ret) {
			scanf("%s", ubuff);
			printf("Remote> Invalid value. Starting again....");
		} else {
			printf("\r\nRemote>Repeat demo ? (enter yes or no) \r\n");
			scanf("%s", ubuff);
			if((strcmp(ubuff,"no")) && (strcmp(ubuff,"yes"))) {
				printf("\r\nRemote>Invalid option. Starting again....\r\n");
			} else if((!strcmp(ubuff,"no"))) {
				printf("\r\nRemote>RPC retargetting quitting ...\r\n");
				sprintf(wbuff, RPC_CHANNEL_READY_TO_CLOSE);
				rpmsg_retarget_send(wbuff, sizeof(RPC_CHANNEL_READY_TO_CLOSE) + 1);
				break;
			}
		}
	}
	printf("\r\nRemote> Firmware's rpmsg-openamp-demo-channel going down! \r\n");

	while (1) {
		__asm__ ( "\
			wfi\n\t" \
		);
	}

	return 0;
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {
	app_rp_chnl = rp_chnl;
	chnl_cb_flag = 1;
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
}

static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
						void * priv, unsigned long src) {
}

static void shutdown_cb(struct rpmsg_channel *rp_chnl) {
	rpmsg_retarget_deinit(rp_chnl);
	remoteproc_resource_deinit(proc);
}

static void init_system() {
#ifdef ZYNQMP_R5
	/* Initilaize GIC */
	zynqMP_r5_gic_initialize();
#else
#ifdef ZYNQ_A9
	/* Place the vector table at the image entry point */
	arm_arch_install_isr_vector_table(RAM_VECTOR_TABLE_ADDR);

	/* Enable MMU */
	arm_ar_mem_enable_mmu();

	/* Initialize ARM stacks */
	init_arm_stacks();

	/* Initialize GIC */
	zc702evk_gic_initialize();
#endif
#endif
}
