#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>

#include "gpio.h"

//执行测试程序之前需要先加载好外部驱动模块GPIO_driver.ko

void show_help(void)
{
	printf("example:\n");
	printf("	gpio /dev/gpio-xxx H	#set gpio output higth \n");
	printf("	gpio_test /dev/gpio-xxx L  #set gpio output low   \n");
	printf("	gpio_test /dev/gpio-xxx R  #read gpio input status(H/L) \n");
}


int main(int argc, char *argv[])
{
	int fd;
	char cmd;
	int data;
	
	if(argc < 3) {
		show_help();
		return -1;
	}
	
	//打开虚拟驱动设备	
	fd = open(argv[1], O_RDWR);	//gpio_request
	if(fd < 0) {
		printf("faile to open %s \n", argv[1]);
		return -1;	
	}
	
	cmd = argv[2][0];
	
	switch(cmd) {
	case 'H':
		printf("set %s output hight\n", argv[1]);
		ioctl(fd, SET_GPIO_HIGHT); //gpio_direction_output(MXS_PIN_TO_GPIO(gpio_info_tmp->pin), 1);
		break;
	case 'L':
		printf("set %s output low\n", argv[1]);
		ioctl(fd, SET_GPIO_LOW); //gpio_direction_output(MXS_PIN_TO_GPIO(gpio_info_tmp->pin), 0);
		break;
	case 'R':
		ioctl(fd, GET_GPIO_VALUE, (long)(&data));
		if(data) {
			printf("get %s %d status H \n", argv[1], data);
		} else {
			printf("get %s %d status L \n", argv[1], data);
		}
		break;
	default:
		show_help();
		break;
	}
	
	return 0;
}










