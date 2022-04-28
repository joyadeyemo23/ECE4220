#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/kobject.h>    // Using kobjects for the sysfs bindings
#include <linux/kthread.h>    // Using kthreads for the flashing functionality
#include <linux/delay.h>      // Using this header for the msleep() function

MODULE_LICENSE("GPL");

int init_module() {
    printk(KERN_INFO "Module installed! -- Old style\n");
    return 0;
}

void cleanup_module() {
    printk(KERN_INFO "Module removed! -- Old style\n");
}

module_init(init_module)
module_exit(cleanup_module)


void iowrite32(u32 value, void * addr)
{
		
}



