/*********************************************************************************
**   ADC参考电压为1.85V，外部输入电压不能超过3.7V,内部带有一个模拟除2电路。
*********************************************************************************/

#include<stdio.h>	/* using printf()        */
#include<stdlib.h>      /* using sleep()         */
#include<fcntl.h>       /* using file operation  */
#include<sys/ioctl.h>   /* using ioctl()         */

#include "lradc.h"


//执行测试程序之前需要先加载好外部驱动模块lradc.ko


int main(int argc, char *argv[])
{
	int fd;
	int iRes;
	int time = 50;
	double val;
	
	//对驱动设备进行文件操作就会指向驱动程序定义的文件操作
	fd = open("/dev/magic-adc", 0);
	if(fd < 0) {
		printf("open error by APP- %d\n",fd);
		return -1;
	}
	
	while(time--) {
		sleep(1);
		ioctl(fd, IMX28_ADC_CH0_DIV2, &iRes); /* 开启除2     CH0    */ //对应引脚AI0
		val = (iRes * 3.7) / 4096.0;
		printf("CH0:%.2f  ", val);
		
		ioctl(fd, IMX28_ADC_CH1, &iRes);      /* 不开除2     CH1    */ //对应引脚AI1
		val = (iRes * 1.85) / 4096.0;
		printf("CH1:%.2f  ", val);
		
		ioctl(fd,IMX28_ADC_CH6_DIV2, &iRes);  /* 开启除2     CH6    */ //对应引脚AI6
		val = (iRes * 3.7) / 4096.0;
        printf("CH6:%.2f  ", val);

        ioctl(fd,IMX28_ADC_VBAT_DIV4, &iRes); /* 电池电压默认除4    */
		val = (iRes * 7.4) / 4096.0;
        printf("Vbat:%.2f  ", val);
		
		printf("\n");
	}
	close(fd);
	
	return 0;
}










































