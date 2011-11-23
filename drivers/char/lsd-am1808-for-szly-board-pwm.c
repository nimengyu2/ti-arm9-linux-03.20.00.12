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

#define DEVICE_NAME "pwm" //设备名(/dev/pwm) 

#define MOTOR_MAGIC 'P'
#define SET_PWM_PIN_HIGH 	_IOW(MOTOR_MAGIC, 2,int)
#define SET_PWM_PIN_LOW 	_IOW(MOTOR_MAGIC, 3,int)
#define SET_PWM_ON 		_IOW(MOTOR_MAGIC, 4,int)
#define SET_PWM_OFF 		_IOW(MOTOR_MAGIC, 5,int)

// 使用的引脚为GP2[14]
// SPI1_SCS[0]/EPWM1B/PRU0_R30[8]/GP2[14]/TM64P3_IN12

// test this IO
#define PWM1B_PIN   DA850_GPIO2_14
#define PWM1B_NUM   (2*16+14)
 
#define SYSCFG0_BASE  	0x01c14000
#define CFGCHIP1        IO_ADDRESS(SYSCFG0_BASE)+0x180
#define CFGCHIP1_TBCLKSYNC      (1 << 12)

//#define PWM1B_GPIO   1
#define EHRPWM0_BASE  		0x01F00000
#define EHRPWM0_TBCTL 		IO_ADDRESS(EHRPWM0_BASE)+0x00
#define EHRPWM0_TBSTS 		IO_ADDRESS(EHRPWM0_BASE)+0x02
#define EHRPWM0_TBPHSHR 	IO_ADDRESS(EHRPWM0_BASE)+0x04
#define EHRPWM0_TBPHS 		IO_ADDRESS(EHRPWM0_BASE)+0x06
#define EHRPWM0_TBCNT 		IO_ADDRESS(EHRPWM0_BASE)+0x08
#define EHRPWM0_TBPRD 		IO_ADDRESS(EHRPWM0_BASE)+0x0A
#define EHRPWM0_CMPCTL 		IO_ADDRESS(EHRPWM0_BASE)+0x0E
#define EHRPWM0_CMPAHR 	IO_ADDRESS(EHRPWM0_BASE)+0x10
#define EHRPWM0_CMPA 		IO_ADDRESS(EHRPWM0_BASE)+0x12
#define EHRPWM0_CMPB 		IO_ADDRESS(EHRPWM0_BASE)+0x14
#define EHRPWM0_AQCTLA  	IO_ADDRESS(EHRPWM0_BASE)+0x16
#define EHRPWM0_AQCTLB 		IO_ADDRESS(EHRPWM0_BASE)+0x18
#define EHRPWM0_AQSFRC 		IO_ADDRESS(EHRPWM0_BASE)+0x1A
#define EHRPWM0_AQCSFRC 	IO_ADDRESS(EHRPWM0_BASE)+0x1C
#define EHRPWM0_DBCTL 		IO_ADDRESS(EHRPWM0_BASE)+0x1E
#define EHRPWM0_DBRED 		IO_ADDRESS(EHRPWM0_BASE)+0x20
#define EHRPWM0_DBFED 		IO_ADDRESS(EHRPWM0_BASE)+0x22
#define EHRPWM0_PCCTL 		IO_ADDRESS(EHRPWM0_BASE)+0x3C
#define EHRPWM0_TZSEL 		IO_ADDRESS(EHRPWM0_BASE)+0x24
#define EHRPWM0_TZCTL 		IO_ADDRESS(EHRPWM0_BASE)+0x28

extern struct clk pwm1_clk;
unsigned long g_u32_pwm_output_freq = 0;


// 经过测试,发现时钟应该就是150MHZ,但是实际通过示波器测试的时候发现
// 38KHZ的时候,频率不是很准,这个是因为示波器在测试频率的时候不是按照
// 计数法测的,而是纯粹看示波器表面的时间间距,不同档位的频率是不一样的
// 后来使用万用表测频率还是比较准确的
#define M_SYSTEM_CLOCK  150000000
#define M_PWM0B_OUTPUT_FREQ   38000

// pwm初始化
int fn_pwm_init(void)
{
	unsigned short u16_tmp;
	int i;
	unsigned long u32_period;
	clk_enable(&pwm1_clk);	

	// 开启PWM同步	
	u16_tmp = __raw_readw(CFGCHIP1);
	u16_tmp &= (~CFGCHIP1_TBCLKSYNC);
	__raw_writew(u16_tmp, CFGCHIP1);	

	// 设定周期值
	if(g_u32_pwm_output_freq != 0)
	{
		// 计算周期             
		u32_period =  M_SYSTEM_CLOCK/g_u32_pwm_output_freq;
        }  
	else
	{
		printk("PWM freq can't be 0,error!");             
		return 0;
	}	 
	// 写入周期值
	u16_tmp = u32_period;
	__raw_writew(u16_tmp, EHRPWM0_TBPRD);	
	//printk("pwm1b set period = %d \n",u16_tmp);

	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_TBPHSHR);
	
	// 增计数  分频 128*14	PRDLD=1  PHSEN=0  CTRMODE=0
	// Time-Base Control Register
	//u16_tmp = (0x07 << 10) | (0x07 << 7) | (3 << 4);    
	// 不分频	
	u16_tmp = (3 << 4);  
	__raw_writew(u16_tmp, EHRPWM0_TBCTL);
	
	// 比较控制器 SHDWBMODE=1 SHDWAMODE=1
	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_CMPCTL);

	// 输出控制A  CAU=2  PRD=1  ZRO = 0
	u16_tmp = (0x02 << 4) | (0x01 << 2) | (0x00);    
	__raw_writew(u16_tmp, EHRPWM0_AQCTLA);

	// 输出控制B  CBU=2  PRD=1  ZRO = 0
	u16_tmp = (0x02 << 8) | (0x01 << 2) | (0x00);    
	__raw_writew(u16_tmp, EHRPWM0_AQCTLB);

	// 比较值
	u16_tmp = u32_period/2;
	__raw_writew(u16_tmp, EHRPWM0_CMPA);
	// 比较值
	u16_tmp = u32_period/2;
	__raw_writew(u16_tmp, EHRPWM0_CMPB);

	// 计数值为0
	// Time-Base Counter Register
	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_TBCNT);

	// 死区控制直接输出
	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_DBCTL);

	// PC short 旁路
	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_PCCTL);

	//  死区时间
	u16_tmp = 0;    
	__raw_writew(u16_tmp, EHRPWM0_TZSEL);

	u16_tmp = (1 << 2) | (1);    
	__raw_writew(u16_tmp, EHRPWM0_TZCTL);

	// 使能时钟	
	clk_enable(&pwm1_clk);	

	//  使能TB时钟
	u16_tmp = __raw_readw(CFGCHIP1);
	u16_tmp |= (CFGCHIP1_TBCLKSYNC);
	__raw_writew(u16_tmp, CFGCHIP1);	

	//printk("pwm1b set output on\n");

	return 1;
}

// PWM输出关闭
void fn_pwm_exit(void)
{
	unsigned short u16_tmp;	
	u16_tmp = __raw_readw(CFGCHIP1);
	u16_tmp &= (~CFGCHIP1_TBCLKSYNC);
	__raw_writew(u16_tmp, CFGCHIP1);		
	//printk("pwm1b set output off\n");
}

 
// ioctl 函数的实现 
// 在应用用户层将通过 ioctl 函数向内核传递参数，以控制 LED的输出状态 
static int am1808_led_ioctl( 
 struct inode *inode,  
 struct file *file,   
 unsigned int cmd,  
 unsigned long arg) 
{ 
   	//printk("cmd is %d \n",cmd);
   	switch (cmd)
   	{
      	 	// 用于测试该引脚是否可以使用
      	 	case SET_PWM_PIN_HIGH:
      		 gpio_set_value(PWM1B_NUM,1); 
       		break;
       		case SET_PWM_PIN_LOW:
       		gpio_set_value(PWM1B_NUM,0); 
       		break;
       		case SET_PWM_ON:
		g_u32_pwm_output_freq = arg;
       		fn_pwm_init();
      		 break;
      		 case SET_PWM_OFF:
       		fn_pwm_exit();
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
    	int status;
    	// 配置这个引脚为为EPWM0B功能
    	status = davinci_cfg_reg(DA850_ECAP2_APWM2);
    	if (status < 0) {
		printk("AM1808-PWM pin could not be muxed for GPIO functionality %d\n",DA850_ECAP2_APWM2);
		return status;
    	}
    	// 注册设备 
    	ret = misc_register(&misc); 
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
MODULE_AUTHOR("LSD-ARM team nmy ."); 
