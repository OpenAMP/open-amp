/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#ifndef _RPMSG_CORE_H_
#define _RPMSG_CORE_H_

#include <stdint.h>
#include <metal/list.h>
#include <metal/mutex.h>
#include <openamp/rpmsg.h>

#if defined __cplusplus
extern "C" {
#endif

#ifdef RPMSG_DEBUG
#define RPMSG_ASSERT(_exp, _msg) do { \
		if (!(_exp)) { \
			openamp_print("FATAL: %s - "_msg, __func__); \
			while (1); \
		} \
	} while (0)
#else
#define RPMSG_ASSERT(_exp, _msg) if (!(_exp)) while (1)
#endif

int rpmsg_get_address(unsigned long *bitmap, int size);
int rpmsg_set_address(unsigned long *bitmap, int size, int addr);
int rpmsg_release_address(unsigned long *bitmap, int size, int addr);
int rpmsg_is_address_set(unsigned long *bitmap, int size, int addr);
int rpmsg_send_ns_message(struct rpmsg_endpoint *ept, unsigned long flags);

struct rpmsg_endpoint *rpmsg_get_endpoint(struct rpmsg_device *rvdev,
					  const char *name, uint32_t addr,
					  uint32_t dest_addr);
int rpmsg_register_endpoint(struct rpmsg_device *rdev,
			    struct rpmsg_endpoint *ept);
void rpmsg_unregister_endpoint(struct rpmsg_endpoint *ept);

struct rpmsg_endpoint *
rpmsg_get_ept_from_remote_addr(struct rpmsg_device *rdev,
			       uint32_t dest_addr);


struct rpmsg_endpoint *
rpmsg_get_ept_from_addr(struct rpmsg_device *rdev, uint32_t addr);
#if defined __cplusplus
}
#endif

#endif /* _RPMSG_CORE_H_ */
