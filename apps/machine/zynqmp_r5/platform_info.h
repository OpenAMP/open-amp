#ifndef PLATFORM_INFO_H_
#define PLATFORM_INFO_H_

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>

/* Interrupt vectors */
#define IPI_IRQ_VECT_ID         XPAR_XIPIPSU_0_INT_ID

#define RPMSG_CHAN_NAME         "rpmsg-openamp-demo-channel"

struct remoteproc *platform_create_proc(int proc_index, int rsc_index);

struct  rpmsg_virtio_device *
platform_create_rpmsg_vdev(struct remoteproc *rproc, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev));
#endif /* PLATFORM_INFO_H_ */
