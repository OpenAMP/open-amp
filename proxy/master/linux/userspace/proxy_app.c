#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "proxy_app.h"

#define RPC_BUFF_SIZE 512

struct _proxy_data {
	int active;
	int rpmsg_proxy_fd;
	struct _sys_rpc *rpc;
	struct _sys_rpc *rpc_response;
	char *firmware_path;
};

static struct _proxy_data *proxy;
char fw_dst_path[] = "/lib/firmware/firmware";
char cp_cmd[512];

int handle_open(struct _sys_rpc *rpc)
{
	int fd, bytes_written;

	/* Open remote fd */
	fd = open(rpc->sys_call_args.data, rpc->sys_call_args.int_field1,
			rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = OPEN_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = fd;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_close(struct _sys_rpc *rpc)
{
	int retval, bytes_written;

	/* Close remote fd */
	retval = close(rpc->sys_call_args.int_field1);

	/* Construct rpc response */
	proxy->rpc_response->id = CLOSE_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = retval;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_read(struct _sys_rpc *rpc)
{
	int bytes_read, bytes_written, payload_size;
	char *buff;

	/* Allocate buffer for requested data size */
	buff = malloc(rpc->sys_call_args.int_field2);

	if (rpc->sys_call_args.int_field1 == 0)
		/* Perform read from fd for large size since this is a
		STD/I request */
		bytes_read = read(rpc->sys_call_args.int_field1, buff, 512);
	else
		/* Perform read from fd */
		bytes_read = read(rpc->sys_call_args.int_field1, buff,
					rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = READ_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = bytes_read;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /* not used */
	proxy->rpc_response->sys_call_args.data_len = bytes_read;
	if (bytes_read > 0)
		memcpy(proxy->rpc_response->sys_call_args.data, buff,
			bytes_read);

	payload_size = sizeof(struct _sys_rpc) +
			((bytes_read > 0) ? bytes_read : 0);

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
					payload_size);

	return (bytes_written != payload_size) ? -1 : 0;
}

int handle_write(struct _sys_rpc *rpc)
{
	int bytes_written;

	/* Write to remote fd */
	bytes_written = write(rpc->sys_call_args.int_field1,
				rpc->sys_call_args.data,
				rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = WRITE_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = bytes_written;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_rpc(struct _sys_rpc *rpc)
{
	int retval;

	/* Handle RPC */
	switch (rpc->id) {
	case OPEN_SYSCALL_ID:
	{
		retval = handle_open(rpc);
		break;
	}
	case CLOSE_SYSCALL_ID:
	{
		retval = handle_close(rpc);
		break;
	}
	case READ_SYSCALL_ID:
	{
		retval = handle_read(rpc);
		break;
	}
	case WRITE_SYSCALL_ID:
	{
		retval = handle_write(rpc);
		break;
	}
	default:
	{
		printf("\r\nMaster>Err:Invalid RPC sys call ID! \r\n");
		retval = -1;
		break;
	}
	}

	return retval;
}

int terminate_rpc_app()
{
	int bytes_written;
	int msg = TERM_SYSCALL_ID;
	bytes_written = write(proxy->rpmsg_proxy_fd, &msg, sizeof(int));
	return bytes_written;
}

void exit_action_handler(int signum)
{
	proxy->active = 0;
}

void kill_action_handler(int signum)
{
	printf("\r\nMaster>RPC service killed !!\r\n");

	/* Send shutdown signal to remote application */
	terminate_rpc_app();
	/* Close proxy rpmsg device */
	close(proxy->rpmsg_proxy_fd);

	/* Free up resources */
	free(proxy->rpc);
	free(proxy->rpc_response);
	free(proxy);

	/* Unload drivers */
	system("modprobe -r rpmsg_proxy_dev_driver");
	system("modprobe -r virtio_rpmsg_bus");
	system("modprobe -r zynq_remoteproc_driver");
	system("modprobe -r remoteproc");
	system("modprobe -r virtio_ring");
	system("modprobe -r virtio");
}

void display_help_msg()
{
	printf("\r\nLinux proxy application.\r\n");
	printf("-r	 Displays proxy application version.\n");
	printf("-f	 Accepts path of firmware to load on remote core.\n");
	printf("-h	 Displays this help message.\n");
}
int main(int argc, char *argv[])
{
	struct sigaction exit_action;
	struct sigaction kill_action;
	unsigned int bytes_rcvd;
	int i = 0;

	/* Initialize signalling infrastructure */
	memset(&exit_action, 0, sizeof(struct sigaction));
	memset(&kill_action, 0, sizeof(struct sigaction));
	exit_action.sa_handler = exit_action_handler;
	kill_action.sa_handler = kill_action_handler;
	sigaction(SIGTERM, &exit_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGKILL, &kill_action, NULL);
	sigaction(SIGHUP, &kill_action, NULL);

	/* Parse and process input args */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			printf("\r\nLinux proxy application version 1.0\r\n");
			return 0;
		} else if (strcmp(argv[i], "-h") == 0) {
			display_help_msg();
			return 0;
		} else if (strcmp(argv[i], "-f") == 0) {
			if (i+1 < argc)
				/* Construct file copy command string */
				sprintf(cp_cmd , "cp %s %s", argv[i+1],
						fw_dst_path);
		}
	}

	/* Bring up remote firmware */
	printf("\r\nMaster>Loading remote firmware\r\n");
	system(cp_cmd);
	system("modprobe zynq_remoteproc_driver");

	/* Create rpmsg proxy device */
	printf("\r\nMaster>Create rpmsg proxy device\r\n");
	system("modprobe rpmsg_proxy_dev_driver");

	/* Allocate memory for proxy data structure */
	proxy = malloc(sizeof(struct _proxy_data));

	/* Open proxy rpmsg device */
	printf("\r\nMaster>Opening rpmsg proxy device\r\n");
	do {
		proxy->rpmsg_proxy_fd = open("/dev/rpmsg_proxy", O_RDWR);

	} while (proxy->rpmsg_proxy_fd < 0);

	/* Allocate memory for rpc payloads */
	proxy->active = 1;
	proxy->rpc = malloc(RPC_BUFF_SIZE);
	proxy->rpc_response = malloc(RPC_BUFF_SIZE);

	/* RPC service starts */
	printf("\r\nMaster>RPC service started !!\r\n");
	while (proxy->active) {
		/* Block on read for rpc requests from remote context */
		bytes_rcvd = read(proxy->rpmsg_proxy_fd, proxy->rpc,
					RPC_BUFF_SIZE);

		/* User event, break! */
		if (!proxy->active)
			break;

		/* Handle rpc */
		if (handle_rpc(proxy->rpc)) {
			printf("\r\nMaster>Err:Handling remote procedure");
			printf(" call!\r\n");
			printf("\r\nrpc id %d\r\n", proxy->rpc->id);
			printf("\r\nrpc int field1 %d\r\n",
				proxy->rpc->sys_call_args.int_field1);
			printf("\r\nrpc int field2 %d\r\n",
				proxy->rpc->sys_call_args.int_field2);
			break;
		}
	}

	printf("\r\nMaster>RPC service exiting !!\r\n");

	/* Send shutdown signal to remote application */
	terminate_rpc_app();
	/* FIXME: May be wait here for sometime to allow remote application
	to complete its uninitialization. */

	/* Close proxy rpmsg device */
	close(proxy->rpmsg_proxy_fd);

	/* Free up resources */
	free(proxy->rpc);
	free(proxy->rpc_response);
	free(proxy);

	/* Unload drivers */
	system("modprobe -r rpmsg_proxy_dev_driver");
	system("modprobe -r virtio_rpmsg_bus");
	system("modprobe -r zynq_remoteproc_driver");
	system("modprobe -r remoteproc");
	system("modprobe -r virtio_ring");
	system("modprobe -r virtio");

	return 0;
}

