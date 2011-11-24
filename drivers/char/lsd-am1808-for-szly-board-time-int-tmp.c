/**
 * @file lsddm-amr1808-b4-time-int.c
 * @brief time int driver
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
 * @date 2011-7-5 16:12:55
 * @version 0.01
 *
 * This file is pary of drivers for LSDDM-AMR1808-B4 board.
 */

/**
 * @note
 *  \n
 *  
 * \b Change \b Logs: \n
 * 2011.07.05   ������ʼ by toby \n
 * 
 */

/**
 * @addtogroup api_lsddm-amr1808-b4-time_int
 * @{
 */

// TODO: Add your code here
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/delay.h> 
#include <linux/poll.h> 
#include <linux/irq.h> 
#include <asm/irq.h> 
#include <linux/interrupt.h> 
#include <asm/uaccess.h> 
#include <mach/hardware.h> 
#include <linux/platform_device.h> 
#include <linux/cdev.h> 
#include <linux/miscdevice.h> 
#include <linux/sched.h> 
#include <linux/gpio.h> 

#include <linux/module.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <mach/cputype.h>
#include <mach/hardware.h>
#include <mach/mux.h>
#include <asm/gpio.h>

#define DA850_TIMER0_BASE			0x01C20000 
#define DA850_TIMER1_BASE			0x01C21000 
#define DA850_TIMER2_BASE			0x01F0C000
#define DA850_TIMER3_BASE			0x01F0D000
#define DA850_TIMER_USE   DA850_TIMER0_BASE
#define DA850_TIMER_USE_IRQ  IRQ_DA8XX_TINT12_1

// GET from TI BSL code
//----------------------------------------------------------------------------
// datatypes
//----------------------------------------------------------------------------
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   ulong_t;
typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef signed long     long_t;

typedef enum 
{
	false = 0,
	true = 1
}bool_e; 
//-----------------------------------------------------------------------------
// bit manipulation macros and bitval lookup table declarations.
//-----------------------------------------------------------------------------
#define SETBIT(dest,mask)     (dest |= mask)
#define CLRBIT(dest,mask)     (dest &= ~mask)
#define TGLBIT(dest,mask)     (dest ^= mask)
#define CHKBIT(dest,mask)     (dest & mask)
//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
    volatile uint32_t REV;             // 0x0000
    volatile uint32_t EMUMGT;          // 0x0004
    volatile uint32_t GPINT_GPEN;      // 0x0008
    volatile uint32_t GPDATA_GPDIR;    // 0x000C
    volatile uint32_t TIM12;           // 0x0010
    volatile uint32_t TIM34;           // 0x0014
    volatile uint32_t PRD12;           // 0x0018
    volatile uint32_t PRD34;           // 0x001C
    volatile uint32_t TCR;             // 0x0020
    volatile uint32_t TGCR;            // 0x0024
    volatile uint32_t WDTCR;           // 0x0028
    volatile uint32_t RSVD0[2];        // 0x002C
    volatile uint32_t REL12;           // 0x0034
    volatile uint32_t REL34;           // 0x0038
    volatile uint32_t CAP12;           // 0x003C
    volatile uint32_t CAP34;           // 0x0040
    volatile uint32_t INTCTLSTAT;      // 0x0044
    volatile uint32_t RSVD1[6];        // 0x0048
    volatile uint32_t CMP0;            // 0x0060
    volatile uint32_t CMP1;            // 0x0064
    volatile uint32_t CMP2;            // 0x0068
    volatile uint32_t CMP3;            // 0x006C
    volatile uint32_t CMP4;            // 0x0070
    volatile uint32_t CMP5;            // 0x0074
    volatile uint32_t CMP6;            // 0x0078
    volatile uint32_t CMP7;            // 0x007C
} timer_regs_t;

// define all the available timer peripherals for the processor.
#define TMR0            ((timer_regs_t *)DA850_TIMER0_BASE)
#define TMR1            ((timer_regs_t *)DA850_TIMER1_BASE)

// bitmask defines for GPINT_GPEN.
#define GPENO34         (0x02000000)   // bit 25
#define GPENI34         (0x01000000)   // bit 24
#define GPENO12         (0x00020000)   // bit 17
#define GPENI12         (0x00010000)   // bit 16
#define GPINT34INVO     (0x00002000)   // bit 13
#define GPINT34INVI     (0x00001000)   // bit 12
#define GPINT34ENO      (0x00000200)   // bit 9
#define GPINT34ENI      (0x00000100)   // bit 8
#define GPINT12INVO     (0x00000020)   // bit 5
#define GPINT12INVI     (0x00000010)   // bit 4
#define GPINT12ENO      (0x00000002)   // bit 1
#define GPINT12ENI      (0x00000001)   // bit 0

// bitmask defines for GPDATA_GPDIR.
#define GPDIRO34        (0x02000000)   // bit 25
#define GPDIRI34        (0x01000000)   // bit 24
#define GPDIRO12        (0x00020000)   // bit 17
#define GPDIRI12        (0x00010000)   // bit 16
#define GPDATAO34       (0x00000200)   // bit 9
#define GPDATAI34       (0x00000100)   // bit 8
#define GPDATAO12       (0x00000002)   // bit 1
#define GPDATAI12       (0x00000001)   // bit 0

// bitmask defines for TCR.
#define ENAMODE34                (0x00C00000)   // bit 22,23
#define ENAMODE34_ONETIME        (0x00400000)   // bit 22
#define ENAMODE34_CONT           (0x00800000)   // bit 23
#define ENAMODE34_CONT_RELOAD    (0x00C00000)   // bit 22,23
#define ENAMODE12                (0x000000C0)   // bit 6,7
#define ENAMODE12_ONETIME        (0x00000040)   // bit 6
#define ENAMODE12_CONT           (0x00000080)   // bit 7
#define ENAMODE12_CONT_RELOAD    (0x000000C0)   // bit 6,7

// bitmask defines for TGCR.
#define PRESCALER(n)             ((n) << 8)
#define PLUSEN                   (0x00000010)   // bit 4
#define TIMMODE_64BIT            (0x0000000C)   // bit 2,3
#define TIMMODE_32BIT_UNCHAINED  (0x00000004)   // bit 2
#define TIMMODE_64BIT_WDOG       (0x00000008)   // bit 3
#define TIMMODE_32BIT_CHAINED    (0x0000000C)   // bit 2,3
#define TIM34RS                  (0x00000002)   // bit 1
#define TIM12RS                  (0x00000001)   // bit 0

// bitmask defines for INTCTLSTAT.
#define PRDINTSTAT34             (0x00020000)   // bit 17
#define PRDINTEN34               (0x00010000)   // bit 16
#define PRDINTSTAT12             (0x00000002)   // bit 1
#define PRDINTEN12               (0x00000001)   // bit 0

#if 0
//-----------------------------------------------------------------------------
// \brief   initialize the microsecond timer.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...us timer ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t USTIMER_init(void)
{
	// configure timer 0 for free run.
	// 32-bit unchained mode, timer3:4 /12 prescaler.

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR0->GPINT_GPEN = GPENO12 | GPENI12;
   TMR0->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR0->TGCR = 0x00000000;
   TMR0->TCR = 0x00000000;

   // disable interrupts and set emulation to free run.
   TMR0->INTCTLSTAT = 0;
   SETBIT(TMR0->EMUMGT, SOFT | FREE);

   // config timer0 in 32-bit unchained mode.
   // remove timer0 - 3:4 from reset.
   SETBIT(TMR0->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED | TIM34RS );

   // init timer0 - 1:2 period....use full range of counter.
   TMR0->TIM34 = 0x00000000;
   TMR0->PRD34 = 0xFFFFFFFF;

   // start timer0 - 3:4.
   SETBIT(TMR0->TCR, ENAMODE34_CONT);

   // configure timer 1 for start/stop.
	// 32-bit unchained mode, timer3:4 /12 prescaler.

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR1->GPINT_GPEN = GPENO12 | GPENI12;
   TMR1->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR1->TGCR = 0x00000000;
   TMR1->TCR = 0x00000000;

   // disable interrupts and set emulation to free run.
   TMR1->INTCTLSTAT = 0;
   SETBIT(TMR1->EMUMGT, SOFT | FREE);

   // config timer1 in 32-bit unchained mode.
   SETBIT(TMR1->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED);

   // init timer1 - 3:4 period....0 until someone calls delay().
   TMR1->TIM34 = 0x00000000;
   TMR1->PRD34 = 0x00000000;
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   halts execution for "in_delay" number of microseconds.
//
// \param   uint32_t in_delay - the number of us to delay.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_delay(uint32_t in_delay)
{
   // stop the timer, clear int stat, and clear timer value.
   CLRBIT(TMR1->TGCR, TIM34RS);
   TMR1->TCR = 0x00000000;
   SETBIT(TMR1->INTCTLSTAT, PRDINTSTAT34);
   TMR1->TIM34 = 0x00000000;

   // setup compare time.
   // NOTE: not checking for possible rollover here...do not pass in a
   // value > 0x7FFFFFFF....would result in a much shorter delay than expected.
   TMR1->PRD34 = TICKS_PER_US * in_delay;
   
   // start timer1 - 3:4 to run once up to the period.
   SETBIT(TMR1->TCR, ENAMODE34_ONETIME);
   SETBIT(TMR1->TGCR, TIM34RS);
   
   // wait for the signal that we have hit our period.
   while (!CHKBIT(TMR1->INTCTLSTAT, PRDINTSTAT34))
   {
      asm("nop");
   }
//   uint32_t i;
//   for (i = 0; i < in_delay * 1; i++) {}
}

//-----------------------------------------------------------------------------
// \brief   resets the timer to 0.
//
// \param   none.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_reset(void)
{
   CLRBIT(TMR0->TGCR, TIM34RS);
   TMR0->TIM34 = 0x00000000;
   SETBIT(TMR0->TGCR, TIM34RS);
}

//-----------------------------------------------------------------------------
// \brief   returns the current timer value, in microseconds.
//
// \param   none.
//
// \return  uint32_t - timer value, in us.
//-----------------------------------------------------------------------------
uint32_t USTIMER_get(void)
{
   return (TMR0->TIM34 / TICKS_PER_US);
}

//-----------------------------------------------------------------------------
// \brief   sets the timer value, in microseconds.
//
// \param   uint32_t in_time - value to set the timer, in us.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_set(uint32_t in_time)
{
   TMR0->TIM34 = TICKS_PER_US * in_time;
}
#endif

unsigned long fn_time0_init_mode1(void)
{
	// configure timer 0 for free run. ���ö�ʱ��0 ��������
	// 32-bit unchained mode, timer3:4 /12 prescaler.  32λ������ģʽ ˫32λ��ʱ��  ��ʱ��T0M34��12��Ƶ

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR0->GPINT_GPEN = GPENO12 | GPENI12; // ����
   TMR0->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR0->TGCR = 0x00000000;  // ��ʱ�� ȫ�ֿ��ƼĴ�����0
   TMR0->TCR = 0x00000000;   // ��ʱ�� ���ƼĴ�����0

   // disable interrupts and set emulation to free run.
   TMR0->INTCTLSTAT = 0;  //�����ֹ�����ж�
   SETBIT(TMR0->EMUMGT, SOFT | FREE); // �趨���������

   // config timer0 in 32-bit unchained mode.
   // remove timer0 - 3:4 from reset.
   SETBIT(TMR0->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED | TIM34RS );

   // init timer0 - 1:2 period....use full range of counter.
   TMR0->TIM34 = 0x00000000;   // ��ʱ����ʼֵ
   TMR0->PRD34 = 0xFFFFFFFF;   // ����ֵ

   // start timer0 - 3:4.   
   SETBIT(TMR0->TCR, ENAMODE34_CONT);  // ������ʱ��

   // configure timer 1 for start/stop.
	// 32-bit unchained mode, timer3:4 /12 prescaler.

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR1->GPINT_GPEN = GPENO12 | GPENI12;
   TMR1->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR1->TGCR = 0x00000000;
   TMR1->TCR = 0x00000000;

   // disable interrupts and set emulation to free run.
   TMR1->INTCTLSTAT = 0;
   SETBIT(TMR1->EMUMGT, SOFT | FREE);

   // config timer1 in 32-bit unchained mode.
   SETBIT(TMR1->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED);

   // init timer1 - 3:4 period....0 until someone calls delay().
   TMR1->TIM34 = 0x00000000;
   TMR1->PRD34 = 0x00000000;
   
   return (ERR_NO_ERROR);
}


/*  
  �豸��(/dev/time_int)
*/
#define DEVICE_NAME "time_int" 
#define NO_USE   0

static int fd_open_flag = 0;


/*
 * ��Ϊ�������ǻ����жϷ�ʽ�ģ��ڴ˴���һ���ȴ����У�������жϺ���ʹ�ã����а������²���ȡ����
 * ֵʱ�����ỽ�Ѵ˶��У��������жϱ�־���Ա���ͨ��read()�����жϺͶ�ȡ��ֵ���ݵ��û�̬����û�а�
 * ������ʱ��ϵͳ��������ѯ����״̬���Խ�ʡʱ����Դ 
 */
static DECLARE_WAIT_QUEUE_HEAD(time_int_waitq); // �ȴ�����

/*
 * �жϱ�ʶ�������������Ķ���ʹ�ã��жϷ��������������Ϊ1��read()������������� 
 */
static volatile int ev_press = 0; 
 
/*
 * �������������жϷ������
 */
static irqreturn_t time_int_interrupt(int irq, void *dev_id) 
{ 
    struct button_irq_desc *button_irqs = (struct button_irq_desc *)dev_id; 
    int is_down;

    // udelay(0); 
    // ��ȡ�����µİ���״̬
    if(gpio_get_value(button_irqs->pin) == 0)
    { 
        // ����������
        is_down = 1;
    }
    else
    {
        // ����δ����
        is_down = 0;
    }

    // ״̬�ı䣬���������£��������Կ�����������û�б����µ�ʱ�򣬼Ĵ�����ֵΪ1(����)������
    // �������µ�ʱ�򣬼Ĵ�����Ӧ��ֵΪ0 
    if(is_down != (button_values[button_irqs->number] & 1))
    {
        // ������������£���button_values[0]�ͱ�Ϊ��1������Ӧ�� ASCII ��Ϊ 31 
        button_values[button_irqs->number] = '0' + is_down; 

        ev_press = 1; // �����жϱ�־Ϊ1 
        wake_up_interruptible(&time_int_waitq); // ���ѵȴ�����
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

/*
 * ��Ӧ�ó���ִ�� open(��/dev/time_int��,��)ʱ����õ��˺��������������������Ҫ��ע��4���������жϡ�
 * ���õ��ж�������IRQ_TYPE_EDGE_BOTH��Ҳ����˫�ش������������غ��½��ؾ�������жϣ�������
 * ��Ϊ�˸�����Ч���жϰ���״̬
 */
static int lsddm_amr1808_b4_time_int_open(struct inode *inode, struct file *file) 
{ 
    int err = 0; 
    int status;

	
    // ע���жϺ��� 
    err = request_irq(IRQ_DA8XX_TINT12_1, time_int_interrupt, IRQ_TYPE_EDGE_BOTH,
       "time-out", NULL); 
    if (err)
	{
		printk("time-int:request_irq false,err = %d\n",err);
		//return -1;
	} 
	else
	{
		printk("time-int:request_irq ok = %d\n",err);
		
	}
  
    if(err)
    { 
        disable_irq(DA850_TIMER_USE_IRQ); 
        free_irq(DA850_TIMER_USE_IRQ, NULL);
		printk("time-int:free_irq\n");
        return -EBUSY; 
	}    
    // �������� 
    return 0; 
} 
 
/*
 * �˺�����ӦӦ�ó����ϵͳ����close(fd)������ �ڴ�������Ҫ�����ǵ��ر��豸ʱ�ͷ�4���������ж�
 * ������
 */
static int lsddm_amr1808_b4_time_int_close(struct inode *inode, struct file *file) 
{ 
	disable_irq(DA850_TIMER_USE_IRQ); 
    free_irq(DA850_TIMER_USE_IRQ, NULL);
	printk("time-int:close time-int\n");
    return 0; 
} 
 
#if 0
/*
 * ��ӦӦ�ó����read(fd,��)��������Ҫ�������û��ռ䴫�ݼ�ֵ
 */
static int lsddm_amr1808_b4_time_int_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) 
{ 
    unsigned long err;

    if(!ev_press)
    { 
        if(filp->f_flags & O_NONBLOCK) 
            // ���жϱ�ʶΪ 0 ʱ�����Ҹ��豸���Է�������ʽ��ʱ������ 
            return -EAGAIN; 
        else 
            // ���жϱ�ʶΪ 0 ʱ�����Ҹ��豸����������ʽ��ʱ����������״̬���ȴ������� 
            wait_event_interruptible(time_int_waitq, ev_press);
    } 

    // ���жϱ�ʶ���� 
    ev_press = 0; 

    // һ���ֵ�����ݵ��û��ռ� 
    err = copy_to_user(buff, (const void *)button_values, min(sizeof(button_values), count)); 

    return err ? -EFAULT : min(sizeof(button_values), count); 
} 

static unsigned int lsddm_amr1808_b4_time_int_poll( struct file *file, struct poll_table_struct *wait) 
{ 
    unsigned int mask = 0; 
    
    // �ѵ���poll���� select �Ľ��̹�����У��Ա㱻����������
    poll_wait(file, &time_int_waitq, wait); 

    if(ev_press)
        mask |= POLLIN | POLLRDNORM; 

    return mask; 
} 
#endif

// �豸������ 
static struct file_operations dev_fops = { 
    .owner   =   THIS_MODULE, 
    .open    =   lsddm_amr1808_b4_time_int_open, 
    .release =   lsddm_amr1808_b4_time_int_close,  
    //.read    =   lsddm_amr1808_b4_time_int_read, 
    //.poll    =   lsddm_amr1808_b4_time_int_poll, 
}; 
 
static struct miscdevice misc = { 
    .minor = MISC_DYNAMIC_MINOR,
    .name =  DEVICE_NAME,
    .fops =  &dev_fops,
}; 

static int __init lsddm_amr1808_b4_time_int_init(void) 
{ 
    int ret; 

    // �Ѱ����豸ע��Ϊ misc �豸�����豸�����Զ������ 
    ret = misc_register(&misc); 
    //printk (DEVICE_NAME"\tinitialized\n"); 

    return ret; 
} 

static void __exit lsddm_amr1808_b4_time_int_exit(void) 
{ 
#if 0    
	if(fd_open_flag != 0)
    {
      int i; 
     
      for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) { 

         gpio_free(button_irqs[i].pin);
         // �ͷ��жϺţ���ע���жϴ����� 
         free_irq(gpio_to_irq(button_irqs[i].pin), (void *)&button_irqs[i]); 
      } 
    }
#endif
    misc_deregister(&misc); 
} 

module_init(lsddm_amr1808_b4_time_int_init); 
module_exit(lsddm_amr1808_b4_time_int_exit);

MODULE_AUTHOR("yuge");
MODULE_DESCRIPTION("LSDDM-AMR1808-B4 time Driver");
MODULE_LICENSE("GPL");

/**
 * Close the Doxygen group.
 * @}
 */

