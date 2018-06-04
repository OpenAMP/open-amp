/* This is a sample demonstration application that showcases usage of remoteproc
and rpmsg APIs on the remote core. This application is meant to run on the remote CPU
running baremetal code. This applicationr receives two matrices from the master,
multiplies them and returns the result to the master core. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openamp/open_amp.h>
#include "rsc_table.h"
#include "platform_info.h"

#define	MAX_SIZE		6
#define NUM_MATRIX		2

#define SHUTDOWN_MSG	0xEF56A55A

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

typedef struct _matrix {
	unsigned int size;
	unsigned int elements[MAX_SIZE][MAX_SIZE];
} matrix;

/* Local variables */
static struct rpmsg_endpoint lept;
static int ept_deleted = 0;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);


/*-----------------------------------------------------------------------------*
 *  Calculate the Matrix
 *-----------------------------------------------------------------------------*/
static void Matrix_Multiply(const matrix *m, const matrix *n, matrix *r)
{
	unsigned int i, j, k;

	memset(r, 0x0, sizeof(matrix));
	r->size = m->size;

	for (i = 0; i < m->size; ++i) {
		for (j = 0; j < n->size; ++j) {
			for (k = 0; k < r->size; ++k) {
				r->elements[i][j] +=
					m->elements[i][k] * n->elements[k][j];
			}
		}
	}
}

/*-----------------------------------------------------------------------------*
 *  RPMSG callbacks setup by remoteproc_resource_init()
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	matrix matrix_array[NUM_MATRIX];
	matrix matrix_result;

	(void)priv;
	(void)src;

	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		LPRINTF("shutdown message is received.\n");
		rpmsg_destroy_ept(&lept);
		return RPMSG_EPT_CB_HANDLED;
	}

	memcpy(matrix_array, data, len);
	/* Process received data and multiple matrices. */
	Matrix_Multiply(&matrix_array[0], &matrix_array[1], &matrix_result);

	/* Send the result of matrix multiplication back to master. */
	if (rpmsg_send(ept, &matrix_result, sizeof(matrix)) < 0) {
		LPERROR("rpmsg_send failed\n");
	}
	return RPMSG_EPT_CB_HANDLED;
}

static void rpmsg_endpoint_destroy(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPERROR("Endpoint is destroyed\n");
	ept_deleted = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret;

	ret = rpmsg_create_ept(&lept, rdev, RPMSG_CHAN_NAME, 0, RPMSG_ADDR_ANY,
			       rpmsg_endpoint_cb, rpmsg_endpoint_destroy);
	if (ret) {
		LPERROR("Failed to create endpoint.\n");
		return -1;
	}

	LPRINTF("Waiting for events...\n");
	while(1) {
		platform_poll(priv);
		/* we got a shutdown request, exit */
		if (ept_deleted) {
			break;
		}
	}

	return 0;
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
	struct remoteproc *rproc;
	struct rpmsg_device *rdev;
	int ret;

	LPRINTF("Starting application...\n");

	/* Initialize HW system components */
	init_system();

	if (argc >= 2) {
		proc_id = strtoul(argv[1], NULL, 0);
	}

	if (argc >= 3) {
		rsc_id = strtoul(argv[2], NULL, 0);
	}

	rproc = platform_create_proc(proc_id, rsc_id);
	if (!rproc) {
		LPERROR("Failed to create remoteproc device.\n");
		ret = -1;
	} else {
		rdev = platform_create_rpmsg_vdev(rproc, 0,
						   VIRTIO_DEV_SLAVE,
						   NULL, NULL);
		if (!rdev) {
			LPERROR("Failed to create rpmsg virtio device.\n");
			ret = -1;
		} else {
			app(rdev, (void *)rproc);
			ret = 0;
		}
	}

	LPRINTF("Stopping application...\n");
	remoteproc_remove(rproc);
	cleanup_system();

	return ret;
}
