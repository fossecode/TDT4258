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

struct cdev my_cdev;
dev_t my_dev;
struct class *cl;


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
	
	alloc_chrdev_region(&my_dev ,0 , 1, "driver-gamepad");

	cdev_init(&my_cdev, &my_fops);

	cl = class_create(THIS_MODULE, "driver-gamepad");
	device_create(cl, NULL, my_dev, NULL, "driver-gamepad");
	*request_region(GPIO_PA_BASE, 0x120, "driver-gamepad");


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
	printk("yolo");
	return 0;
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
	release_region(GPIO_PA_BASE, 0x120);
	printk("Short life for a small module...\n");
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

