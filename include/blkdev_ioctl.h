/* 
 * A simple block device driver use for create, display, delete device driver.
 * Record the time in this device driver and lower device driver.
 * Save the bio and delay submit the bio.
 *
 * Date: 20190107
 * Author:
 * 			-Victor Tang
 */

#ifndef _IOCTL_H_
#define _IOCTL_H_

#define print_star printf("*************************************************\n")

#define MISC_NAME "ioctl_test"

#define DEV_NAME_LEN		32
#define DEV_PATH_LEN		32
#define NAME_LEN_BUF		37
#define MAX_DEVICE 			1024
#define HELP				-2

struct create_param {
	char dev_name[DEV_NAME_LEN];
	char dev_path[DEV_PATH_LEN];
};

struct delete_param {
	char name[16];
};

//defined in ioctl.h
/* used to create numbers */
/*
#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOR_BAD(type,nr,size)	_IOC(_IOC_READ,(type),(nr),sizeof(size))
#define _IOW_BAD(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOWR_BAD(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),sizeof(size))
*/
/* used to decode ioctl numbers.. */
/*

#define _IOC_TYPE(nr)		(((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
ps:
driver ioctl call _IOC_TYPE(cmd) to check if cmd is valid ioctl command

*/
//end of defined in ioctl.h

/*
 * define ioctl coded for interface between kernel_module and user program
 *
 */
#define IOCTL_CODE 			0xcc

#define TEST_CMD			_IOWR (IOCTL_CODE, 0x09, unsigned long)
#define CREATE_CMD			_IOWR (IOCTL_CODE, 0x0A, struct create_param)
#define GET_NUM_CMD 		_IOWR (IOCTL_CODE, 0x0E, unsigned long)
#define DELETE_CMD			_IOWR (IOCTL_CODE, 0x0B, struct delete_param)

#endif
