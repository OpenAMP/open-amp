/*
 * RPMSG Echo Test Kernel Driver
 *
 * Copyright (C) 2014 Mentor Graphics Corporation
 *
 * Test application that validates data integraty of inter processor
 * communication from linux userspace to a remote software
 * context. The application sends chunks of data to the
 * remote processor. The remote side echoes the data back
 * to application which then validates the data returned.
 *
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
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int flag;

struct _payload {
unsigned long	num;
unsigned long	size;
char		data[];
};

#define MAX_RPMSG_BUFF_SIZE 512
#define PAYLOAD_MIN_SIZE	1
#define PAYLOAD_MAX_SIZE	(MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS		(PAYLOAD_MAX_SIZE/PAYLOAD_MIN_SIZE)
/* Shutdown message ID */
#define SHUTDOWN_MSG		0xEF56A55A

#define GIGA			1000000000L

static const char * const shutdown_argv[]
			= { "/sbin/shutdown", "-h", "-P", "now", NULL };

struct rpmsg_endpoint *ept;

struct _payload *p_payload;

static const char init_msg[] = "init_msg";
int err_cnt;

static void rpmsg_echo_test_kern_app_rx_cb(struct rpmsg_channel *rpdev,
			void *data, int len, void *priv, u32 src)
{
	struct _payload *local_payload = data;
	int	i;

	if (p_payload) {

		pr_err("\r\n Master : Linux Kernal Space : Received payload ");
		pr_err("num %d of size %d \r\n", local_payload->num, len);

		/* Shutdown Linux if such a message is received.
		Only applicable when Linux is a remoteproc remote. */
		if ((*(int *) data) == SHUTDOWN_MSG) {
			call_usermodehelper(shutdown_argv[0], shutdown_argv,
						NULL, UMH_NO_WAIT);
		} else {

			for (i = 0; i < local_payload->size; i++) {

				if (local_payload->data[i] != 0xA5) {
					pr_err("\r\n Data corruption ");
					pr_cont("at index %d \r\n", i);
					err_cnt++;
					break;
				}
			}

			/* Free memory allocated to payload buffer. */
			kzfree(p_payload);

			/* Wake up the application. */
			flag = 1;
			wake_up_interruptible(&wait_queue);
		}
	}
}

static int rpmsg_echo_test_kern_app_probe(struct rpmsg_channel *rpdev)
{
	int	err, i, size;
	int shutdown_msg = SHUTDOWN_MSG;

	pr_err("\r\n Echo Test Start! \r\n");

	/* Create endpoint for remote channel and register rx callabck */
	ept = rpmsg_create_ept(rpdev, rpmsg_echo_test_kern_app_rx_cb, 0,
				RPMSG_ADDR_ANY);

	if (!ept) {
		pr_err(" Endpoint creation for failed!\r\n");
		return -ENOMEM;
	}

	/* Send init message to complete the connection loop */
	err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
				init_msg, sizeof(init_msg));

	if (err) {
		pr_err(" Init message send failed!\r\n");
		return err;
	}

	for (i = 0, size = PAYLOAD_MIN_SIZE; i < NUM_PAYLOADS; i++, size++) {
		p_payload = kzalloc(2 * sizeof(unsigned long) + size,
					GFP_KERNEL);
		p_payload->num = i;
		p_payload->size = size;
		memset(&(p_payload->data[0]), 0xA5, size);

		pr_err("\r\n Master : Linux Kernal Space : Sending payload num %d of size %d \r\n",
			p_payload->num, (2 * sizeof(unsigned long)) + size);

		/* Send payload to remote. */
		err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
				p_payload, (2 * sizeof(unsigned long)) + size);

		if (err) {
			pr_err(" send failed!\r\n");
			return err;
		}

		/* Wait till the data is echoed back. */
		wait_event_interruptible(wait_queue, flag != 0);
		flag = 0;
	}

	/* Send payload to remote. */
	err = rpmsg_send_offchannel(rpdev, ept->addr, rpdev->dst,
				&shutdown_msg, sizeof(int));

	if (err) {
		pr_err(" Shutdown message send failed!\r\n");
		return err;
	}

	pr_err("\r\n *******************************************\r\n");
	pr_err("\r\n Echo Test Results: Error count = %d\r\n", err_cnt);
	pr_err("\r\n *******************************************\r\n");

	return 0;
}

static void rpmsg_echo_test_kern_app_remove(struct rpmsg_channel *rpdev)
{
	rpmsg_destroy_ept(ept);
}

static void rpmsg_cb(struct rpmsg_channel *rpdev, void *data,
					int len, void *priv, u32 src)
{

}

static struct rpmsg_device_id rpmsg_echo_test_kern_app_id_table[] = {
	{ .name	= "rpmsg-openamp-demo-channel" },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_echo_test_kern_app_id_table);

static struct rpmsg_driver rpmsg_echo_test_kern_app = {
	.drv.name	= KBUILD_MODNAME,
	.drv.owner   = THIS_MODULE,
	.id_table	= rpmsg_echo_test_kern_app_id_table,
	.probe	   = rpmsg_echo_test_kern_app_probe,
	.callback	= rpmsg_cb,
	.remove	  = rpmsg_echo_test_kern_app_remove,
};

static int __init init(void)
{
	return register_rpmsg_driver(&rpmsg_echo_test_kern_app);
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_echo_test_kern_app);
}

module_init(init);
module_exit(fini);

MODULE_DESCRIPTION("rpmsg echo test kernel application");
MODULE_LICENSE("GPL v2");
