/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This is a sample demonstration application that showcases usage of proxy
 *from the remote core.
 *This application is meant to run on the remote CPU running linux.
 *This application can print to the master console and perform file I/O through
 *rpmsg channels.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <openamp/open_amp.h>
#include <openamp/rpmsg_rpc_client_server.h>
#include "rsc_table.h"
#include "platform_info.h"
#include "rpmsg-rpc-demo.h"

#define REDEF_O_CREAT   0000100
#define REDEF_O_EXCL    0000200
#define REDEF_O_RDONLY  0000000
#define REDEF_O_WRONLY  0000001
#define REDEF_O_RDWR    0000002
#define REDEF_O_APPEND  0002000
#define REDEF_O_ACCMODE 0000003

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static struct rpmsg_rpc_data *rpmsg_default_rpc;

static void rpmsg_rpc_shutdown(struct rpmsg_rpc_data *rpc)
{
	(void)rpc;
	LPRINTF("RPMSG RPC is shutting down.\r\n");
}

void linux_rpmsg_set_default_rpc(struct rpmsg_rpc_data *rpc)
{
	if (!rpc)
		return;
	rpmsg_default_rpc = rpc;
}

/*************************************************************************
 *
 *   FUNCTION
 *
 *       rpmsg_open
 *
 *   DESCRIPTION
 *
 *       Open a file.
 *
 *************************************************************************/
int rpmsg_open(const char *filename, int flags, int mode)
{
	struct rpmsg_rpc_data *rpc = rpmsg_default_rpc;
	struct rpmsg_rpc_req_open rpc_open_req;
	struct rpmsg_rpc_resp_open rpc_open_resp;
	int filename_len = strlen(filename) + 1;
	unsigned int payload_size = sizeof(rpc_open_req);
	int ret;

	if (!filename || payload_size > (int)MAX_BUF_LEN) {
		return -EINVAL;
	}

	if (!rpc)
		return -EINVAL;

	/* Construct rpc payload */
	rpc_open_req.flags = flags;
	rpc_open_req.mode = mode;
	memcpy(rpc_open_req.filename, filename, filename_len);

	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = (void *)&rpc_open_resp;
	rpc->respbuf_len = sizeof(rpc_open_resp);
	metal_spinlock_release(&rpc->buflock);
	ret = rpmsg_rpc_client_send(rpc, OPEN_ID, &rpc_open_req, payload_size);

	while ((atomic_flag_test_and_set(&rpc->nacked))) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}

	if (ret >= 0) {
		/* Obtain return args and return to caller */
		int *id = rpc->respbuf;

		if (*id == OPEN_ID) {
			void *resp_ptr = rpc->respbuf + MAX_FUNC_ID_LEN;
			struct rpmsg_rpc_resp_open *resp =
			(struct rpmsg_rpc_resp_open *)resp_ptr;

			ret = resp->fd;
		} else
			ret = -EINVAL;
	}
	return ret;
}

/*************************************************************************
 *
 *   FUNCTION
 *
 *       rpmsg_read
 *
 *   DESCRIPTION
 *
 *       Read data through RPMsg channel
 *
 *************************************************************************/
int rpmsg_read(int fd, char *buffer, int buflen)
{
	struct rpmsg_rpc_req_read rpc_read_req;
	struct rpmsg_rpc_data *rpc = rpmsg_default_rpc;
	struct rpmsg_rpc_resp_read rpc_read_resp;
	int payload_size = sizeof(rpc_read_req);
	int ret;

	if (!rpc || !buffer || buflen == 0)
		return -EINVAL;

	/* Construct rpc payload */
	rpc_read_req.fd = fd;
	rpc_read_req.buflen = buflen;

	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = (void *)&rpc_read_resp;
	rpc->respbuf_len = sizeof(rpc_read_resp);
	metal_spinlock_release(&rpc->buflock);

	ret = rpmsg_rpc_client_send(rpc, READ_ID, &rpc_read_req, payload_size);

	while ((atomic_flag_test_and_set(&rpc->nacked))) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}

	/* Obtain return args and return to caller */
	if (ret >= 0) {
		int *id = rpc->respbuf;

		if (*id == READ_ID) {
			void *resp_ptr = rpc->respbuf + MAX_FUNC_ID_LEN;
			struct rpmsg_rpc_resp_read *resp =
			(struct rpmsg_rpc_resp_read *)resp_ptr;

			if (resp->bytes_read > 0) {
				memcpy(buffer, resp->buf, resp->bytes_read);
			}
			ret = resp->bytes_read;
		} else
			ret = -EINVAL;
	}

	return ret;
}

/*************************************************************************
 *
 *   FUNCTION
 *
 *       rpmsg_write
 *
 *   DESCRIPTION
 *
 *       Write data through RPMsg channel
 *
 *************************************************************************/
int rpmsg_write(int fd, char *ptr, int len)
{
	int ret;
	struct rpmsg_rpc_req_write rpc_write_req;
	int payload_size = sizeof(rpc_write_req);
	struct rpmsg_rpc_data *rpc = rpmsg_default_rpc;
	struct rpmsg_rpc_resp_write rpc_write_resp;

	if (!rpc)
		return -EINVAL;

	rpc_write_req.fd = fd;
	memcpy(rpc_write_req.ptr, ptr, len + 1);
	rpc_write_req.len = len;
	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = (void *)&rpc_write_resp;
	rpc->respbuf_len = sizeof(rpc_write_resp);
	metal_spinlock_release(&rpc->buflock);
	ret = rpmsg_rpc_client_send(rpc, WRITE_ID, &rpc_write_req,
				    payload_size);

	while ((atomic_flag_test_and_set(&rpc->nacked))) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}

	if (ret >= 0) {
		/* Obtain return args and return to caller */
		int *id = rpc->respbuf;

		if (*id == WRITE_ID) {
			void *resp_ptr = rpc->respbuf + MAX_FUNC_ID_LEN;
			struct rpmsg_rpc_resp_write *resp =
			(struct rpmsg_rpc_resp_write *)resp_ptr;

			ret = resp->bytes_written;
		} else
			ret = -EINVAL;
	}

	return ret;

}

/*************************************************************************
 *
 *   FUNCTION
 *
 *       rpmsg_close
 *
 *   DESCRIPTION
 *
 *       Close a file.
 *
 *************************************************************************/
int rpmsg_close(int fd)
{
	struct rpmsg_rpc_data *rpc = rpmsg_default_rpc;
	int ret;
	struct rpmsg_rpc_req_close rpc_close_req;
	struct rpmsg_rpc_resp_close rpc_close_resp;
	int payload_size = sizeof(rpc_close_req);

	if (!rpc)
		return -EINVAL;

	rpc_close_req.fd = fd;
	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = (void *)&rpc_close_resp;
	rpc->respbuf_len = sizeof(rpc_close_resp);
	metal_spinlock_release(&rpc->buflock);
	ret = rpmsg_rpc_client_send(rpc, CLOSE_ID, &rpc_close_req,
				    payload_size);

	while ((atomic_flag_test_and_set(&rpc->nacked))) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}

	if (ret >= 0) {
		/* Obtain return args and return to caller */
		int *id = rpc->respbuf;

		if (*id == CLOSE_ID) {
			void *resp_ptr = rpc->respbuf + MAX_FUNC_ID_LEN;
			struct rpmsg_rpc_resp_close *resp =
			(struct rpmsg_rpc_resp_close *)resp_ptr;

			ret = resp->close_ret;
		} else
			ret = -EINVAL;
	}

	return ret;
}

/*************************************************************************
 *
 *   FUNCTION
 *
 *       rpmsg_input
 *
 *   DESCRIPTION
 *
 *       Read input through RPMsg channel
 *
 *************************************************************************/
int rpmsg_input(char *buffer, int buflen)
{
	struct rpmsg_rpc_req_input rpc_input_req;
	struct rpmsg_rpc_data *rpc = rpmsg_default_rpc;
	struct rpmsg_rpc_resp_input rpc_input_resp;
	int payload_size = sizeof(rpc_input_req);
	int ret;

	if (!rpc || !buffer || buflen == 0)
		return -EINVAL;

	/* Construct rpc payload */
	rpc_input_req.buflen = buflen;
	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = (void *)&rpc_input_resp;
	rpc->respbuf_len = sizeof(rpc_input_resp);
	metal_spinlock_release(&rpc->buflock);
	ret = rpmsg_rpc_client_send(rpc, INPUT_ID, &rpc_input_req,
				    payload_size);

	while ((atomic_flag_test_and_set(&rpc->nacked))) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}

	/* Obtain return args and return to caller */
	if (ret >= 0) {
		int *id = rpc->respbuf;

		if (*id == INPUT_ID) {
			void *resp_ptr = rpc->respbuf + MAX_FUNC_ID_LEN;
			struct rpmsg_rpc_resp_input *resp =
			(struct rpmsg_rpc_resp_input *)resp_ptr;

			if (resp->bytes_read > 0) {
				memcpy(buffer, resp->buf, resp->bytes_read);
			}
			ret = resp->bytes_read;
		} else {
			ret = -EINVAL;
		}
	}

	return ret;
}

/*-----------------------------------------------------------------------------*
 *  Application specific
 *-----------------------------------------------------------------------------
 */
int app(struct rpmsg_device *rdev, void *priv)
{
	struct rpmsg_rpc_data rpc;
	struct rpmsg_rpc_req_term rpccall;
	int fd, bytes_written, bytes_read;
	char *fname = "remote.file";
	char wbuff[50];
	char rbuff[1024];
	char ubuff[50];
	char idata[50];
	char fdata[50];
	int ret;

	/* redirect I/Os */
	LPRINTF("Initializating I/Os redirection...\r\n");
	ret = linux_rpmsg_rpc_init(&rpc, rdev, RPMSG_SERVICE_NAME,
				   RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
				   priv, platform_poll, rpmsg_rpc_shutdown);

	linux_rpmsg_set_default_rpc(&rpc);
	if (ret) {
		LPRINTF("Failed to initialize rpmsg rpc\r\n");
		return -1;
	}

	printf("\nRemote>Linux Remote Procedure Call (RPC) Demonstration\r\n");
	printf("\nRemote>***************************************************"
	"\r\n");

	printf("\nRemote>Rpmsg based retargetting to proxy initialized..\r\n");

	/* Remote performing file IO on Master */
	printf("\nRemote>FileIO demo ..\r\n");

	printf("\nRemote>Creating a file on master and writing to it..\r\n");
	fd = rpmsg_open(fname, REDEF_O_CREAT | REDEF_O_WRONLY | REDEF_O_APPEND,
			S_IRUSR | S_IWUSR);
	printf("\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);
	sprintf(wbuff, "This is a test string being written to file..");
	bytes_written = rpmsg_write(fd, wbuff, strlen(wbuff));
	printf("\nRemote>Wrote to fd = %d, size = %d, content = %s\r\n", fd,
	       bytes_written, wbuff);
	rpmsg_close(fd);
	printf("\nRemote>Closed fd = %d\r\n", fd);

	/* Remote performing file IO on Master */
	printf("\nRemote>Reading a file on master and displaying its "
	"contents..\r\n");
	fd = rpmsg_open(fname, REDEF_O_RDONLY, S_IRUSR | S_IWUSR);
	printf("\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);
	bytes_read = rpmsg_read(fd, rbuff, sizeof(rbuff));
	*(char *)(&rbuff[0] + bytes_read + 1) = 0;
	printf("\nRemote>Read from fd = %d, size = %d, "
	"printing contents below .. %s\r\n", fd, bytes_read, rbuff);
	rpmsg_close(fd);
	printf("\nRemote>Closed fd = %d\r\n", fd);

	while (1) {
		/* Remote performing STDIO on Master */
		printf("\nRemote>Remote firmware using scanf and printf .."
		"\r\n");
		printf("\nRemote>Scanning user input from master..\r\n");
		printf("\nRemote>Enter name\r\n");
		ret = rpmsg_input(ubuff, sizeof(ubuff));
		if (ret) {
			printf("\nRemote>Enter age\r\n");
			ret = rpmsg_input(idata, sizeof(idata));
			if (ret) {
				printf("\nRemote>Enter value for pi\r\n");
				ret = rpmsg_input(fdata, sizeof(fdata));
				if (ret) {
					printf("\nRemote>User name = '%s'"
					"\r\n", ubuff);
					printf("\nRemote>User age = '%s'\r\n",
					       idata);
					printf("\nRemote>User entered value of "
					"pi = '%s'\r\n", fdata);
				}
			}
		}
		if (!ret) {
			rpmsg_input(ubuff, sizeof(ubuff));
			printf("Remote> Invalid value. Starting again....");
		} else {
			printf("\nRemote>Repeat demo ? (enter yes or no) \r\n");
			scanf("%s", ubuff);
			if ((strcmp(ubuff, "no")) && (strcmp(ubuff, "yes"))) {
				printf("\nRemote>Invalid option. Starting again"
				"....\r\n");
			} else if ((!strcmp(ubuff, "no"))) {
				printf("\nRemote>RPC retargetting quitting ..."
				"\r\n");
				break;
			}
		}
	}

	printf("\nRemote> Firmware's rpmsg-rpc-channel going down! \r\n");
	rpccall.id = TERM_ID;
	(void)rpmsg_rpc_client_send(&rpc, TERM_ID, &rpccall, sizeof(rpccall));

	LPRINTF("Release remoteproc procedure call\r\n");
	linux_rpmsg_rpc_release(&rpc);
	return 0;
}

/*-----------------------------------------------------------------------------*
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
						   VIRTIO_DEV_SLAVE,
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
