#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//文件操作需要包含的头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>


int main(void)
{
	int fd;
	char buf[1] = {0};
	
	//加载驱动会在/dev/目录下出现驱动虚拟出的新设备
	//对驱动设备进行文件操作就会指向驱动程序定义的文件操作
	fd = open("/dev/imx28x_led", O_RDWR);	//gpio_request(LED_GPIO, "led"); 请求GPIO
	if(fd < 0) {
		perror("open /dev/imx283_led");
	}
	
	printf("test write...\n");
	buf[0] = 0;
	write(fd, buf, 1);	//gpio_direction_output(LED_GPIO, data[0]);
	sleep(1);
	
	printf("test ioctl...\n");
	ioctl(fd, 0);	//gpio_direction_output(LED_GPIO, 0); 置高电平
	sleep(2);
	ioctl(fd, 1);	//gpio_direction_output(LED_GPIO, 1); 置低电平
	sleep(1);
	

	return 0;
}






















