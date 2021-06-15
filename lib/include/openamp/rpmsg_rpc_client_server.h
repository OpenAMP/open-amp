/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPMSG_RPC_CLIENT_SERVER_H
#define RPMSG_RPC_CLIENT_SERVER_H

#include <metal/mutex.h>
#include <openamp/open_amp.h>
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

#define OPEN_ID          0x1UL
#define CLOSE_ID         0x2UL
#define WRITE_ID         0x3UL
#define READ_ID          0x4UL
#define ACK_STATUS_ID    0x5UL
#define TERM_ID          0x6UL
#define INPUT_ID         0x7UL

#define MAX_BUF_LEN 495UL
#define MAX_FUNC_ID_LEN sizeof(unsigned long int)
#define MAX_FILE_NAME_LEN 10
#define MAX_STRING_LEN 300

struct rpmsg_rpc_data;

typedef int (*rpmsg_rpc_poll)(void *arg);
typedef void (*rpmsg_rpc_shutdown_cb)(struct rpmsg_rpc_data *rpc);

extern int request_termination;
extern int ept_deleted;
extern int err_cnt;

struct rpmsg_rpc_syscall_header {
	int32_t int_field1;
	int32_t int_field2;
	uint32_t data_len;
};

struct rpmsg_rpc_syscall {
	uint32_t id;
	struct rpmsg_rpc_syscall_header args;
};

struct rpmsg_rpc_data {
	struct rpmsg_endpoint ept;
	int ept_destroyed;
	atomic_int nacked;
	void *respbuf;
	size_t respbuf_len;
	rpmsg_rpc_poll poll;
	void *poll_arg;
	rpmsg_rpc_shutdown_cb shutdown_cb;
	metal_mutex_t lock;
	struct metal_spinlock buflock;
};

struct rpmsg_rpc_req_open {
	char filename[MAX_FILE_NAME_LEN];
	int flags;
	int mode;
};

struct rpmsg_rpc_req_read {
	int fd;
	int buflen;
};

struct rpmsg_rpc_req_input {
	int buflen;
};

struct rpmsg_rpc_req_write {
	int fd;
	char ptr[MAX_STRING_LEN];
	int len;
};

struct rpmsg_rpc_req_close {
	int fd;
};

struct rpmsg_rpc_req_term {
	int id;
};

struct rpmsg_rpc_resp_open {
	int fd;
};

struct rpmsg_rpc_resp_read {
	char buf[MAX_STRING_LEN];
	int bytes_read;
};

struct rpmsg_rpc_resp_input {
	char buf[MAX_STRING_LEN];
	int bytes_read;
};

struct rpmsg_rpc_resp_write {
	int bytes_written;
};

struct rpmsg_rpc_resp_close {
	int close_ret;
};

/**
 * linux_rpmsg_rpc_init - initialize RPMsg remote procedure call
 *
 * This function is to initialize the remote procedure call
 * global data. RPMsg RPC will send request to remote and
 * wait for callback.
 *
 * @rpc: pointer to the global remote procedure call data
 * @rdev: pointer to the rpmsg device
 * @ept_name: name of the endpoint used by RPC
 * @ept_addr: address of the endpoint used by RPC
 * @ept_raddr: remote address of the endpoint used by RPC
 * @poll_arg: pointer to poll function argument
 * @poll: poll function
 * @shutdown_cb: shutdown callback function
 *
 * return 0 for success, and negative value for failure.
 */
int linux_rpmsg_rpc_init(struct rpmsg_rpc_data *rpc,
			 struct rpmsg_device *rdev,
			 const char *ept_name, uint32_t ept_addr,
			 uint32_t ept_raddr,
			 void *poll_arg, rpmsg_rpc_poll poll,
			 rpmsg_rpc_shutdown_cb shutdown_cb);

/**
 * linux_rpmsg_rpc_release - release RPMsg remote procedure call
 *
 * This function is to release remoteproc procedure call
 * global data.
 *
 * @rpc: pointer to the global remote procedure call
 *
 */
void linux_rpmsg_rpc_release(struct rpmsg_rpc_data *rpc);

/**
 * linux_rpmsg_rpc_server_init - App Callback function
 *
 * This function performs app callback based on the function id
 *
 * @id:   function id
 * @data: pointer to request parameters
 * @ept:  pointer to endpoint struct
 *
 * return negative value for failure
 */
int linux_rpmsg_rpc_server_init(int id, void *data,
				struct rpmsg_endpoint *ept);

/**
 * linux_rpmsg_endpoint_client_cb - Server Callback Function
 *
 * This function performs the callback received on the endpoint
 *
 * @ept:  pointer to endpoint struct
 * @data: pointer to data
 * @len:  size of data
 * @src:  source endpoint address
 * @priv: pointer to the platform
 *
 * return 0 for success
 */
int linux_rpmsg_endpoint_client_cb(struct rpmsg_endpoint *ept,
				   void *data, size_t len,
				   uint32_t src, void *priv);

/**
 * linux_rpmsg_endpoint_server_cb - Client Callback Function
 *
 * This function performs the callback received on the endpoint
 *
 * @ept:  pointer to endpoint struct
 * @data: pointer to data
 * @len:  size of data
 * @src:  source endpoint address
 * @priv: pointer to the platform
 *
 * return 0 for success
 */
int linux_rpmsg_endpoint_server_cb(struct rpmsg_endpoint *ept, void *data,
				   size_t len, uint32_t src, void *priv);

/**
 * rpmsg_rpc_client_send - Request RPMsg RPC call
 *
 * This function sends RPC request
 *
 * @rpc: pointer to remoteproc procedure call data struct
 * @rp:  function id
 * @request_param: pointer to request buffer
 * @param_size: length of the request data
 *
 * return length of the received response, negative value for failure.
 */
int rpmsg_rpc_client_send(struct rpmsg_rpc_data *rpc,
			  int rp, void *request_param,
			  size_t param_size);

/**
 * rpmsg_rpc_server_send - Request RPMsg RPC call
 *
 * This function sends RPC request
 *
 * @ept: pointer to endpoint struct
 * @rp:  function id
 * @request_param: pointer to request buffer
 * @param_size: length of the request data
 *
 * return length of the received response, negative value for failure.
 */
int rpmsg_rpc_server_send(struct rpmsg_endpoint *ept,
			  int rp, void *request_param,
			  size_t param_size);

#if defined __cplusplus
}
#endif

#endif /* RPMSG_RPC_CLIENT_SERVER_H */
