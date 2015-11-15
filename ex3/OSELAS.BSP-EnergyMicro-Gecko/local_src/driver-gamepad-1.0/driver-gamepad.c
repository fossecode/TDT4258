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
#include <linux/irq.h>
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

 #include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
//#include <linux/semaphore.h>
#include <linux/device.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/siginfo.h>

#include "efm32gg.h"

struct siginfo signal_info;
struct task_struct *task;
struct cdev my_cdev;
dev_t my_dev;
struct class *cl;
bool enabled = false;

static irqreturn_t IRQ_HANDLER(int irq, void *dummy, struct pt_regs * regs);
static void setupGPIO();

static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;    

static void memwrite(void *base, uint32_t offset, uint32_t value);
static uint32_t memread(void *base, uint32_t offset);

void __iomem *gpio;


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

void memwrite(void *base, uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *) ((uint32_t) base + offset) = value;
}

uint32_t memread(void *base, uint32_t offset)
{
    return *(volatile uint32_t *) ((uint32_t) base + offset);
}


static irqreturn_t IRQ_HANDLER(int irq, void *dev_id, struct pt_regs * regs)
{
	int direction = 0;
	switch(*GPIO_PC_DIN){
      case 0b11111101:
      	direction = 3;
      	printk("up");
        break;
      case 0b11110111:
      	direction = 1;
      	printk("down");
        break;
	  case 0b11111110:
      	direction = 2;
      	printk("left");
        break;
      case 0b11111011:
      	direction = 0;
      	printk("right");
        break;
      default:
      	direction = 0;
      	break;
    }

	//Set the signal value to the reversed button value(Because they are active low)
	signal_info.si_int = direction;	

    //Resets the interrupt
	//memwrite(gpio, GPIO_IFC, 0xFFFF);
	*GPIO_IFC = *GPIO_IF;

	//Checks if everything is up and running and sends the signal to the game.
    if(enabled)
    {
        int status = send_sig_info(50, &signal_info, task);
        if (status < 0)
        {
            printk("Unable to send interrupt\n");
            return IRQ_NONE;
        }
    }
    else
    {
        printk("Driver not enabled\n");
        return IRQ_NONE;
    }

	return IRQ_HANDLED;
}


/* function to set up GPIO mode and interrupts*/
static void setupGPIO()
{
  /* TODO set input and output pins for the joystick */
    
  /* Example of HW access from C code: turn on joystick LEDs D4-D8
     check efm32gg.h for other useful register definitions
  */
  //*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO; /* enable GPIO clock*/
  *GPIO_PA_CTRL = 2;  /* set high drive strength */
  *GPIO_PA_MODEH = 0x55555555; /* set pins A8-15 as output */
/*  *GPIO_PA_DOUT = 0x0700; turn on LEDs D4-D8 (LEDs are active low) */ 

  *GPIO_PC_MODEL = 0x33333333;
  *GPIO_PC_DOUT = 0xff;

  *GPIO_EXTIPSELL = 0x22222222;
  *GPIO_EXTIFALL  = 0xFF;

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
	alloc_chrdev_region(&my_dev ,0 , 1, "driver-gamepad");
	check_mem_region(GPIO_BASE, GPIO_SIZE);
	request_mem_region(GPIO_BASE, GPIO_SIZE, "driver-gamepad");

	gpio = ioremap_nocache(GPIO_BASE, GPIO_SIZE);
	setupGPIO();

	request_irq(17, IRQ_HANDLER, NULL, "button_click", NULL);
	request_irq(18, IRQ_HANDLER, NULL, "button_click", NULL);

	//Setup signal sending, to trigger interrupts in the game.
	memset(&signal_info, 0, sizeof(struct siginfo));
	signal_info.si_signo = 50;
	signal_info.si_code = SI_QUEUE;
	
	
	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	cdev_add(&my_cdev, my_dev, 1);

	cl = class_create(THIS_MODULE, "driver-gamepad");
	device_create(cl, NULL, my_dev, NULL, "driver-gamepad");

	/**request_region(GPIO_PA_BASE, 0x120, "driver-gamepad");
	*request_region(CMU_BASE2, 0x058, "driver-gamepad");
	*request_region(ISER0, 0x4, "driver-gamepad");*/
	
	printk("Hello world, here is your module speaking\n");

	
	return 0;
}




/* User program opens the driver */
static int open_driver(struct inode *inode, struct file *filp){
	enabled = true;
	return 0;
}

/* User program closes the driver */
static int release_driver(struct inode *inode, struct file *filp){
	return 0;
}

/* User program reads from the driver */
static int read_driver(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	//sprintf(message, direction , buff, 1);
	int error_count = 0;
   	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
   	error_count = copy_to_user(buff, message, strlen("message"));

 
 	return error_count;
}

/* User program writes to the the driver */
static int write_driver(struct file *filp, const char __user *buff, size_t count, loff_t *offp){
	char pid_array[5];
	int pid = 0;
	
	//return if the pid is to big
	if(count > 5)
		return -1;
	
	//Copy userspace data to the buffer
	copy_from_user(pid_array, buff, count);
	sscanf(pid_array, "%d", &pid);
	
	//Locks the RCU while writing
	rcu_read_lock();
	
	//Finds the task based on the PID
	task = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
	if(task == NULL){
		printk("Error: Could not find the task with pid: %d\n", pid);
		rcu_read_unlock();
		return -1;
	}
	rcu_read_unlock();
	printk("write pid ------ %d\n", pid);
	return count;
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

