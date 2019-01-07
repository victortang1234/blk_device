/* 
 *
 * A simple block device driver
 * Date: 20190107
 * Author:
 * 			-Victor Tang
 *
 */

struct blk_dev {
	struct list_head entry;
	char name[];

	struct request_queue queue;
	struct block_device device;
	struct gendisk *disk;
	int major;
	sector_t size; /*device size in Bytes*/
};

static LIST_HEAD(sys_dev_list); /* device in system are stored here */
static DEFINE_SPINLOCK(dev_list_lock); /* protect sys_dev_list */

static struct blk_dev *check_dev_exist(const char *dev_name)
{
	struct blk_dev *dev = NULL;

	spin_lock(&dev_list_lock);
	list_for_each_entry(dev, sys_dev_list, entry) {
		if (strcmp(dev->name, dev_name) == 0) {
			goto OUT;
		}
	}
	dev = NULL;

OUT:
	spin_unlock(&dev_list_lock);
	return dev;
}

int dev_create(const char *dev_name, const char *phy_dev)
{
	struct blk_dev *dev;
	int ret = 0;

	dev = check_dev_exist();
	if (dev) {
		printk("device has already existed.\n");
		goto OUT;
	}

OUT:
	return ret;
}
