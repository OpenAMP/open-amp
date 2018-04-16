#ifndef _HIL_H_
#define _HIL_H_

/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**************************************************************************
 * FILE NAME
 *
 *       hil.h
 *
 * DESCRIPTION
 *
 *       This file defines interface layer to access hardware features. This
 *       interface is used by both RPMSG and remoteproc components.
 *
 ***************************************************************************/

#include <openamp/virtqueue.h>


/**
 * hil_vring_notify()
 *
 * This function generates IPI to let the other side know that there is
 * job available for it. The required information to achieve this, like interrupt
 * vector, CPU id etc is be obtained from the proc_vring table.
 *
 * @param vq - pointer to virtqueue
 *
 */
extern void _notify(struct virtqueue *vq);

#endif				/* _HIL_H_ */
