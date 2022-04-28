// JOY Adeyemo 
// Lab 1, Part 1 
// 3/4/2022
#include <asm/io.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/kobject.h>    // Using kobjects for the sysfs bindings
#include <linux/kthread.h>    // Using kthreads for the flashing functionality
#include <linux/delay.h>      // Using this header for the msleep() function


MODULE_LICENSE("GPL");

pin2: 0x00000004;
pin3: 0x00000008;
pin4: 0x00000010;
pin5: 0x00000020;


static int __init lkm_example_init(void) 
{

//get pointer to the registers (IOREMAP)
  unsigned long* GPFSEL0 = (unsigned long*) ioremap(0x00000004, 2);
  if (GPFSEL0 == NULL) 
   {
	return 2;
   }

  unsigned long* GPFSET0 = GPFSEL0 + 0x1C / 4;
  unsigned long* GPCLR0 = GPFSEL0 + 0x28 / 4;

//set the pin mode for one or all LEDS (GPFSEL)
//first pin
   iowrite32(0, GPFSEL0);
   iowrite32(pin2, GPFSET0);
   iowrite32(pin2, GPFSET0);


   //clear the pins
   iowrite32(pin2, GPCLR0);
   
   // set pin 3, pin 4, pin 5
   iowrite32(pin3, GPSET0); //setting pin3 to high 
   iowrite32(pin4, GPSET0); //pin 4
   iowrite32(pin5, GPSET0); // pin 5
	

   printk(KERN_INFO "Module installed! -- New style\n");
   return 0;
}

static void __exit lkm_example_exit(void) 
{

  //SET led from low to high 
    unsigned long * base = (unsigned long *) ioremap(0x3F200000, 4096);
    unsigned long * GPCLR0 = base + 0x28/4;
    iowrite32(0x0000003C, GPCLR0);

    printk(KERN_INFO "Module removed! -- New style\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);

//base address is 0x3F200000
sudo insmod Name.ko
sudo rmmod Name
dmesg




