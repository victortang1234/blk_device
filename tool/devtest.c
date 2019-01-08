/*
 *
 * IOctl sample code
 * Author:
 * 			-Victor Tang
 */
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "../include/blkdev_ioctl.h"

int main()
{
	unsigned long arg = 123456;
	int fd;

	fd = open("/dev/"MISC_NAME, O_RDWR, 0);
	if (fd < 0) {
		printf("Failed to open /dev/%s, %d\n", MISC_NAME, errno);
		goto OUT;
	}

	if (ioctl(fd, TEST_CMD, &arg)) {
		printf("Failed to execute ioctl.\n");
	}

	close(fd);

OUT:
	return 0;
}
