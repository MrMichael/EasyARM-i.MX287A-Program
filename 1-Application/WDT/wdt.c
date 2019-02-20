#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/watchdog.h>

//内核自带看门狗驱动

static const char *device = "/dev/watchdog";                             /* 设备驱动文件路径             */
static int enable    = 1;                                                /* 执行程序默认使能看门狗       */
static int timeout   = 20;                                               /* 默认配置的看门狗溢出时间     */
static int keepalive = 5;                                                /* 默认进行5秒喂狗演示          */



/*********************************************************************************************************
** Function name:           print_usage
** Descriptions:            程序命令行参数用法提示
** input parameters:        prog:程序描述字符串
** output parameters:       无
** Returned value:          无
*********************************************************************************************************/
void print_usage(const char *prog)
{
    printf("Usage: %s [-desk]\r\n", prog);
    puts("  -d --device       device to use (default /dev/watchdog)\r\n"
         "  -e --enable       0 = disable, 1 = enable(default)\r\n"
         "  -s --timeout      seconds for timeout setting, 1~4194304(default 20)\r\n"
         "  -k --keepalive    seconds for keep alive test show, 0~4194304(default 5)\r\n");
    exit(1);
}

/*********************************************************************************************************
** Function name:           parse_opts
** Descriptions:            解释并处理命令行参数
** input parameters:        argc:命令行参数个数; argv:参数列表
** output parameters:       无
** Returned value:          无
*********************************************************************************************************/
void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            {"device",    1, 0, 'd'},
            {"enable",    1, 0, 'e'},
            {"timeout",   1, 0, 's'},
            {"keepalive", 1, 0, 'k'},
            {NULL, 0, 0, 0},
        };
        int c;
        
        c = getopt_long(argc, argv, "d:e:s:k:", lopts, NULL);
        
        if (c == -1)
        break;
        
        switch (c) {
            case 'd':
            device = optarg;
            break;
            case 'e':
            enable = atoi(optarg);
            if ((enable < 0) || (enable > 1)) {
                printf("enable parameter error,it must be 0 or 1\r\n");
                exit(1);
            }
            break;
            case 's':
            timeout = atoi(optarg);
            if ((timeout <= 0) || (timeout > (0x10000000 >> 6))) {
                printf("timeout parameter error,it must be 1 to 4194304\r\n");
                exit(1);
            }
            break;
            case 'k':
            keepalive = atoi(optarg);
            if ((keepalive < 0) || (keepalive> (0x10000000 >> 6))) {
                printf("keepalive parameter error,it must be 0 to 4194304\r\n");
                exit(1);
            }
            break;
            default:
            print_usage(argv[0]);
            break;
        }
    }
}


int main(int argc, char *argv[])
{
	int fd;
	int ret = 0;
	int i = 0;
	int opts = 0;
	
	parse_opts(argc, argv);
	fd = open(device, O_RDWR);                                           /* 读写方式打开设备文件         */
    if (fd < 0) {
        printf("device open failed!\r\n");                               /* 设备文件打开失败             */
        return -1;
    }
	
	//设备文件打开后看门狗随机被使能，溢出时间为内核中记录的上一次的设置值，首次设置默认为19秒
    printf("device open succeed, watchdog enable, "\
           "system will be restart in %d second if not keep alive!\r\n", timeout);
	
	if(enable == 0) {
		// 禁能看门狗前需先写入'V'(WDT_OK_TO_CLOSE)，防止误关闭
		if(write(fd, "V", 1) != 1) {
			printf("write WDT_OK_TO_CLOSE failed!");
		}
		opts = WDIOS_DISABLECARD;
		ret = ioctl(fd, WDIOC_SETOPTIONS, &opts);    
		printf("watchdog disable!\r\n");
	} else {
		ioctl(fd, WDIOC_SETTIMEOUT, &timeout); 
		if(keepalive > 0) {
			for (i = 0; i < keepalive; i++) {
				ret = ioctl(fd, WDIOC_KEEPALIVE, 0);                     /* 喂狗操作                     */
				sleep(1); 
				printf("keep alive:%d s, ioctl ret = %d\r\n", i + 1, ret);
			}
		}
	}
	
	ret = close(fd);                                                     /* 关闭设备文件                 */
    if (ret != 0) {
        printf("device closed unexpectdly, watchdog keep on!\r\n");
    }
    return ret;
}





















