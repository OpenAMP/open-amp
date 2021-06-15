/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <metal/mutex.h>
#include <metal/spinlock.h>
#include <metal/utilities.h>
#include <openamp/open_amp.h>
#include <openamp/rpmsg_rpc_client_server.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

void linux_rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	struct rpmsg_rpc_data *rpc;

	rpc = metal_container_of(ept, struct rpmsg_rpc_data, ept);
	rpc->ept_destroyed = 1;
	rpmsg_destroy_ept(ept);
	atomic_flag_clear(&rpc->nacked);
	if (rpc->shutdown_cb)
		rpc->shutdown_cb(rpc);
}

int linux_rpmsg_rpc_init(struct rpmsg_rpc_data *rpc,
			 struct rpmsg_device *rdev,
			 const char *ept_name, uint32_t ept_addr,
			 uint32_t ept_raddr,
			 void *poll_arg, rpmsg_rpc_poll poll,
			 rpmsg_rpc_shutdown_cb shutdown_cb)
{
	int ret;

	if (!rpc || !rdev)
		return -EINVAL;
	metal_spinlock_init(&rpc->buflock);
	metal_mutex_init(&rpc->lock);
	rpc->shutdown_cb = shutdown_cb;
	rpc->poll_arg = poll_arg;
	rpc->poll = poll;
	rpc->ept_destroyed = 0;
	rpc->respbuf = NULL;
	rpc->respbuf_len = 0;
	atomic_init(&rpc->nacked, 1);
	ret = rpmsg_create_ept(&rpc->ept, rdev,
			       ept_name, ept_addr, ept_raddr,
			       linux_rpmsg_endpoint_client_cb,
			       linux_rpmsg_service_unbind);
	if (ret != 0) {
		metal_mutex_release(&rpc->lock);
		return -EINVAL;
	}
	while (!is_rpmsg_ept_ready(&rpc->ept)) {
		if (rpc->poll)
			rpc->poll(rpc->poll_arg);
	}
	return 0;
}

int rpmsg_rpc_client_send(struct rpmsg_rpc_data *rpc,
			  int rp, void *request_param,
			  size_t param_size)
{
	int ret;

	if (!rpc)
		return -EINVAL;

	unsigned char tmpbuf[MAX_BUF_LEN];
	/* to optimize with the zero copy API */
	memcpy(tmpbuf, &rp, MAX_FUNC_ID_LEN);
	memcpy(&tmpbuf[MAX_FUNC_ID_LEN], request_param, param_size);
	(void)atomic_flag_test_and_set(&rpc->nacked);
	ret = rpmsg_send(&rpc->ept, tmpbuf, MAX_FUNC_ID_LEN + param_size);
	if (ret < 0)
		return -EINVAL;
	return ret;

}

void linux_rpmsg_rpc_release(struct rpmsg_rpc_data *rpc)
{
	if (!rpc)
		return;
	if (rpc->ept_destroyed == 0)
		rpmsg_destroy_ept(&rpc->ept);
	metal_mutex_acquire(&rpc->lock);
	metal_spinlock_acquire(&rpc->buflock);
	rpc->respbuf = NULL;
	rpc->respbuf_len = 0;
	metal_spinlock_release(&rpc->buflock);
	metal_mutex_release(&rpc->lock);
	metal_mutex_deinit(&rpc->lock);
}

int linux_rpmsg_endpoint_client_cb(struct rpmsg_endpoint *ept,
				   void *data, size_t len,
				   uint32_t src, void *priv)
{
	int *resp_id = data;

	(void)priv;
	(void)src;

	if (data && ept) {
		if (*resp_id == TERM_ID) {
			rpmsg_destroy_ept(ept);
		} else {
			struct rpmsg_rpc_data *rpc;

			rpc = metal_container_of(ept,
						 struct rpmsg_rpc_data,
						 ept);
			metal_spinlock_acquire(&rpc->buflock);
			if (rpc->respbuf && rpc->respbuf_len != 0) {
				if (len > rpc->respbuf_len)
					len = rpc->respbuf_len;
				rpc->respbuf = data;
			}
			atomic_flag_clear(&rpc->nacked);
			metal_spinlock_release(&rpc->buflock);
		}
	}

	return RPMSG_SUCCESS;
}

