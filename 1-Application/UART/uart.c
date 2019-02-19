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

#define DATA_LEN                0xFF                                    /* test data's len              */

//基于内核自带的uart驱动模块


/*********************************************************************************************************
** Function name:           openSerial
** Descriptions:            open serial port at raw mod
** input paramters:         iNum        serial port which can be value at: 1, 2, 3, 4
** output paramters:        NONE
** Return value:            file descriptor
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static int openSerial(char *cSerialName)
{
    int iFd;

    struct termios opt; 

    iFd = open(cSerialName, O_RDWR | O_NOCTTY);                        
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt);      

    //cfsetispeed(&opt, B57600);
    //cfsetospeed(&opt, B57600);

     cfsetispeed(&opt, B115200);
     cfsetospeed(&opt, B115200);

    
    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   DATA_LEN;                                      
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }


    return iFd;
}


int main(void)
{
	char tmp[1024];
	int len;
	int fd, i;
	
	//打开内核自带驱动模块添加的设备
	fd = openSerial("/dev/ttySP0");	//对应引脚URX0、UTX0
	if(fd < 0) {
		perror("openSerial\n");
	}
	
	for(i = 0; i < 16; i++) 
		tmp[i] = i%0xFF;
	
	write(fd, tmp, 16);
	
	while(1) {
		len = read(fd, tmp, 1);
		for(i = 0; i < len; i++)
			printf(" %d", tmp[i]);
		printf("\n");
	}
	
	return 0;
}























