/*********************************************Copyright (c)***********************************************
**                                Guangzhou ZLG MCU Technology Co., Ltd.
**
**                                        http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               lradc.c
** Last modified date:      2013年11月28日15:57:38
** Last version:            
** Descriptions:           
**
**--------------------------------------------------------------------------------------------------------
** Created by:              周华
** Created date:            2013年11月28日15:57:44
** Version:           	    v1.0
** Descriptions:            ADC驱动，读取
**--------------------------------------------------------------------------------------------------------
** Modified by:       	cxf 
** Modified date:       2014-09-22
** Version:            
** Descriptions:        ADC Vref = Bandgap Reference(channel 14) = 1.85V
**			如果电压大于1.85V，需要开启硬件除2
**			ADC为12位
**			每个寄存器都有四个，分别用来 r/w 、set 、clear 、trogger 
**                          名称类似 HW_LRADC_CTRL0、HW_LRADC_CTRL0_SET、
**                          HW_LRADC_CTRL0_CLR、HW_LRADC_CTRL0_TOG       
**  HW_LRADC_CTRL0      SFTRST(31)  CLKGATE(30) SCHEDULE(7~0)   
**  HW_LRADC_CTRL1      CHn_IRQ_EN(23~16)   CHn_IRQ(7~0)        主要为触屏、ADC中断
**  HW_LRADC_CTRL2      CHn_DIV/2(31~24)                        主要为温度控制
**  HW_LRADC_CTRL3      DISCARD(25、24) CYCLE_TIME(9、8)HIGH_TIME(5/4)DELAY_CLK(1)INVERT_CLK(0)忽略、时钟
**  HW_LRADC_CTRL4      4*8 channels 0-16 channel selector
**                      
*********************************************************************************************************/

#include<linux/module.h>                                                /* module                       */
#include<linux/fs.h>                                                    /* file operation               */
#include<asm/uaccess.h>                                                 /* get_user()                   */
#include<linux/miscdevice.h>                                            /* miscdevice                   */
#include<asm/io.h>                                                      /* ioctl                        */
#include <mach/regs-lradc.h>						/* #define                      */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysdev.h>
#include <linux/platform_device.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/delay.h>

#include <mach/hardware.h>
#include <mach/device.h>
#include <mach/regs-lradc.h>
#include <mach/lradc.h>

#include "lradc.h"

#define  ZLG_IMX_283   0
#define  ZLG_IMX_287   1
#define  ZLG_IMX_280   2

extern int zlg_board_type;

static void __iomem *adc_base = NULL;


/*********************************************************************************************************
 文件操作接口
*********************************************************************************************************/
static int adc_open (struct inode *inode, struct file *fp) 
{

	try_module_get(THIS_MODULE);    

	//modified by cxf 2014-10-22 for imx280 ADCs
	if(ZLG_IMX_280 == zlg_board_type) {
		writel(BM_LRADC_CTRL0_SFTRST,adc_base + HW_LRADC_CTRL0_SET);
		udelay(1);
		writel(BM_LRADC_CTRL0_SFTRST,adc_base + HW_LRADC_CTRL0_CLR);
		/* Clear the Clock Gate for normal operation */
		writel(BM_LRADC_CTRL0_CLKGATE,adc_base + HW_LRADC_CTRL0_CLR);
	}
	writel(0xC0000000, adc_base + HW_LRADC_CTRL0_CLR);              /* 开启CLK，关闭复位模式        */
	writel(0x18430000, adc_base + HW_LRADC_CTRL1_CLR);              /* 关闭0-1-6通道及按键中断      */ 
	writel(0x30000000, adc_base + HW_LRADC_CTRL3_SET);              /* 忽略上电前三次采集数据       */     
	writel(0x76543210, adc_base + HW_LRADC_CTRL4);                  /* 设置对应的数据存放位置       */
	
	return 0;
}
static int adc_release (struct inode *inode, struct file *fp)
{
	module_put(THIS_MODULE);
    
	return 0;
}
/*********************************************************************************************************
**   cmd: 
**	不开启除2：
**	10:CH0 	11:CH1 	12:CH2 	13:CH3 	14:CH4  15:CH5  16:CH6 	 17:Vbat(内部除4)	  
**	开启除2: 
**      20:CH0	21:CH1  22:CH2  23:CH3  24:CH4  25:CH5  26:CH6 
**
**   被注释部分为3.0以上内核函数定义形式
*********************************************************************************************************/

//static int adc_ioctl(struct file *fp, struct file *flip, unsigned int cmd, unsigned long arg)
static int adc_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg)
{
    	int iRes;
	int iTmp;
	
/*
	if(cmd < 10 || cmd > 27) {
		return -1;
	}

	iTmp = cmd % 10;
	//CH2 ~ CH5 only for imx280
	if((zlg_board_type != ZLG_IMX_280) && (iTmp > 1) && (iTmp < 6)) {
		printk("Not ZLG_IMX_280 board.\n");
		return -1;
	} 
*/
	iTmp = _IOC_NR(cmd) % 10;

	switch(cmd){
	case IMX28_ADC_CH0:
		writel(0x01000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;

	case IMX28_ADC_CH0_DIV2:
		writel(0x01000000, adc_base + HW_LRADC_CTRL2_SET);
       		break; 

	case IMX28_ADC_CH1:
		writel(0x02000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;	
	
	case IMX28_ADC_CH1_DIV2:
                writel(0x02000000, adc_base + HW_LRADC_CTRL2_SET);
		break;
	
	case IMX28_ADC_CH2:
		writel(0x04000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;
	
	case IMX28_ADC_CH2_DIV2:
		writel(0x04000000, adc_base + HW_LRADC_CTRL2_SET);
       		break; 
	
	case IMX28_ADC_CH3:
		writel(0x08000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;	
	
	case IMX28_ADC_CH3_DIV2:
                writel(0x08000000, adc_base + HW_LRADC_CTRL2_SET);
		break;
	
	case IMX28_ADC_CH4:
		writel(0x10000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;
	
	case IMX28_ADC_CH4_DIV2:
		writel(0x10000000, adc_base + HW_LRADC_CTRL2_SET);
       		break; 
	
	case IMX28_ADC_CH5:
		writel(0x20000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;	
	
	case IMX28_ADC_CH5_DIV2:
                writel(0x20000000, adc_base + HW_LRADC_CTRL2_SET);
		break;
	
	case IMX28_ADC_CH6:
		writel(0x40000000, adc_base + HW_LRADC_CTRL2_CLR);
		break;
        
	case IMX28_ADC_CH6_DIV2:
                writel(0x40000000, adc_base + HW_LRADC_CTRL2_SET);
		break;

	case IMX28_ADC_VBAT:	
        case IMX28_ADC_VBAT_DIV4:
		break;

	default:
		printk("adc control cmd invalid!!\n");
		return -1;
	}

        /* Clear the accumulator & NUM_SAMPLES */
        __raw_writel(0xFFFFFFFF,
                     adc_base + HW_LRADC_CHn_CLR(iTmp));

        /* Clear the interrupt flag */
        __raw_writel(1 << iTmp,
                     adc_base + HW_LRADC_CTRL1_CLR);
        
        __raw_writel(BF_LRADC_CTRL0_SCHEDULE(1 << iTmp),
                     adc_base + HW_LRADC_CTRL0_SET);

        /* wait for completion */
        while ((__raw_readl(adc_base + HW_LRADC_CTRL1)
                & (1 << iTmp)) != (1 << iTmp))
                cpu_relax();

        /* Clear the interrupt flag */
        __raw_writel(1 << iTmp,
                     adc_base + HW_LRADC_CTRL1_CLR);

        iRes =  __raw_readl(adc_base + HW_LRADC_CHn(iTmp)) &
                           BM_LRADC_CHn_VALUE;
   	copy_to_user((void *)arg, (void *)(&iRes), sizeof(int));
    
    	return 0;
}
/*********************************************************************************************************
Device Struct
*********************************************************************************************************/
struct file_operations adc_fops = 
{
	.owner		= THIS_MODULE,
	.open		= adc_open,
	.release	= adc_release,
    	.ioctl      	= adc_ioctl,
};
static struct miscdevice adc_miscdev = 
{
	.minor	        = MISC_DYNAMIC_MINOR,
   	.name	        = "magic-adc",
    	.fops	        = &adc_fops,
};
/*********************************************************************************************************
Module Functions
*********************************************************************************************************/
static int __init adcModule_init (void)
{
    	int iRet=0;

	if (ZLG_IMX_283 == zlg_board_type) {
		printk("zlg EasyARM-imx283 adc driver up. \n");
	} else if(ZLG_IMX_287 == zlg_board_type) {
		printk("zlg EasyARM-imx287 adc driver up. \n");
	} else if(ZLG_IMX_280 == zlg_board_type) {
		//printk("zlg EasyARM-imx280 adc driver up. \n");
	} 	
	adc_base = ioremap(0x80050000, 0x180*4);    
    	iRet = misc_register(&adc_miscdev);
	if (iRet) {
		printk("register failed!\n");
	} 
	return iRet;
}
static void __exit adcModule_exit (void)                                /* warning:return void          */
{
	printk("zlg EasyARM-imx28xx adc driver down.\n");
	misc_deregister(&adc_miscdev);
}
/*********************************************************************************************************
Driver Definitions
*********************************************************************************************************/
module_init(adcModule_init);
module_exit(adcModule_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("admin@9crk.com");
MODULE_DESCRIPTION("EasyARM283 By zhouhua");
/*********************************************************************************************************
End File
*********************************************************************************************************/


