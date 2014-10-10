/*
 * RPMSG Matrix Multiplication Kernel Driver
 *
 * Copyright (C) 2014 Mentor Graphics Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>
#include <linux/slab.h>
#include <linux/random.h>

/* Application */
#define	MAX_SIZE			6
#define NUM_MATRIX			2
/* Shutdown message ID */
#define SHUTDOWN_MSG			0xEF56A55A

struct rpmsg_endpoint *ept;
static const char init_msg[] = "init_msg";

static const char *const shutdown_argv[]
		= { "/sbin/shutdown", "-h", "-P", "now", NULL };

struct _matrix {
	unsigned long size;
	unsigned long elements[MAX_SIZE][MAX_SIZE];
};

static struct	_matrix *p_matrix;

static void matrix_print(struct _matrix *m)
{
	int i, j;

	/* Generate two random matrices */
	pr_err(" \r\n Master : Linux : Printing results \r\n");

	for (i = 0; i < m->size; ++i) {
		for (j = 0; j < m->size; ++j)
			pr_cont(" %d ", (unsigned int)m->elements[i][j]);
		pr_info("\r\n");
	}
}

static void generate_matrices(int num_matrices, unsigned int matrix_size,
				void *p_data)
{
	int	i, j, k, val;
	struct _matrix *p_matrix = p_data;

	/* Generate two random matrices */
	pr_err(" \r\n Master : Linux : Generating random matrices \r\n");

	for (i = 0; i < num_matrices; i++) {

		/* Initialize workload */
		p_matrix[i].size = matrix_size;

		pr_err(" \r\n Master : Linux : Input matrix %d \r\n", i);
		for (j = 0; j < matrix_size; j++) {

			pr_info("\r\n");
			for (k = 0; k < matrix_size; k++) {
				get_random_bytes(&val, sizeof(val));
				p_matrix[i].elements[j][k] =
						((val & 0x7F) % 10);
				pr_cont(" %d ",
				(unsigned int)p_matrix[i].elements[j][k]);
			}
		}
		pr_err("\r\n");
	}

}

static void rpmsg_mat_mul_kern_app_rx_cb(struct rpmsg_channel *rpdev,
			void *data, int len, void *priv, u32 src)
{
	int err;
	int shutdown_msg = SHUTDOWN_MSG;

	pr_err(" \r\n Master : Linux : Received %d bytes of data", len);
	pr_err(" over rpmsg channel from remote \r\n");

	/* Shutdown Linux if such a message is received. Only applicable
		when Linux is a remoteproc remote. */
	if ((*(int *) data) == SHUTDOWN_MSG) {
		call_usermodehelper(shutdown_argv[0], shutdown_argv,
					NULL, UMH_NO_WAIT);
	} else {
		/* print results */
		matrix_print((struct _matrix *)data);

		/* Send payload to remote. */
		err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
					&shutdown_msg, sizeof(int));

		if (err)
			pr_err(" Shutdown send failed!\r\n");

		kzfree(p_matrix);
	}
}

static int rpmsg_mat_mul_kern_app_probe(struct rpmsg_channel *rpdev)
{
	int	err;

	pr_err("\r\n Demo Start - Demo rpmsg driver got probed \r\n");
	pr_err("since the rpmsg device associated with driver was found !\r\n");

	pr_err("\r\n Create endpoint and register rx callback \r\n");
	/* Create endpoint for remote channel and register rx callabck */
	ept = rpmsg_create_ept(rpdev, rpmsg_mat_mul_kern_app_rx_cb, 0,
				RPMSG_ADDR_ANY);

	if (!ept) {
		pr_err(" Endpoint creation for failed!\r\n");
		return -ENOMEM;
	}

	/* Send init message to complete the connection loop */
	err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
				init_msg, sizeof(init_msg));

	if (err) {
		pr_err(" Init messages send failed!\r\n");
		return err;
	}

	/* Allocate memory for random matrices */
	p_matrix = kzalloc(sizeof(struct _matrix)*2, GFP_KERNEL);

	/* Generate random matrices */
	generate_matrices(NUM_MATRIX, MAX_SIZE, p_matrix);

	/* Send matrices to remote for computation */
	err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
				p_matrix, sizeof(struct _matrix) * 2);

	pr_err("\r\n Master : Linux : Sent %d bytes of data over rpmsg channel to remote \r\n",
		sizeof(struct _matrix) * 2);

	if (err) {
		pr_err(" send failed!\r\n");
		return err;
	}

	return 0;
}

static void rpmsg_mat_mul_kern_app_remove(struct rpmsg_channel *rpdev)
{
	rpmsg_destroy_ept(ept);
}

static void rpmsg_cb(struct rpmsg_channel *rpdev, void *data,
					int len, void *priv, u32 src)
{

}

static struct rpmsg_device_id rpmsg_mat_mul_kern_app_id_table[] = {
	{ .name	= "rpmsg-openamp-demo-channel" },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_mat_mul_kern_app_id_table);

static struct rpmsg_driver rpmsg_mat_mul_kern_app = {
	.drv.name	= KBUILD_MODNAME,
	.drv.owner	= THIS_MODULE,
	.id_table	= rpmsg_mat_mul_kern_app_id_table,
	.probe		= rpmsg_mat_mul_kern_app_probe,
	.callback	= rpmsg_cb,
	.remove	= rpmsg_mat_mul_kern_app_remove,
};

static int __init init(void)
{
	return register_rpmsg_driver(&rpmsg_mat_mul_kern_app);
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_mat_mul_kern_app);
}

module_init(init);
module_exit(fini);

MODULE_DESCRIPTION("Sample rpmsg matrix multiplication kernel application");
MODULE_LICENSE("GPL v2");
