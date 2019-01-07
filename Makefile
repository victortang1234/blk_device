###########################################
#
#Makefile for build blkdev.ko
#
##########################################

obj-m := blkdev.o
blkdev-y += blkdev_ioctl.o
#blkdev-y += blkdev_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(MAKE) -C tool

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(MAKE) -C tool clean
	rm -rf Module.markers modules.order Modules.symvers user
