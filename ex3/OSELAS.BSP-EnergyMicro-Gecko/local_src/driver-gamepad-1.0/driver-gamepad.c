/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include "efm32gg.h"
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/signal.h>

#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/siginfo.h>

struct cdev my_cdev;
dev_t my_dev;
struct class *cl;

static char *direction;
static irqreturn_t IRQ_HANDLER(int irq, void *dummy, struct pt_regs * regs);

static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;    


/* User program opens the driver */
static int open_driver(struct inode *inode, struct file *filp);

/* User program closes the driver */
static int release_driver(struct inode *inode, struct file *filp);

/* User program reads from the driver */
static int read_driver(struct file *filp, char __user *buff, size_t count, loff_t *offp);

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = read_driver,
	.open = open_driver,
	.release = release_driver
};

static irqreturn_t IRQ_HANDLER(int irq, void *dev_id, struct pt_regs * regs)
{
	switch(*GPIO_PC_DIN){
      case 0b11111101:
      	direction = "up";
        break;
      case 0b11110111:
      	direction = "down";
        break;
           case 0b11111110:
      	direction = "left";
        break;
      case 0b11111011:
      	direction = "right";
        break;
      case 0b01111111:
      	direction = "exit";
        break;
    }

	*GPIO_IFC = *GPIO_IF;

	return 0;
}


/* function to set up GPIO mode and interrupts*/
static void setupGPIO()
{
  *GPIO_PC_MODEL = 0x33333333;
  *GPIO_PC_DOUT = 0xff;

  *GPIO_EXTIPSELL = 0x22222222;
  *GPIO_EXTIFALL  = 0xFF;
  *GPIO_EXTIRISE  = 0xFF;

  *GPIO_IEN = 0xff;
  *GPIO_PA_DOUT = 0b1111111100000000;

  *ISER0 |= 0x4000802;


}


/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init template_init(void)
{

	request_irq(17, IRQ_HANDLER, NULL, "button_click", NULL);
	request_irq(18, IRQ_HANDLER, NULL, "button_click", NULL);
	
	alloc_chrdev_region(&my_dev ,0 , 1, "driver-gamepad");
	cdev_init(&my_cdev, &my_fops);

	cl = class_create(THIS_MODULE, "driver-gamepad");
	device_create(cl, NULL, my_dev, NULL, "driver-gamepad");

	*request_region(GPIO_PC_BASE, 0xFC, "driver-gamepad");
	*request_region(ISER0, 0x4, "driver-gamepad");

	setupGPIO();

	cdev_add(&my_cdev, my_dev, 1);
	printk("Hello world, here is your module speaking\n");

	return 0;
}




/* User program opens the driver */
static int open_driver(struct inode *inode, struct file *filp){
	direction = "none";
	return 0;
}

/* User program closes the driver */
static int release_driver(struct inode *inode, struct file *filp){
	return 0;
}

/* User program reads from the driver */
static int read_driver(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	sprintf(message, direction , buff, 1);
	int error_count = 0;
   	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
   	error_count = copy_to_user(buff, message, strlen("message"));
 	direction = "none";
 	return error_count;
}



/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	unregister_chrdev_region(my_dev ,1);
	release_region(GPIO_PC_BASE, 0xFC);
	release_region(ISER0, 0x4);
	free_irq(17, NULL);
	free_irq(18, NULL);
	printk("Short life for a small module...\n");
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

