/*
 *
 * Name: devcreate
 * Date: 20190107
 * Author:
 * 		- Victor Tang
 *
 */
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
//#include <ctype.h>
#include <string.h>
#include "../include/blkdev_ioctl.h"

const char* const short_option = ":n:d:h";
struct option long_option[] = {
	{"name", 1, NULL, 'n'},
	{"dir", 1, NULL, 'd'},
	{"help", 0, NULL, 'h'},
	{0, 0, 0, 0},
};

void print_usage()
{
	printf("Usage:\n");
	printf("devcreate -n --name dev_name -d --dir dev_path\n");
	/* it seems that something strange here */
	printf("devcreate -a --all\n");
	printf("devcreate -h --help\n");
}

static int
get_input_create(int argc, char *argv[], struct create_param *user_create_param)
{
	int ret = 0;
	_Bool flag_n = 0, flag_d = 0, flag_h = 0;
	char result, invalid_char;
	char *device_name;
	int check_ch;

	if (1 == argc) {
		printf("Please provide a device name\n");
		print_usage();
		ret = EINVAL;
		goto OUT;
	} else {
		while ((result = getopt_long(argc, argv,
						short_option, long_option, NULL)) != -1) {
			switch(result){
				case 'n':
					if (!flag_n){
						memcpy(user_create_param->dev_name,
								optarg, (strlen(optarg)+1));
						flag_n = !flag_n;
					}else {
						printf("Option -n/--name may not be repeated.\n");
						ret = EINVAL;
						goto OUT;
					}
					break;
				case 'd':
					if(!flag_d){
						memcpy(user_create_param->dev_path,optarg,(strlen(optarg)+1));
						flag_d = !flag_d;
					}else{
						printf("Option -d/--dir may not be repeated.\n");
						ret = EINVAL;
						goto OUT;
					}
					break;
				case 'h':
					if(!flag_h){
						flag_h = !flag_h;
					}
					else{
						printf("Option -h/--help may not be repeated.\n");
						ret = EINVAL;
						goto OUT;
					}
					break;
				case '?':
					invalid_char = optopt;
					if (0 == invalid_char) {
						printf("devcreate: unrecognized option \'%s\'\n", argv[1]);
					} else {
						printf("devcreate: invalid option %c\n",invalid_char);
					}
					ret = EINVAL;
					break;
				case':':
					printf("Lack of argument\n");
					ret = EINVAL;
					goto OUT;
				default:
					break;
			}
		}
	}

	if(optind < argc){
		printf("\tDevcreate: unrecognized option ");
		while(optind < argc){
			printf("%s ",argv[optind++]);
		}
		printf("\n");
		print_usage();
		ret = EINVAL;
		goto OUT;
	}
	if(flag_n && !flag_d){
		printf("\tDirectory required for device \'%s\'.\n",user_create_param->dev_name);
		print_usage();
		ret = EINVAL;
		goto OUT;
	}
	if(!flag_n && flag_d){
		printf("\tDevice name required for Directory \'%s\'.\n",user_create_param->dev_path);
		print_usage();
		ret = EINVAL;
		goto OUT;
	}
	/*chech whether dev_name invalid*/
	if(strlen(user_create_param->dev_name) > 31){
		printf("\tDevcreate: dev_name \'%s\' is invalid.\n",user_create_param->dev_name);
		ret = EINVAL;
		goto OUT;
	}

	for(device_name=user_create_param->dev_name;*device_name!='\0';device_name++){
		check_ch = isalnum(*device_name);
		if(!check_ch){
			ret = EINVAL;
			printf("\tDevcreate: dev_name \'%s\' is invalid.\n",user_create_param->dev_name);
			goto OUT;
		}
	}
	if(flag_h) {
		ret = HELP;
	}

OUT:
	return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	unsigned long dev_number = 0;
	char name_buf[NAME_LEN_BUF];
	struct create_param user_create_param;
	struct stat dev_name_buf;
	struct stat dev_path_buf;

	ret = get_input_create(argc, argv, &user_create_param);
	if (HELP == ret) {
		print_star;
		print_usage();
		print_star;
		goto OUT;
	} else if (ret) {
		printf("Error during parsing command line.\n");
		goto OUT;
	}

	snprintf(name_buf, NAME_LEN_BUF, "/dev/%s", user_create_param.dev_name);

	ret = stat(name_buf, &dev_name_buf);
	if (!ret){
		printf("dev name \'%s\' has already existed.\n", user_create_param.dev_name);
		ret = EEXIST;
		goto OUT;
	}
	ret = stat(user_create_param.dev_path, &dev_path_buf);
	if (ret) {
		printf("phy dev \'%s\' does not exist.\n", user_create_param.dev_path);
		ret = ENOTBLK;
		goto OUT;
	}
	fd = open("/dev/"MISC_NAME, O_RDWR, 0);
	if (fd < 0) {
		printf("failed to open \'/dev/%s\'.", MISC_NAME);
		ret = ENOENT;
		goto OUT;
	}
	ret = ioctl(fd, GET_NUM_CMD, &dev_number);
	if (-1 == ret) {
		ret =  errno;
		printf("get dev_num is failed.\n");
		close(fd);
		goto OUT;
	}
	if (dev_number > MAX_DEVICE) {
		//ret =  errno;
		printf("only 1024 devices is permitted.\n");
		close(fd);
		goto OUT;
	}
	ret = ioctl(fd, CREATE_CMD, &user_create_param);
	if (-1 == ret) {
		ret = errno;
		printf("failed to execute ioctl.\n");
		close(fd);
		goto OUT;
	}
	printf("dev named %s is created successfully.\n", user_create_param.dev_name);

	close(fd);

OUT:
	return ret;
}
