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

struct cdev my_cdev;
dev_t my_dev;
struct class *cl;

#define GPIO_IF        ((volatile uint32_t*)(GPIO_PA_BASE + 0x114))


/* User program opens the driver */
static int open_driver(struct inode *inode, struct file *filp);

/* User program closes the driver */
static int release_driver(struct inode *inode, struct file *filp);

/* User program reads from the driver */
static int read_driver(struct file *filp, char __user *buff, size_t count, loff_t *offp);

/* User program writes to the the driver */
static int write_driver(struct file *filp, const char __user *buff, size_t count, loff_t *offp);

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = read_driver,
	.write = write_driver,
	.open = open_driver,
	.release = release_driver
};

static irqreturn_t ODD_IRQ_HANDLER(int irq, void *dev_id, struct pt_regs *regs){
	printk("ODD");

	*GPIO_IFC = *GPIO_IF;
	return 0;
}

static irqreturn_t EVEN_IRQ_HANDLER(int irq, void *dev_id, struct pt_regs *regs){
	printk("EVEN");

	*GPIO_IFC = *GPIO_IF;
	return 0;
}

/* function to set up GPIO mode and interrupts*/
static void setupGPIO()
{
  /* TODO set input and output pins for the joystick */
    
  /* Example of HW access from C code: turn on joystick LEDs D4-D8
     check efm32gg.h for other useful register definitions
  */
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO; /* enable GPIO clock*/
  *GPIO_PA_CTRL = 2;  /* set high drive strength */
  *GPIO_PA_MODEH = 0x55555555; /* set pins A8-15 as output */
/*  *GPIO_PA_DOUT = 0x0700; turn on LEDs D4-D8 (LEDs are active low) */ 

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

	request_irq(17, EVEN_IRQ_HANDLER, NULL, "gpio_even_handler", NULL);
	request_irq(18, ODD_IRQ_HANDLER, NULL, "gpio_odd_handler", NULL);
	
	alloc_chrdev_region(&my_dev ,0 , 1, "driver-gamepad");
	cdev_init(&my_cdev, &my_fops);

	cl = class_create(THIS_MODULE, "driver-gamepad");
	device_create(cl, NULL, my_dev, NULL, "driver-gamepad");

	*request_region(GPIO_PA_BASE, 0x120, "driver-gamepad");
	*request_region(CMU_BASE2, 0x058, "driver-gamepad");
	*request_region(ISER0, 0x4, "driver-gamepad");

	setupGPIO();

	cdev_add(&my_cdev, my_dev, 1);
	printk("Hello world, here is your module speaking\n");
	return 0;
}




/* User program opens the driver */
static int open_driver(struct inode *inode, struct file *filp){
	return 0;
}

/* User program closes the driver */
static int release_driver(struct inode *inode, struct file *filp){
	return 0;
}

/* User program reads from the driver */
static int read_driver(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	return GPIO_PC_DIN;
}

/* User program writes to the the driver */
static int write_driver(struct file *filp, const char __user *buff, size_t count, loff_t *offp){
	return 0;
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
	//release_region(GPIO_PA_BASE, 0x120);
	free_irq(17, NULL);
	free_irq(18, NULL);
	printk("Short life for a small module...\n");
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

