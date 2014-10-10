/*
 * RPMSG User Device Kernel Driver
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
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/ioctl.h>
#include <linux/errno.h>

static struct class *rpmsg_class;
static dev_t rpmsg_dev;

struct _rpmsg_dev_params {
	int rpmsg_major;
	int rpmsg_minor;
	struct device *rpmsg_dev;
	struct cdev cdev;
	struct rpmsg_channel *rpmsg_chnl;
};

struct _rpmsg_dev_instance {
	struct rpmsg_endpoint *ept;
	struct _rpmsg_dev_params *dev_params;
	wait_queue_head_t usr_wait_q;
	struct mutex sync_lock;
};

#define MAX_RPMSG_BUFF_SIZE		512
#define RPMSG_KFIFO_SIZE		(MAX_RPMSG_BUFF_SIZE * 4)

#define IOCTL_CMD_GET_KFIFO_SIZE	1
#define IOCTL_CMD_GET_AVAIL_DATA_SIZE	2
#define IOCTL_CMD_GET_FREE_BUFF_SIZE	3

/* Shutdown message ID */
#define SHUTDOWN_MSG			0xEF56A55A

static struct kfifo rpmsg_kfifo;
static int block_flag ;
static const char init_msg[] = "init_msg";

static const char *const shutdown_argv[]
		= { "/sbin/shutdown", "-h", "-P", "now", NULL };

/* This callback gets invoked when we received data from the remote node */
static void rpmsg_user_dev_drv_rx_cb(struct rpmsg_channel *rpdev,
			void *data, int len, void *priv, u32 src)
{
	struct _rpmsg_dev_instance *rpmsg_dev_instance = priv;

	/* Shutdown Linux if such a message is received. Only applicable
	when Linux is a remoteproc remote. */
	if ((*(int *) data) == SHUTDOWN_MSG) {
		call_usermodehelper(shutdown_argv[0], shutdown_argv,
					NULL, UMH_NO_WAIT);
	} else {
		/* Push data received into rpmsg kfifo */
		mutex_lock_interruptible(&rpmsg_dev_instance->sync_lock);
		kfifo_in(&rpmsg_kfifo, data, (unsigned int)len);
		mutex_unlock(&rpmsg_dev_instance->sync_lock);

		/* Wake up any blocking contexts waiting for data */
		block_flag = 1;
		wake_up_interruptible(&rpmsg_dev_instance->usr_wait_q);
	}
}


static int rpmsg_dev_open(struct inode *inode, struct file *p_file)
{
	struct _rpmsg_dev_instance *rpmsg_dev_instance;
	struct _rpmsg_dev_params *rpmsg_dev_params;
	int retval;

	/* Create and initialize rpmsg device instance */
	rpmsg_dev_instance = kzalloc(sizeof(struct _rpmsg_dev_instance),
					GFP_KERNEL);

	if (!rpmsg_dev_instance) {
		pr_err("\r\n cannot allocate memory for rpmsg device instance \r\n");
		return -ENOMEM;
	}

	/* Initialize rpmsg instance with device params from inode */
	rpmsg_dev_instance->dev_params = container_of(inode->i_cdev,
					struct _rpmsg_dev_params, cdev);

	rpmsg_dev_params = rpmsg_dev_instance->dev_params;

	/* Allocate kfifo for rpmsg */
	retval = kfifo_alloc(&rpmsg_kfifo, RPMSG_KFIFO_SIZE, GFP_KERNEL);
	kfifo_reset(&rpmsg_kfifo);

	if (retval) {
		pr_err("\r\n error in kfifo_alloc for rpmsg \r\n");

		return retval;
	}

	/* Initialize mutex */
	mutex_init(&rpmsg_dev_instance->sync_lock);

	/* Initialize wait queue head that provides blocking rx for userspace */
	init_waitqueue_head(&rpmsg_dev_instance->usr_wait_q);

	p_file->private_data = rpmsg_dev_instance;

	/* Create endpoint for remote channel and register rx callabck */
	rpmsg_dev_instance->ept = rpmsg_create_ept(rpmsg_dev_params->rpmsg_chnl,
						rpmsg_user_dev_drv_rx_cb,
						rpmsg_dev_instance,
						RPMSG_ADDR_ANY);


	if (!rpmsg_dev_instance->ept) {
		pr_err(" Endpoint creation for failed!\r\n");
		return -ENOMEM;
	}

	/* Send init message to remote to complete the connection stage.
	The message carries the address of new endpoint so that the
	further messages from remote are received in its call back */
	retval = rpmsg_send_offchannel(rpmsg_dev_params->rpmsg_chnl,
					rpmsg_dev_instance->ept->addr,
					rpmsg_dev_params->rpmsg_chnl->dst,
					init_msg, sizeof(init_msg));

	if (retval) {
		pr_err(" Init message send failed!\r\n");
		return retval;
	}

	return 0;
}

static ssize_t rpmsg_dev_write(struct file *p_file,
				const char __user *ubuff, size_t len,
				loff_t *p_off)
{
	struct _rpmsg_dev_instance *rpmsg_dev_instance = p_file->private_data;
	struct _rpmsg_dev_params *rpmsg_dev_params
					= rpmsg_dev_instance->dev_params;
	int err;
	unsigned int size;
	char *tmp_buff;

	if (len < MAX_RPMSG_BUFF_SIZE)
		size = len;
	else
		size = MAX_RPMSG_BUFF_SIZE;

	tmp_buff = kzalloc(size, GFP_KERNEL);

	if (copy_from_user(tmp_buff, ubuff, size)) {
		pr_err("\r\n user to kernel buff copy error \r\n");
		return -1;
	}

	err = rpmsg_send_offchannel(rpmsg_dev_params->rpmsg_chnl,
					rpmsg_dev_instance->ept->addr,
					rpmsg_dev_params->rpmsg_chnl->dst,
					tmp_buff, size);

	if (err) {
		size = 0;
		pr_err("\r\n rpmsg_send_off_channel error \r\n");
	}

	return size;
}

static ssize_t rpmsg_dev_read(struct file *p_file, char __user *ubuff,
				size_t len, loff_t *p_off)
{
	struct _rpmsg_dev_instance *rpmsg_dev_instance = p_file->private_data;
	int retval;
	unsigned int data_available, data_used, bytes_copied;

	/* Acquire lock to access rpmsg kfifo */
	if (mutex_lock_interruptible(&rpmsg_dev_instance->sync_lock))
		return -ERESTARTSYS;

	data_available = kfifo_len(&rpmsg_kfifo);

	if (data_available ==  0) {
		/* Release lock */
		mutex_unlock(&rpmsg_dev_instance->sync_lock);

		/* if non-blocking read is requested return error */
		if (p_file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		/* Block the calling context till data becomes available */
		wait_event_interruptible(rpmsg_dev_instance->usr_wait_q,
					block_flag != 0);
	}

	/* reset block flag */
	block_flag = 0;

	/* Provide requested data size to user space */
	data_available = kfifo_len(&rpmsg_kfifo);
	data_used = (data_available > len) ? len : data_available;
	retval = kfifo_to_user(&rpmsg_kfifo, ubuff, data_used, &bytes_copied);

	/* Release lock on rpmsg kfifo */
	mutex_unlock(&rpmsg_dev_instance->sync_lock);

	return retval ? retval : bytes_copied;
}

static long rpmsg_dev_ioctl(struct file *p_file, unsigned int cmd,
				unsigned long arg)
{
	unsigned int tmp;

	switch (cmd) {
	case IOCTL_CMD_GET_KFIFO_SIZE:
		tmp = kfifo_size(&rpmsg_kfifo);
		if (copy_to_user((unsigned int *)arg, &tmp, sizeof(int)))
			return -EACCES;
		break;

	case IOCTL_CMD_GET_AVAIL_DATA_SIZE:
		tmp = kfifo_len(&rpmsg_kfifo);
		pr_err("kfifo len ioctl = %d ", kfifo_len(&rpmsg_kfifo));
		if (copy_to_user((unsigned int *)arg, &tmp, sizeof(int)))
			return -EACCES;
		break;
	case IOCTL_CMD_GET_FREE_BUFF_SIZE:
		tmp = kfifo_avail(&rpmsg_kfifo);
		if (copy_to_user((unsigned int *)arg, &tmp, sizeof(int)))
			return -EACCES;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int rpmsg_dev_release(struct inode *inode, struct file *p_file)
{
	struct _rpmsg_dev_instance *rpmsg_dev_instance = p_file->private_data;

	rpmsg_destroy_ept(rpmsg_dev_instance->ept);
	kfree(rpmsg_dev_instance);

	return 0;
}

static const struct file_operations rpmsg_dev_fops = {
	.owner = THIS_MODULE,
	.read = rpmsg_dev_read,
	.write = rpmsg_dev_write,
	.open = rpmsg_dev_open,
	.unlocked_ioctl = rpmsg_dev_ioctl,
	.release = rpmsg_dev_release,
};

static int rpmsg_user_dev_drv_probe(struct rpmsg_channel *rpdev)
{
	struct _rpmsg_dev_params *rpmsg_dev_params;

	/* Allocate memory for the rpmsg device parameters data structure */
	rpmsg_dev_params = kzalloc(sizeof(struct _rpmsg_dev_params),
					GFP_KERNEL);

	if (!rpmsg_dev_params) {
		pr_err("\r\n cannot allocate memory for rpmsg device params \r\n");
		return -ENOMEM;
	}

	/* Initialize rpmsg device parameters data structure */
	rpmsg_dev_params->rpmsg_major = MAJOR(rpmsg_dev);
	rpmsg_dev_params->rpmsg_minor = 0;
	/* Create device */
	rpmsg_dev_params->rpmsg_dev = device_create(rpmsg_class, &rpdev->dev,
						rpmsg_dev, NULL, "rpmsg");
	if (rpmsg_dev_params->rpmsg_dev == NULL) {
		class_destroy(rpmsg_class);
		unregister_chrdev_region(rpmsg_dev, 1);
		return -1;
	}
	/* Initialize character device */
	cdev_init(&rpmsg_dev_params->cdev, &rpmsg_dev_fops);
	rpmsg_dev_params->cdev.owner = THIS_MODULE;
	if (cdev_add(&rpmsg_dev_params->cdev, rpmsg_dev, 1) == -1) {
		device_destroy(rpmsg_class, rpmsg_dev);
		class_destroy(rpmsg_class);
		unregister_chrdev_region(rpmsg_dev, 1);
		return -1;
	}
	rpmsg_dev_params->rpmsg_chnl = rpdev;

	dev_set_drvdata(&rpdev->dev, rpmsg_dev_params);

	dev_info(&rpdev->dev, "new channel: 0x%x -> 0x%x!\n",
			rpdev->src, rpdev->dst);

	return 0;
}

static void rpmsg_user_dev_drv_remove(struct rpmsg_channel *rpdev)
{
	struct _rpmsg_dev_params *rpmsg_dev_params
					= dev_get_drvdata(&rpdev->dev);

	device_destroy(rpmsg_class, rpmsg_dev);
	cdev_del(&rpmsg_dev_params->cdev);
	kfree(rpmsg_dev_params);
}

static void rpmsg_cb(struct rpmsg_channel *rpdev, void *data,
					int len, void *priv, u32 src)
{

}

static struct rpmsg_device_id rpmsg_user_dev_drv_id_table[] = {
	{ .name	= "rpmsg-openamp-demo-channel" },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_user_dev_drv_id_table);

static struct rpmsg_driver rpmsg_user_dev_drv = {
	.drv.name	= KBUILD_MODNAME,
	.drv.owner	= THIS_MODULE,
	.id_table	= rpmsg_user_dev_drv_id_table,
	.probe		= rpmsg_user_dev_drv_probe,
	.callback	= rpmsg_cb,
	.remove	= rpmsg_user_dev_drv_remove,
};

static int __init init(void)
{
	/* Allocate char device for this rpmsg driver */
	if (alloc_chrdev_region(&rpmsg_dev, 0, 1, KBUILD_MODNAME) < 0) {
		pr_err("\r\n Error allocating char device \r\n");
		return -1;
	}

	/* Create device class for this device */
	rpmsg_class = class_create(THIS_MODULE, KBUILD_MODNAME);

	if (rpmsg_class == NULL) {
		unregister_chrdev_region(rpmsg_dev, 1);
		pr_err("\r\n Error allocating char device \r\n");
		return -1;
	}

	/* Register this rpmsg driver with the rpmsg bus layer */
	return register_rpmsg_driver(&rpmsg_user_dev_drv);
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_user_dev_drv);
	class_destroy(rpmsg_class);
	unregister_chrdev_region(rpmsg_dev, 1);
}

module_init(init);
module_exit(fini);

MODULE_DESCRIPTION("Sample driver to exposes rpmsg svcs to userspace via a char device");
MODULE_LICENSE("GPL v2");
