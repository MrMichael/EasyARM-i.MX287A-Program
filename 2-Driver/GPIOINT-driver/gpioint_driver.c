#include<linux/init.h>
#include<linux/module.h>
#include<mach/gpio.h>                                                  
#include<asm/io.h>                                                 
#include"mach/../../mx28_pins.h"
#include <mach/pinctrl.h>
#include "mach/mx28.h"
#include<linux/fs.h>
#include <linux/io.h>
#include<asm/uaccess.h>                                     
#include<linux/miscdevice.h>                          
#include<linux/irq.h>                          
#include<linux/sched.h>                   
#include<linux/interrupt.h>              
#include<linux/timer.h>

#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>


#define GPIO_BUTTON_PIN		MXS_PIN_TO_GPIO(PINID_AUART0_RX)
//#define GPIO_BUTTON_PIN		MXS_PIN_TO_GPIO(PINID_SSP0_DATA4)


//中断处理函数
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	printk("irq test \n");	

	return IRQ_RETVAL(IRQ_HANDLED);
}


static int __init gpio_drv_init(void)
{
	int irq_no;
	int iRet;

	gpio_free(GPIO_BUTTON_PIN);
	iRet = gpio_request(GPIO_BUTTON_PIN, "irqtest");
	if (iRet != 0) {
		printk("request gpio failed \n");
		return -EBUSY;
	}

	gpio_direction_input(GPIO_BUTTON_PIN);
	irq_no  = gpio_to_irq(GPIO_BUTTON_PIN); 
	set_irq_type(irq_no, IRQF_TRIGGER_FALLING);	//下降沿中断

	//申请中断并设置中断处理函数
	iRet = request_irq(irq_no, buttons_irq, IRQF_DISABLED, "gpio_int", NULL);
	if (iRet != 0){
		printk("request irq failed!! ret: %d  irq:%d gpio:%d  \n", iRet, irq_no, GPIO_BUTTON_PIN);
		return -EBUSY;
	}

	return 0;
}

static void __exit gpio_drv_exit(void)
{
	int irq_no;

	irq_no  = gpio_to_irq(GPIO_BUTTON_PIN); 
	free_irq(irq_no, NULL);

	gpio_free(GPIO_BUTTON_PIN);
}

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);

MODULE_AUTHOR("EasyARM28xx By zhuguojun");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("gpio button interrupt module");

