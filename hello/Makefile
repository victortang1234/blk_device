###########################################
#
#Makefile for build blkdev.ko
#
##########################################

#disable "module verification failed:
#	signature and/or	required key missing - tainting kernel"
CONFIG_MODULE_SIG=n

obj-m := hello.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf Module.markers modules.order Modules.symvers user hello.ko
