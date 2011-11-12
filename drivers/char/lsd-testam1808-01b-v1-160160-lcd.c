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

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/crc32.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/dm9000.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/irq.h>

#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/io.h>

#define DEVICE_NAME "am1808-lcd160160" //设备名(/dev/lcd) 

#define MOTOR_MAGIC 'm'
#define LCD_INIT 	_IOW(MOTOR_MAGIC, 2,int)
#define LCD_WRITE_CMD	_IOW(MOTOR_MAGIC, 3,int)
#define LCD_WRITE_DATA  _IOW(MOTOR_MAGIC, 4,int)

void __iomem *write_cmd_addr;
void __iomem *write_data_addr;


void fn_delay_ms(unsigned int ms)
{
    //sleep(ms);
    mdelay(ms);
}

void fn_delay_us(unsigned int us)
{
    //sleep(ms);
    udelay(us);
}

//-----------------------------------------------------------------------------
//
// 函数名称  :  fn_lcd_write_data 
// 函数功能  :  
//
// 输入参数  :  无
//
// 返回参数  :  无
// 
//-----------------------------------------------------------------------------
static void fn_lcd_write_data(unsigned char data)
{
	writeb(data, write_data_addr );
	//fn_delay_us(2);
}

//-----------------------------------------------------------------------------
//
// 函数名称  :  fn_lcd_write_cmd 
// 函数功能  :  写一个字节命令
//
// 输入参数  :  无
//
// 返回参数  :  无
// 
//-----------------------------------------------------------------------------
static void fn_lcd_write_cmd(unsigned char cmd)
{
	writeb(cmd, write_cmd_addr );
	//fn_delay_us(2);
}



//-----------------------------------------------------------------------------
//
// 函数名称  :  fn_lcd_init
// 函数功能  :  液晶初始化
//
// 输入参数  :  无
//
// 返回参数  :  无
// 
//-----------------------------------------------------------------------------
void fn_lcd_init(void)
{
#if 0
         LCD_BLK_L();

	// 硬件上电复位，低电平有效
	LCD_RESET_L();
	fn_delay_ms(2);			
	LCD_RESET_H();
	fn_delay_ms(400);
#endif
	// 发送复位命令
	fn_lcd_write_cmd(0xe2);		
	fn_delay_ms(20);

	// 供电控制
	// Bias Ratio:1/10 bias
	fn_lcd_write_cmd(0xe9);	
	// Bias Ratio:1/12 bias		
    // fn_lcd_write_cmd(0xeb);	
	// power control set as internal power	
    fn_lcd_write_cmd(0x2b);			
	// set temperate compensation as 0.05%
	fn_lcd_write_cmd(0x25);			
	// set temperate compensation as 0%
    // fn_lcd_write_cmd(0x24);		

	fn_lcd_write_cmd(0x81);			//electronic Potentiometer
	fn_lcd_write_cmd(0xbf);
    // fn_lcd_write_cmd(0xa0);
	// display control 
	fn_lcd_write_cmd(0xa4);			//all pixel off
	fn_lcd_write_cmd(0xa6);			//inverse display off

	// lcd 控制 
	fn_lcd_write_cmd(0xc0);			//18:partial display and MX disable,MY enable
        fn_lcd_write_cmd(0xa3);		        //line rate 15.2klps
	fn_lcd_write_cmd(0xd1);			//20:rgb-rgb
	fn_lcd_write_cmd(0xd5);			//21:4k color mode
	fn_lcd_write_cmd(0x84);			//11:partial display control disable
   
	// n-line inversion 
	fn_lcd_write_cmd(0xc8);    		// SET N-LINE INVERSION
	fn_lcd_write_cmd(0x10);			//enable NIV

	// com scan fuction 
	fn_lcd_write_cmd(0xda);			//22:enable FRC,PWM,LRM sequence

	// window 
	fn_lcd_write_cmd(0xf4);			//wpc0:column
	fn_lcd_write_cmd(0x25);			//start from 130
	fn_lcd_write_cmd(0xf6);			//wpc1
	fn_lcd_write_cmd(0x5A);			//end:272

	fn_lcd_write_cmd(0xf5);			//wpp0:row
	fn_lcd_write_cmd(0x00);			//start from 0
	fn_lcd_write_cmd(0xf7);			//wpP0
	fn_lcd_write_cmd(0x9F);			//end 160

	fn_lcd_write_cmd(0xf8);			//inside mode

	fn_lcd_write_cmd(0x89);			//12:RAM control,


	fn_lcd_write_cmd(0xad);			//display on,select on/off mode.Green Enhance mode disable
	// fn_lcd_write_cmd(0xaf);               //32-shade mode
	// scroll line
	fn_lcd_write_cmd(0x40);			//low bit of scroll line
	fn_lcd_write_cmd(0x50);			//high bit of scroll line
	fn_lcd_write_cmd(0xc0);			//enable FLT and FLB, 18:SET LCD MAPPING CONTROL
	fn_lcd_write_cmd(0x90);			//13:FLT,FLB set
	fn_lcd_write_cmd(0x00);

	// partial display
	fn_lcd_write_cmd(0x84);			//11:set partial display control:off
	fn_lcd_write_cmd(0xf1);			//com end
	fn_lcd_write_cmd(0x9f);			//160
	fn_lcd_write_cmd(0xf2);			//display start
	fn_lcd_write_cmd(0);		     //0
	fn_lcd_write_cmd(0xf3);			//display end
	fn_lcd_write_cmd(159);			//160
#if 0
        LCD_BLK_H();
#endif
}



 
// ioctl 函数的实现 
// 在应用用户层将通过 ioctl 函数向内核传递参数，以控制 LED的输出状态 
static int am1808_lcd_ioctl( 
 struct inode *inode,  
 struct file *file,   
 unsigned int cmd,  
 unsigned long arg) 
{ 
   //int gpio_num_tmp;
   //printk("cmd is %d \n",cmd);
   switch (cmd)
   {
       case LCD_INIT :
       fn_lcd_init();
       break;
       case LCD_WRITE_CMD:
       fn_lcd_write_cmd(arg&0xff);
       break;
       case LCD_WRITE_DATA :
       fn_lcd_write_data(arg&0xff);
       break;
       default:
       printk("cmd may be ok \n");
       return -EINVAL; 
   } 

   /*printk("arg is %d \n",arg); 
   if(arg == 0)
   {
      gpio_set_value(gpio_num_tmp,0); 
   }
   else if(arg == 1)
   {
     gpio_set_value(gpio_num_tmp,1); 
   }
   else
   {
     printk("arg may be 0|1 \n");
   }*/
   return 0;       
} 
 
 
//  设备函数操作集，在此只有 ioctl函数，通常还有 read, write, open, close 等，因为本 LED驱动在下面已经
//  注册为 misc 设备，因此也可以不用 open/close  
static struct file_operations dev_fops = { 
 .owner = THIS_MODULE, 
 .ioctl = am1808_lcd_ioctl, 
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
    int i;  
    int status;

#if 0
   for(i = 0; i < 13 ; i++)
   {
	    // init/set pinmux 
	   status = davinci_cfg_reg(gpio_pin[i]);
	   if (status < 0) {
		printk("pin could not be muxed for GPIO functionality %d\n",
								gpio_num[i]);
		return status;
	   }
	
	   status = gpio_request(gpio_num[i], "gpio_test\n");
	   if (status < 0) {
	 	printk("ERROR can not open GPIO %d\n", gpio_num[i]);
		return status;
	   }
	   gpio_direction_output(gpio_num[i],0); 
   }    
#endif
	write_cmd_addr = 	((volatile unsigned long)ioremap(0x60000000,4));
	write_data_addr = 	((volatile unsigned long)ioremap(0x60000008,4));

   ret = misc_register(&misc); //注册设备 
   printk (DEVICE_NAME"\tinitialized\n"); //打印初始化信息 
   return ret; 
} 
 
static void __exit dev_exit(void) 
{ 
   int i;
#if 0
   for(i = 0; i <=13 ; i++)
   {
	gpio_free(gpio_num[i]);
   }
#endif
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
MODULE_AUTHOR("FriendlyARM Inc."); 



