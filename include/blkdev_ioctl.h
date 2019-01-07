/* 
 * A simple block device driver use for create, display, delete device driver.
 * Record the time in this device driver and lower device driver.
 * Save the bio and delay submit the bio.
 *
 * Author:
 * 			-Group 3
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

/*
 * define ioctl coded for interface between kernel_module and user program
 *
 */
#define IOCTL_CODE 			0xcc

#define TEST_CMD		_IOWR (IOCTL_CODE,  0x09, unsigned long)
#define CREATE_CMD			_IOWR (IOCTL_CODE, 0x0A, struct create_param)
#define GET_NUM_CMD 		_IOWR (IOCTL_CODE, 0x0E, unsigned long)
#define DELETE_CMD		_IOWR (IOCTL_CODE,  0x0B, struct delete_param)

#endif
