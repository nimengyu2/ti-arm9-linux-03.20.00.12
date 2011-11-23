/**
 * @file lsddm-amr1808-b4-leds.c
 * @brief leds driver
 *
 * Copyright (C) 2001-2011, LSD Science & Technology Co.,Ltd
 * All rights reserved.
 * Software License Agreement
 *
 * LSD Science & Technology (LSD) is supplying this software for use solely
 * its suppliers, and is protected. You may not combine this software with
 * "viral" open software in order to form a larger program.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. LSD SHALL NOT, UNDER ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * @author toby.zhang <zxj@lierda.com>
 * @date 2011-7-5 13:10:58
 * @version 0.01
 *
 * This file is pary of drivers for LSDDM-AMR1808-B4 board.
 */

/**
 * @note
 *  \n
 *  
 * \b Change \b Logs: \n
 * 2011.07.05   创建初始 by toby \n
 * 
 */

/**
 * @addtogroup api_lsddm-amr1808-b4-leds
 * @{
 */

// TODO: Add your code here
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
#include <mach/mux.h>
#include <asm/gpio.h>

/*  
  设备名(/dev/leds)
*/
#define DEVICE_NAME "leds" 

#define MOTOR_MAGIC 'L'
#define SET_LED2 _IOW(MOTOR_MAGIC, 2,int)
#define SET_LED3 _IOW(MOTOR_MAGIC, 3,int)
#define SET_LED4 _IOW(MOTOR_MAGIC, 4,int)
#define SET_LED5 _IOW(MOTOR_MAGIC, 5,int)

/*
    LEDS GPGIO MAP:
    DS2 GPIO8[15] --> gpio_num[0] gpio_pin[0]
    DS3 GPIO8[14] --> gpio_num[1] gpio_pin[1]
    DS4 GPIO8[13] --> gpio_num[2] gpio_pin[2]
    DS5 GPIO8[12] --> gpio_num[3] gpio_pin[3]
*/
static int gpio_num[4];
static int gpio_pin[4];
 
static int
am1808_leds_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    int gpio_num_tmp;

    switch (cmd)
    {
        case SET_LED2:
        gpio_num_tmp = gpio_num[0];
        break;

        case SET_LED3:
        gpio_num_tmp = gpio_num[1];
        break;

        case SET_LED4:
        gpio_num_tmp = gpio_num[2];
        break;

        case SET_LED5:
        gpio_num_tmp = gpio_num[3];
        break;       

        default:
        return -EINVAL; 
    } 

    if(arg == 0)
    {
        // Set the LED GPIO to LOW
        gpio_set_value(gpio_num_tmp, 0); 
    }
    else if(arg == 1)
    {
        // Set the LED GPIO to HIGH
        gpio_set_value(gpio_num_tmp, 1); 
    }
    else
    {
        printk("arg must be 0 or 1\n");
    }

    return 0;       
} 
 
/* 
  设备函数操作集，在此只有 ioctl函数，通常还有 read, write,  
  注册为 misc 设备，因此也可以不用 open/close
*/
static struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .ioctl = am1808_leds_ioctl,
}; 

/* 
  把LED驱动注册为MISC设备 
*/
static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR, // 动态设备号
    .name = DEVICE_NAME, // 设备名称 
    .fops = &dev_fops, // 设备文件操作集 
}; 

static int __init lsddm_amr1808_b4_leds_init(void)
{
    int ret; 
    int i;  
    int status;

    gpio_num[0] = 8*16 + 15; // 引脚号
    gpio_pin[0] = DA850_GPIO8_15; // 引脚索引值

    gpio_num[1] = 8*16 + 14;
    gpio_pin[1] = DA850_GPIO8_14;

    gpio_num[2] = 8*16 + 13;
    gpio_pin[2] = DA850_GPIO8_13;

    gpio_num[3] = 8*16 + 12;
    gpio_pin[3] = DA850_GPIO8_12;

    for(i=0; i<4; i++)
    {
        // init/set pinmux 
        // 设置引脚为GPIO功能
        status = davinci_cfg_reg(gpio_pin[i]);
        if (status < 0)
        {
            printk("pin could not be muxed for GPIO functionality %d\n", 
                    gpio_num[i]);
            return status;
        }
    
        // 引脚是否已经配置成GPIO功能
        status = gpio_request(gpio_num[i], "gpio_test\n");
        if (status < 0)
        {
            printk("ERROR can not open GPIO %d\n", gpio_num[i]);
            return status;
        }
        
        // 使能GPIO時钟
        gpio_direction_output(gpio_num[i], 0);
        
        // 设置GPIO默认值为低点平
        gpio_set_value(gpio_num[i], 1);
    }    
     
    // misc设备注册
    ret = misc_register(&misc);
    //printk (DEVICE_NAME"\tinitialized\n");
    
    return ret; 
} 
 
static void __exit lsddm_amr1808_b4_leds_exit(void) 
{ 
    
    // 释放GPIO引脚功能
    gpio_set_value(gpio_num[0], 1);
    gpio_set_value(gpio_num[1], 1);
    gpio_set_value(gpio_num[2], 1);
    gpio_set_value(gpio_num[3], 1);
    gpio_free(gpio_num[0]);
    gpio_free(gpio_num[1]);
    gpio_free(gpio_num[2]);
    gpio_free(gpio_num[3]);

    // 注销设备
    misc_deregister(&misc); 
}

module_init(lsddm_amr1808_b4_leds_init); 
module_exit(lsddm_amr1808_b4_leds_exit);

MODULE_AUTHOR("Toby Zhang");
MODULE_DESCRIPTION("LSDDM-AMR1808-B4 LEDs Driver");
MODULE_LICENSE("GPL");

/**
 * Close the Doxygen group.
 * @}
 */
