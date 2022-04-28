/** @authors: Luis Alberto Rivera & Tushar Nakini
 *  @group: ViGIR
 *  
 *  @brief
 *
 *  Basic steps needed to configure GPIO interrupt and attach a handler.
 *  Check chapter 6 of the BCM2837 ARM Peripherals manual for details about
 *  the GPIO registers, how to configure, set, clear, etc.
 *  
 *  You can compile your module using the same Makefile provided. Just add
 *  obj-m += YourModuleName.o
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

int mydev_id;	// variable needed to identify the handler

// Interrupt handler function. It is dummy ISR and we don't need to clead the
// GPEDS register as we are going to do that in the user-space program which is
// Lab3_W2.c
static irqreturn_t button_isr(int irq, void *dev_id)
{
	// Step-1
	// ------
	// In general, you want to disable the interrupt while handling it.
    	disable_irq_nosync(79);

	// Step-2
	// ------
	// Reenable the interrupt.
	enable_irq(79);

	//printk("Interrupt handled\n");
	return IRQ_HANDLED;
}

int init_module(){
	// Step-1
	// ------
	// Set the push button IO as input.
	// point to the GPSEL register. Write to GPSEL to set all LED's to outputs
	unsigned int *gpfsel0 = ioremap(0x3F200000, 1024); 
	iowrite32(0x00009240, gpfsel0); 

	unsigned int *gpfset0 = gpfsel0+1;
	iowrite32(0x0, gpfset0); 		//pins 16-19 bcm pushbutton's
	unsigned int *gpfsel1 = gpfsel0+2;
	iowrite32(0x0, gpfsel1); 		//pin 20 bcm pushbutton
	
	// Step-2
	// ------
	// Configure the push buttons to trigger an interrupt.
	//
	// You need to configure the pull-downs for all those ports. There is
	// a specific sequence that needs to be followed to configure those pull-downs.
	// The sequence is described on page 101 of the BCM2837-ARM-Peripherals manual.
	// You can use  udelay(100);  for those 150 cycles mentioned in the manual.
	// It's not exactly 150 cycles, but it gives the necessary delay.
	// WiringPi makes it a lot simpler in user space, but it's good to know
	// how to do this "by hand".
		
	//point to the GPPUD0 register and (GPIO Pull-up/pull-down)
	/*unsigned int *gppud0 = gpfsel0+37; 
	iowrite32(0x01, gppud0); 		// 0b01 : 'Pull Down'
	unsigned int *gppudclk = gpfsel0+38;
	udelay(100); 				// Close to 150 cycles delay
	iowrite32(0x1F0000, gppudclk); 		//assert clock on all 5 push buttons 
	udelay(100);
	iowrite32(0x00000000, gppud0); 		// 0b00 : Diable pull-up/pull-down
	iowrite32(0x001F0000, gppudclk); 	//assert clock on all 5 push button*/

	// Step-3
	// ------
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
	unsigned int *gparen0 = gpfsel0+31;
	iowrite32(0x001F0000, gparen0); 	// Enable on pin 16-20

	// Step-4
	// ------
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
    	int dummy = 0;
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button Handler", &mydev_id);
	/*if (dummy < 0){
		printk("Request for line 79 FAILED!!!");
	}else {
		printk("ISR Module Installed!!!");
	}*/

	// Step-5
	// ------
	// Now enable the IRQ so that hardware won't ignore the interrupt
	enable_irq(79);
	
	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module(){
	// Step-1
	// ------
	// Good idea to clear the Event Detect status register here, just in case.
	// Also, disable (Async) Rising Edge detection for all 5 GPIO ports.
    unsigned int *gpeds0 = ioremap(0x3F200040, 1024); // point to the GPEDS register
	unsigned int *gparen0 = gpeds0+15; 	// point to the GPAREN0 register 

	iowrite32(0x001F0000, gpeds0); 		// clearing GPEDS
	iowrite32(0x00000000, gparen0); 	// disable GPAREN0

	// Step-2
	// ------
	// Remove the interrupt handler; you need to provide the same identifier
	free_irq(79, &mydev_id);

	printk("Button Detection disabled.\n");
}
