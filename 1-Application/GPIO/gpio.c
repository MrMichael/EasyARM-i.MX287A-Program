#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//文件操作需要的头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <errno.h>

#define direction_path	"/sys/devices/virtual/gpio/gpio68/direction"
#define value_path	"/sys/devices/virtual/gpio/gpio68/value"

//先通过/sys/class/gpio/目录，导出特定的GPIO，然后再修改其文件，让系统文件的变化而执行相应操作。

int main(void)
{
	int fd = 0;
	char buf[10];
	
	fd = open(direction_path,O_RDWR);
	if(fd < 0) {
		perror(direction_path);
	}
	lseek(fd, 0, SEEK_SET);
	write(fd, "out", 3);
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, 5);
	printf("get value:%s \n", buf);
	close(fd);
	
	fd = open(value_path,O_RDWR);
	if(fd < 0) {
		perror(value_path);
	}
	lseek(fd, 0, SEEK_SET);
	write(fd, "1", 1);
	close(fd);
	
	printf("ok\n");
	
	return 0;
}













