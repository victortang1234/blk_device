/* 
 *
 * A simple block device driver
 * Date: 20190107
 * Author:
 * 			-Victor Tang
 *
 */
#include <linux/list.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/spinlock_types.h>
#include <linux/errno.h>

//#define DISK_NAME_LEN		16
#define DISK_INIT_MAJOR		0
#define DISK_INIT_MINOR		0
#define SECTOR_BITS			(9)

struct blk_dev {
	struct list_head entry;
	char name[DISK_NAME_LEN];

	struct request_queue *queue;
	struct block_device *device;
	struct gendisk *disk;
	int major;
	sector_t size; /*device size in Bytes*/
};

static LIST_HEAD(sys_dev_list); /* device in system are stored here */
static DEFINE_SPINLOCK(dev_list_lock); /* protect sys_dev_list */

static int dev_open(struct block_device *bdev, fmode_t mode)
{
	/* who opened me? */
	//printk("device opened by:[%s]\n", current->comm);
	return 0;
}

static void dev_close(struct gendisk *g, fmode_t mode)
{
	/* who closed me? */
	//printk("device closed by:[%s]\n", current->comm);
	return;
}

#if 0
static int dev_ioctl(struct block_device *bdev, fmode_t mode, unsigned cmd, unsigned long arg)
{
	return 0;
}
#endif

static struct block_device_operations disk_fops = {
	.open = dev_open,
	.release =  dev_close,
//	.unlocked = dev_ioctl,
	.owner = THIS_MODULE,
};

static void make_request(struct request_queue *q, struct bio *bio)
{
	struct blk_dev *dev = q->queuedata;

	bio->bi_bdev = dev->device;
	submit_bio(bio_rw(bio), bio);

	//return 0;
}

static struct blk_dev *check_dev_exist(const char *dev_name)
{
	struct blk_dev *dev = NULL;

	spin_lock(&dev_list_lock);
	list_for_each_entry(dev, &sys_dev_list, entry) {
		if (strcmp(dev->name, dev_name) == 0) {
			goto OUT;
		}
	}
	dev = NULL;

OUT:
	spin_unlock(&dev_list_lock);
	return dev;
}

static void dev_add(struct blk_dev *dev)
{
	spin_lock(&dev_list_lock);
	list_add(&dev->entry, &sys_dev_list);
	spin_unlock(&dev_list_lock);
}

static void dev_del(struct blk_dev *dev)
{
	spin_lock(&dev_list_lock);
	//list_del(&dev->entry, &sys_dev_list);
	spin_unlock(&dev_list_lock);
}

int dev_delete(const char *dev_name)
{
	struct blk_dev *dev;
	int ret = 0;

	dev = check_dev_exist(dev_name);
	if (!dev) {
		ret = -EINVAL;
		goto OUT;
	}
	dev_del(dev);

	//close_bdev_excl(dev->device);
	del_gendisk(dev->disk);
	put_disk(dev->disk);

	unregister_blkdev(dev->major, dev->name);
	blk_cleanup_queue(dev->queue);
	kfree(dev);

OUT:
	return ret;
}

/*
 * work flow:
 * 1. define device struct
 * 2. alloc major
 * 3. alloc gendisk
 * 4. alloc request queue
 * 5. initialize request queue
 * 6. initialize gendisk struct
 * 7. add gendisk into system
 * 8. now, u can check device in /dev/
 */
int dev_create(const char *dev_name, const char *phy_dev)
{
	struct blk_dev *dev;
	int ret = 0;

	dev = check_dev_exist(dev_name);
	if (dev) {
		printk("device has already existed.\n");
		goto OUT;
	}

	dev = kzalloc(sizeof(struct blk_dev), GFP_KERNEL);
	if (!dev) {
		printk("alloc mem dev fail\n");
		ret = -ENOMEM;
		goto OUT;
	}
	strncpy(dev->name, dev_name, DISK_NAME_LEN);

	/*register a new block device*/
	/*@major: the requested major device number [1..255].
	 *If major=0, try to allocate any unused major number.*/
	/*@name: the name of the new block device as a zero terminated string.
	 * The name of the new block device must be unique within the system.*/
	dev->major = register_blkdev(DISK_INIT_MAJOR, dev_name);
	if (dev->major < 0) {
		printk("get major fail");
		ret = -EIO;
		goto ERR_OUT0;
	}

	/* alloc gendisk */
	dev->disk = alloc_disk(1);
	if (!dev->disk) {
		printk("alloc disk error");
		ret = -ENOMEM;
		goto ERR_OUT1;
	}

	/* alloc request queue */
	dev->queue = blk_alloc_queue(GFP_KERNEL);
	if (!dev->queue) {
		printk("blk alloc queue error");
		ret =  -ENOMEM;
		goto ERR_OUT2;
	}

	/* initialize request queue */
	blk_queue_make_request(dev->queue, make_request);

	/* init gendisk */
	dev->disk->queue = dev->queue;
	dev->disk->queue->queuedata = dev;

	strncpy(dev->disk->disk_name, dev_name, DISK_NAME_LEN);
	dev->disk->major = dev->major;
	dev->disk->first_minor = DISK_INIT_MINOR;
	dev->disk->fops = &disk_fops;
	dev->disk->private_data = dev;

	//dev->device = open_bdev_excl(phy_dev, FMODE_WRITE | FMODE_READ,
	//		dev->device);

	if (IS_ERR(dev->device)) {
		printk("Open the device %s fail", phy_dev);
		ret = -ENOENT;
		goto OUT;
	}

	dev->size = get_capacity(dev->device->bd_disk) < SECTOR_BITS;
	set_capacity(dev->disk, (dev->size >> SECTOR_BITS));

	/* add gendisk into system */
	add_disk(dev->disk);
	dev_add(dev);

	/* now, u can check device in /dev/ */
	return 0;

ERR_OUT2:
	put_disk(dev->disk);
ERR_OUT1:
	unregister_blkdev(dev->major, dev->name);
ERR_OUT0:
	kfree(dev);
OUT:
	return ret;
}
