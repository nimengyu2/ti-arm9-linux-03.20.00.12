#include <linux/miscdevice.h> 
#include <linux/delay.h> 
#include <asm/irq.h> 
#include <mach/hardware.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/mm.h> 
#include <linux/fs.h> 
#include <linux/types.h> 
#include <linux/delay.h> 
#include <linux/moduleparam.h> 
#include <linux/slab.h> 
#include <linux/errno.h> 
#include <linux/ioctl.h> 
#include <linux/cdev.h> 
#include <linux/string.h> 
#include <linux/list.h> 
#include <linux/pci.h> 
#include <linux/gpio.h> 
#include <asm/uaccess.h> 
#include <asm/atomic.h> 
#include <asm/unistd.h> 

#include <linux/version.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <mach/cputype.h>
#include <mach/hardware.h>
#include <mach/mux.h>
#include <mach/gpio.h>
#include <asm/gpio.h>

#include <linux/delay.h>
#include <mach/da8xx.h>
#include <linux/clk.h>

#define DEVICE_NAME "bus-cs4" //设备名(/dev/led) 

#define MOTOR_MAGIC 'P'
#define WRITE_DATA			_IOW(MOTOR_MAGIC, 2,int)
#define READ_DATA 			_IOW(MOTOR_MAGIC, 3,int)


//#define EHRPWM0_BASE  	IO_ADDRESS(0x62000000)	
#define EHRPWM0_BASE  	((volatile unsigned long)ioremap(0x62000000,4))


void fn_write_data_byte(unsigned char data,unsigned long addr)
{
	//while(1)
	{
		__raw_writeb(data, EHRPWM0_BASE+addr);
		//udelay(1);
	}		
}

void fn_test_bus_init(void)
{
	printk("test bus init ok\n");
}

void fn_test_bus_exit(void)
{
	printk("test bus exit ok\n");
}

 
// ioctl 函数的实现 
// 在应用用户层将通过 ioctl 函数向内核传递参数，以控制 LED的输出状态 
static int am1808_led_ioctl( 
 struct inode *inode,  
 struct file *file,   
 unsigned int cmd,  
 unsigned long arg) 
{ 
   int gpio_num_tmp;
   printk("cmd is %d \n",cmd);
   printk("arg is %d \n",arg);
   switch (cmd)
   {
       case WRITE_DATA:
       fn_write_data_byte(0,arg);
       break;
       case READ_DATA:
       
       break;
       default:
       printk("cmd may be 2 3 4 5 \n");
       return -EINVAL; 
   } 
   return 0;       
} 
 
 
//  设备函数操作集，在此只有 ioctl函数，通常还有 read, write, open, close 等，因为本 LED驱动在下面已经
//  注册为 misc 设备，因此也可以不用 open/close  
static struct file_operations dev_fops = { 
 .owner = THIS_MODULE, 
 .ioctl = am1808_led_ioctl, 
}; 
  
//  把 LED驱动注册为 MISC 设备 
static struct miscdevice misc = { 
  //动态设备号
  .minor = MISC_DYNAMIC_MINOR,  
  .name = DEVICE_NAME, 
  .fops = &dev_fops, 
}; 
 
 
// 设备初始化 
static int __init dev_init(void) 
{ 
   int ret;   
   ret = misc_register(&misc); //注册设备 
   printk (DEVICE_NAME"\tinitialized\n"); //打印初始化信息 
   return ret; 
} 
 
static void __exit dev_exit(void) 
{ 
    misc_deregister(&misc); 
} 
 
// 模块初始化，仅当使用 insmod/podprobe 命令加载时有用，
// 如果设备不是通过模块方式加载，此处将不会被调用 
module_init(dev_init); 

// 卸载模块，当该设备通过模块方式加载后，
// 可以通过 rmmod 命令卸载，将调用此函数 
module_exit(dev_exit);

// 版权信息 
MODULE_LICENSE("GPL"); 
// 开发者信息 
MODULE_AUTHOR("lierda Inc."); 

