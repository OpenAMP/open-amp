/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <errno.h>
#include <metal/mutex.h>
#include <metal/spinlock.h>
#include <metal/utilities.h>
#include <openamp/open_amp.h>
#include <openamp/rpmsg_rpc_client_server.h>
#include <string.h>
#include <fcntl.h>

#define raw_printf(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...) raw_printf("Master> " format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

int request_termination;
int ept_deleted;
int err_cnt;

void *copy_from_shbuf(void *dst, void *shbuf, int len)
{
	void *retdst = dst;

	while (len && ((uintptr_t)shbuf % sizeof(int))) {
		*(uint8_t *)dst = *(uint8_t *)shbuf;
		dst++;
		shbuf++;
		len--;
	}
	while (len >= (int)sizeof(int)) {
		*(unsigned int *)dst = *(unsigned int *)shbuf;
		dst += sizeof(int);
		shbuf += sizeof(int);
		len -= sizeof(int);
	}
	while (len > 0) {
		*(uint8_t *)dst = *(uint8_t *)shbuf;
		dst++;
		shbuf++;
		len--;
	}
	return retdst;
}

static int rpmsg_handle_open(struct rpmsg_rpc_req_open *rpc_open_req,
			     struct rpmsg_endpoint *ept)
{
	char *buf;
	struct rpmsg_rpc_resp_open rpc_open_resp;
	int payload_size = sizeof(rpc_open_resp);
	int fd, ret;

	if (!rpc_open_req || !ept)
		return -EINVAL;
	buf = rpc_open_req->filename;

	/* Open remote fd */
	fd = open(buf, rpc_open_req->flags, rpc_open_req->mode);

	/* Construct rpc response */
	rpc_open_resp.fd = fd;

	/* Transmit rpc response */
	ret = rpmsg_rpc_server_send(ept, OPEN_ID, &rpc_open_resp, payload_size);

	return ret > 0 ?  0 : ret;
}

int rpmsg_handle_close(struct rpmsg_rpc_req_close *rpc_close_req,
		       struct rpmsg_endpoint *ept)
{
	struct rpmsg_rpc_resp_close rpc_close_resp;
	int payload_size = sizeof(rpc_close_resp);
	int ret;

	if (!rpc_close_req || !ept)
		return -EINVAL;

	/* Close remote fd */
	ret = close(rpc_close_req->fd);

	/* Construct rpc response */
	rpc_close_resp.close_ret = ret;

	/* Transmit rpc response */
	ret = rpmsg_rpc_server_send(ept, CLOSE_ID, &rpc_close_resp,
				    payload_size);

	return ret > 0 ?  0 : ret;
}

int rpmsg_handle_read(struct rpmsg_rpc_req_read *rpc_read_req,
		      struct rpmsg_endpoint *ept)
{
	struct rpmsg_rpc_resp_read rpc_read_resp;
	unsigned long int bytes_read;
	int payload_size = sizeof(rpc_read_resp);
	int ret;

	if (!rpc_read_req || !ept)
		return -EINVAL;

	if (rpc_read_req->fd == 0) {
		bytes_read = MAX_STRING_LEN;
		/* Perform read from fd for large size since this is a
		 * STD/I request
		 */
		bytes_read = read(rpc_read_req->fd, rpc_read_resp.buf,
				  bytes_read);
	} else {
		/* Perform read from fd */
		bytes_read = read(rpc_read_req->fd, rpc_read_resp.buf,
				  rpc_read_req->buflen);
	}

	/* Construct rpc response */
	rpc_read_resp.bytes_read = bytes_read;

	/* Transmit rpc response */
	ret = rpmsg_rpc_server_send(ept, READ_ID, &rpc_read_resp, payload_size);

	return ret > 0 ?  0 : ret;
}

int rpmsg_handle_write(struct rpmsg_rpc_req_write *rpc_write_req,
		       struct rpmsg_endpoint *ept)
{
	struct rpmsg_rpc_resp_write rpc_write_resp;
	int payload_size = sizeof(rpc_write_resp);
	int bytes_written;
	int ret;

	if (!rpc_write_req || !ept)
		return -EINVAL;

	/* Write to remote fd */
	bytes_written = write(rpc_write_req->fd, rpc_write_req->ptr,
			      rpc_write_req->len);

	/* Construct rpc response */
	rpc_write_resp.bytes_written = bytes_written;

	/* Transmit rpc response */
	ret = rpmsg_rpc_server_send(ept, WRITE_ID, &rpc_write_resp,
				    payload_size);

	return ret > 0 ?  0 : ret;
}

int rpmsg_handle_input(struct rpmsg_rpc_req_input *rpc_input_req,
		       struct rpmsg_endpoint *ept)
{
	struct rpmsg_rpc_resp_input rpc_input_resp;
	int bytes_read;
	int payload_size = sizeof(rpc_input_resp);
	int ret;

	if (!rpc_input_req || !ept)
		return -EINVAL;

	/* Input from remote */
	scanf("%s", rpc_input_resp.buf);
	bytes_read = sizeof(rpc_input_resp.buf);

	/* Construct rpc response */
	rpc_input_resp.bytes_read = bytes_read;

	/* Transmit rpc response */
	ret = rpmsg_rpc_server_send(ept, INPUT_ID, &rpc_input_resp,
				    payload_size);
	return ret > 0 ?  0 : ret;
}

int linux_rpmsg_rpc_server_init(int id, void *data,
				struct rpmsg_endpoint *ept)
{
	int retval;

	switch (id) {
	case OPEN_ID:
		{
			retval =
			rpmsg_handle_open((struct rpmsg_rpc_req_open *)
					   data, ept);
			break;
		}
	case CLOSE_ID:
		{
			retval =
			rpmsg_handle_close((struct rpmsg_rpc_req_close *)
					    data, ept);
			break;
		}
	case READ_ID:
		{
			retval =
			rpmsg_handle_read((struct rpmsg_rpc_req_read *)
					   data, ept);
			break;
		}
	case WRITE_ID:
		{
			retval =
			rpmsg_handle_write((struct rpmsg_rpc_req_write *)
					    data, ept);
			break;
		}
	case INPUT_ID:
		{
			retval =
			rpmsg_handle_input((struct rpmsg_rpc_req_input *)
					    data, ept);
			break;
		}
	case TERM_ID:
		{
			printf("Received termination request\r\n");
			request_termination = 1;
			retval = 0;
			break;
		}
	default:
		{
			LPERROR("Invalid RPC sys call ID: %d:%d!\r\n",
				id, (int)WRITE_ID);
			retval = -1;
			break;
		}
	}

	return retval;

}

int linux_rpmsg_endpoint_server_cb(struct rpmsg_endpoint *ept, void *data,
				   size_t len,
				   uint32_t src, void *priv)
{
	unsigned char buf[MAX_BUF_LEN];
	unsigned long int id;

	(void)priv;
	(void)src;

	if (len < (int)sizeof(*syscall)) {
		LPERROR("Received data is less than the rpc structure: %zd\r\n",
			len);
		err_cnt++;
		return RPMSG_SUCCESS;
	}

	/* In case the shared memory is device memory
	 * E.g. For now, we only use UIO device memory in Linux.
	 */
	if (len > MAX_BUF_LEN)
		len = MAX_BUF_LEN;
	copy_from_shbuf(buf, data, len);
	id = *buf;

	if (linux_rpmsg_rpc_server_init(id, &buf[MAX_FUNC_ID_LEN], ept)) {
		LPRINTF("\nHandling remote procedure call errors:\r\n");
		raw_printf("rpc id %ld\r\n", id);
		err_cnt++;
	}
	return RPMSG_SUCCESS;
}

int rpmsg_rpc_server_send(struct rpmsg_endpoint *ept,
			  int rp, void *request_param,
			  size_t param_size)
{
	if (!ept)
		return -EINVAL;

	unsigned char tmpbuf[MAX_BUF_LEN];
	/* to optimize with the zero copy API */
	memcpy(tmpbuf, &rp, MAX_FUNC_ID_LEN);
	memcpy(&tmpbuf[MAX_FUNC_ID_LEN], request_param, param_size);

	return rpmsg_send(ept, tmpbuf, MAX_FUNC_ID_LEN + param_size);

}
