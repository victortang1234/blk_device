/*
 *
 * IOctl sample code
 *
 * Author:
 * 			-Group 3
 */

#include <linux/module.h>
#include <linux/miscdevice.h>

#include <linux/fs.h>
#include <linux/uaccess.h>

#include "include/blkdev_ioctl.h"

static int ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long user)
{
	struct delete_param del_param;
	struct create_param ctr_param;
	unsigned long test_arg;
	int ret = 0;

	if (_IOC_TYPE(cmd) != IOCTL_CODE) {
		printk("Unkown ioctl command 0x%x\n", cmd);
		ret = -ENOTTY;
		goto OUT;
	}

	switch(cmd) {
		case TEST_CMD:
			ret = copy_from_user(&test_arg, (void *)user, sizeof(unsigned long));
			if (ret) {
				printk("copy from user failed.\n");
			} else {
				printk("received IOctl[arg=%lu] from user.\n", test_arg);
			}
			break;
		case CREATE_CMD:
			ret = copy_from_user(&ctr_param, (void *)user, sizeof(struct create_param));
			if (ret) {
				printk("copy from user failed.\n");
			} else {
				;//
			}
			break;
		case DELETE_CMD:
			ret = copy_from_user(&del_param, (void *)user, sizeof(struct delete_param));
			if (ret) {
				printk("copy from user failed.\n");
			} else {
				;//ret = dev_delete(del_param.name);
			}
			break;
		default:
			printk("Unkown ioctl command[0x%x].\n", cmd);
			ret = -ENOTTY;
			break;
	}

OUT:
	return ret;
}

static struct file_operations _misc_ctl_fops = {
	.compat_ioctl = ioctl,
	.owner = THIS_MODULE,
};

static struct miscdevice _misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MISC_NAME,
	.fops = &_misc_ctl_fops
};

static int __init ioctl_init(void)
{
	int ret;

	ret = misc_register(&_misc_dev);
	if (ret) {
		printk("Register ioctl device[%s] failed\n", _misc_dev.name);
	}

	return ret;
}

static void __exit ioctl_exit(void)
{
#if 0
	if (misc_deregister(&_misc_dev) < 0) {
		printk("Deregister ioctl device[%s],failed\n", _misc_dev.name);
	}
#else
	misc_deregister(&_misc_dev);
#endif

	return;
}

module_init(ioctl_init);
module_exit(ioctl_exit);
MODULE_LICENSE("GPL");
