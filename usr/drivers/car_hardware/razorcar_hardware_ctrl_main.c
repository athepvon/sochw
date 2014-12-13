#include <linux/moduleparam.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <asm/system.h>		/* cli(), *_flags */
#include <linux/mutex.h> 
#include <linux/spinlock.h>
#include <linux/delay.h>	/* udelay */
#include <linux/kdev_t.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <asm/cacheflush.h>
#include <linux/semaphore.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <asm/cache.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include "razorcar_hardware.h"

typedef struct page mem_map_t;
#ifndef NO_IRQ
#define NO_IRQ 0
#endif
// akzare start
#if defined(CONFIG_OF)
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_device.h> 
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
/*#define flush_cache_all()		\
do {									\
	invalidate_dcache();					\
	invalidate_icache();					\
} while (0)*/
/* Match table for of_platform binding */
static struct of_device_id razorcar_hardware_of_match[]  = {
	{ .compatible = "xlnx,razorcar-hardware-1.01.a", },
	{}
};
MODULE_DEVICE_TABLE(of, razorcar_hardware_of_match);
#endif
#ifndef VM_RESERVED
//#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#define VM_RESERVED VM_DONTEXPAND
#endif
MODULE_AUTHOR(" Michael");
MODULE_DESCRIPTION("Generic device driver for razorcar");
MODULE_LICENSE("Dual BSD/GPL");
#define MODULE_NAME             "razorcar_hardware"
//#define MINOR         0
// akzare start
static int major = 232;	/* dynamic by default */
module_param(major, int, 0);
razorcar_hw_regs_t *hw_regs = (razorcar_hw_regs_t *)NULL;

//DEFINE_MUTEX(mutex);
struct mutex mutex;
int counter=0;
int razorcar_hardware_reg_num;
razorcar_hw_regs_t* mem_base_addresses[2];
/*
unsigned long do_io_read16 (void *address)
{
        unsigned long ret = 0;    
        ret = ioread16(address);
	rmb();
	return ret;
}

unsigned long do_io_write_16 (unsigned long val, void *address)
{
        iowrite16(val, address);
	wmb();
	return;
}

unsigned long do_io_read (void *address)
{
        unsigned long ret = 0;        
        ret = ioread32(address);
	rmb();
	return ret;
}

unsigned long do_io_write_32 (unsigned long val, void *address)
{
        iowrite32(val, address);
	wmb();
	return;
}

unsigned long chage_byte_order_32 (unsigned long in_val)
{
        unsigned long out_val;
        unsigned char temp, byte1, byte2, byte3, byte4;     
        byte1 = (unsigned char)(in_val >> 24); 
        byte2 = (unsigned char)((in_val >> 16) & 0x000000FF); 
        byte3 = (unsigned char)((in_val >> 8) & 0x000000FF); 
        byte4 = (unsigned char)(in_val & 0x000000FF); 
        temp = byte1; 
        byte1 = byte4; 
        byte4 = temp; 
        temp = byte2; 
        byte2 = byte3; 
        byte3 = temp; 
        out_val = (unsigned long)((byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4);        
	//return out_val;
        return in_val;
}

unsigned int chage_byte_order_16 (unsigned int in_val)
{
        unsigned int out_val;
        unsigned char temp, byte1, byte2;       
        byte1 = (unsigned char)((in_val >> 8) & 0x00FF); 
        byte2 = (unsigned char)(in_val & 0x00FF); 
        temp = byte1; 
        byte1 = byte2; 
        byte2 = temp; 
        out_val = (unsigned long)((byte1 << 8) | byte2);        
	//return out_val;
        return in_val;
}
*/
/*
 * This structure describes all the operations that can be
 * done on the physical hardware.
 */
struct razorcar_hardware_ops {
        int             (*startup)(struct razorcar_hardware_module *);
        void            (*shutdown)(struct razorcar_hardware_module *);
        void            (*flush_buffer)(struct razorcar_hardware_module *);
 
        /*
          * Return a string describing the type of the module
          */
         const char *(*type)(struct razorcar_hardware_module *);
 
        /*
         * Release IO and memory resources used by the module.
         * This includes iounmap if necessary.
         */
        void            (*release_module)(struct razorcar_hardware_module *);

        /*
         * Request IO and memory resources used by the module.
         * This includes iomapping the module if necessary.
         */
        int             (*request_module)(struct razorcar_hardware_module *);
        void            (*config_module)(struct razorcar_hardware_module *, int);
//       int             (*verify_module)(struct razorcar_hardware_module *, struct serial_struct *);
        int             (*ioctl)(struct razorcar_hardware_module *, unsigned int, unsigned long);
};

/*
 * This is the state information which is persistent across opens.
 * The low level driver must not to touch any elements contained
 * within.
 */
struct razorcar_hardware_state {
        int                     count;
        struct razorcar_hardware_module    *module;

        //struct mutex            mutex;
};


struct razorcar_hardware_module {
        spinlock_t              lock;                   /* module lock */
        unsigned long           iobase;                 /* in/out[bwl] */
        unsigned char __iomem   *membase;               /* read/write[bwl] */
//        unsigned int            (*serial_in)(struct razorcar_hardware_module *, int);
//        void                    (*serial_out)(struct razorcar_hardware_module *, int, int);
        unsigned int                          type;           /* module type */
        const struct razorcar_hardware_ops   *ops;
        resource_size_t                       mapbase;        /* for ioremap */
        struct device                         *dev;           /* parent device */
        void                                  *private_data;  /* generic platform data pointer */
};

static struct razorcar_hardware_module razorcar_hardware_ctrl_mod[razorcar_hardware_NR_MODULES];

/* ---------------------------------------------------------------------
 * Core razorcar_hardware driver operations
 */

static int razorcar_hardware_startup(struct razorcar_hardware_module *module)
{
	//int ret;
	++counter;
	return 0;
}

static void razorcar_hardware_shutdown(struct razorcar_hardware_module *module)
{

}

static void razorcar_hardware_release_module(struct razorcar_hardware_module *module)
{
        release_mem_region(module->mapbase, razorcar_hardware_REGION);
	iounmap(module->membase);
	module->membase = NULL;
	
}

static const char *razorcar_hardware_type(struct razorcar_hardware_module *module)
{
	return module->type == MODULE_razorcar_hardware ? "razorcar_hardware_ctrl" : NULL;
}

static int razorcar_hardware_request_module(struct razorcar_hardware_module *module)
{

       // module->mapbase = module->mapbase - 3;  /* FIXME : I don't know why??? */
        //int ret;
	printk(KERN_INFO "razorcar_hardware_ctrl: module=%p; module->mapbase=%llx\n",
		 module, (unsigned long long) module->mapbase);
	// request memory
	if (!request_mem_region(module->mapbase, razorcar_hardware_REGION, "razorcar_hardware_ctrl")) {
		dev_err(module->dev, "Memory region busy\n");
		return -EBUSY;
	}

	// convert base address to virtual address
	module->membase = ioremap(module->mapbase, razorcar_hardware_REGION);
	if (!module->membase) {
		dev_err(module->dev, "Unable to map registers\n");
		release_mem_region(module->mapbase, razorcar_hardware_REGION);
		return -EBUSY;
	}

        hw_regs = (razorcar_hw_regs_t *)module->membase;	
        //RazorCar_set_BaseAdress(module->membase);
        init_razorcar_platform((uint32_t)module->membase);
	mem_base_addresses[counter] = hw_regs;
        mutex_init(&mutex);
	printk(KERN_INFO "razorcar_hardware_ctrl: after map module=%p; module->membase=%llx\n",
		 module, (unsigned long long) module->membase);
	return 0;
}

static void razorcar_hardware_config_module(struct razorcar_hardware_module *module, int flags)
{
	if (!razorcar_hardware_request_module(module))
		module->type = MODULE_razorcar_hardware;
}

static struct razorcar_hardware_ops razorcar_hardware_ops = {
	.startup	= razorcar_hardware_startup,
	.shutdown	= razorcar_hardware_shutdown,
        .type		= razorcar_hardware_type,
	.release_module	= razorcar_hardware_release_module,
	.request_module	= razorcar_hardware_request_module,
	.config_module	= razorcar_hardware_config_module,
};


// akzare end


//static int plb_data_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg)
static long razorcar_hardware_data_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;
	//int i;
	unsigned long tmp;
	
	//uint32_t *reg_pntr = (uint32_t *)hw_regs;

       // DEFINE_WAIT(wait);

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	//if (_IOC_TYPE(cmd) != RAZOR_CAR_IOC_MAGIC) return -ENOTTY;
	//if (_IOC_NR(cmd) > RAZOR_CAR_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	//if (_IOC_DIR(cmd) & _IOC_READ)
	//	err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	//else if (_IOC_DIR(cmd) & _IOC_WRITE)
	//	err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	//if (err) return -EFAULT;

	//printk(KERN_INFO "razorcar_hardware: ioctl call with command: 0x%X\n", cmd);

	if (mutex_lock_interruptible(&mutex)) return -ERESTARTSYS;

	switch (cmd) {
        case RAZOR_CAR_IOC_LIGHT:
                retval = __get_user(tmp, (unsigned long __user *)arg);
                //printk(KERN_INFO "razorcar_hardware: ioctl light with value : 0x%X\n", tmp);
                hw_regs->leds_buzzer = (hw_regs->leds_buzzer & RAZORCAR_HARDWARE_SET_LIGHT_MASK) + tmp;
		break;
        case RAZOR_CAR_IOC_DRIVING_MODE: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                hw_regs->steering_speed = (hw_regs->steering_speed & RAZORCAR_HARDWARE_SET_MODE_MASK) + ((tmp & 0x3) << 16);
		break;
        case RAZOR_CAR_IOC_STEERING_STRAIGHT: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_steering(STRAIGHT, tmp);
		break;
        case RAZOR_CAR_IOC_STEERING_LEFT: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_steering(LEFT, tmp);
		break;
        case RAZOR_CAR_IOC_STEERING_RIGHT: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_steering(RIGHT, tmp);
		break;
        case RAZOR_CAR_IOC_SPEED_STOP: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_speed(STOP,tmp);
		break;
        case RAZOR_CAR_IOC_SPEED_FORWARD: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_speed(FORWARD,tmp);
		break;
        case RAZOR_CAR_IOC_SPEED_BACKWARD: 
		retval = __get_user(tmp, (unsigned long __user *)arg);
                set_speed(BACKWARD,tmp);
		break;
        case RAZOR_CAR_IOC_READ_FRONT: 
		tmp = get_sensor_values(FRONT_SENSOR);        
		retval = __put_user(tmp, (unsigned long __user *)arg);
		break;
        case RAZOR_CAR_IOC_READ_BACK: 
		tmp = get_sensor_values(REAR_SENSOR);       
		retval = __put_user(tmp, (unsigned long __user *)arg);
		break;
        case RAZOR_CAR_IOC_GET_SPEED: 
		tmp = get_speed();       
		retval = __put_user(tmp, (unsigned long __user *)arg);
		break;        
        case RAZOR_CAR_IOC_GET_STEERING: 
		tmp = get_steering();       
		retval = __put_user(tmp, (unsigned long __user *)arg);
		break;
        default:  /* redundant, as cmd was checked against MAXNR */
	        return -ENOTTY;
		
	}

//out:
	mutex_unlock(&mutex);
	return retval;
	
}
/*
struct mmap_info {
	char *data;
	int reference; 	
};*/
//static struct miscdevice razorcar_hardware_driver;

static int razorcar_hardware_data_open(struct inode *inode,struct file *filp)
{
	return 0;
}

static int razorcar_hardware_data_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int razorcar_hardware_data_mmap(struct file *file, struct vm_area_struct *vma)
{
	/* mm semaphore is already held within mmap functions */
	printk(KERN_INFO "razorcar_hardware: call to mmap: vm_start=0x%08lX vm_end=0x%08lX\n", vma->vm_start, vma->vm_end);

	//vma->vm_flags |= VM_RESERVED | VM_IO;
	vma->vm_flags |= VM_RESERVED ;
	if (remap_pfn_range(vma, vma->vm_start, MEM_ADDR >> PAGE_SHIFT, 
				vma->vm_end-vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

static struct file_operations razorcar_hardware_data_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = razorcar_hardware_data_ioctl,
	.open = razorcar_hardware_data_open,
	.release = razorcar_hardware_data_release,
        .mmap = razorcar_hardware_data_mmap,
};

static struct razorcar_hardware_module * razorcar_hardware_get_module(int id)
{
	struct razorcar_hardware_module *module;

	/* if id = -1; then scan for a free id and use that */
	if (id < 0) {
		for (id = 0; id < razorcar_hardware_NR_MODULES; id++)         
			if (razorcar_hardware_ctrl_mod[id].mapbase == 0)
				break;
	}

	if ((id < 0) || (id >= razorcar_hardware_NR_MODULES)) {               
		printk(KERN_WARNING "razorcar_hardware_ctrl: invalid id: %i\n", id);
		return NULL;
	}

	/* The ID is valid, so get the address of the razorcar_hardware_module structure */
	module = &razorcar_hardware_ctrl_mod[id];

	/* Is the structure is already initialized? */
	if (module->mapbase)
		return module;

	/* At this point, we've got an empty razorcar_hardware_module struct, initialize it */
	spin_lock_init(&module->lock);
	module->membase = NULL;
	module->iobase = 1; /* mark module in use */
	module->ops = &razorcar_hardware_ops;
	module->dev = NULL;
	module->type = MODULE_UNKNOWN;

	return module;
}
/**
 *      razorcar_hardware_add_one_module - attach a driver-defined module structure
 *      @drv: pointer to the uart low level driver structure for this module
 *      @module: razorcar_hardware_module structure to use for this module.
 *
 *      This allows the driver to register its own razorcar_hardware_module structure
 *      with the core driver.  The main purpose is to allow the low
 *      level razorcar_hardware drivers to expand razorcar_hardware_module, rather than having yet
 *      more levels of structures.
 */
int razorcar_hardware_add_one_module(struct razorcar_hardware_module *module)
{
        struct razorcar_hardware_state *state;
        int ret = 0;

        module->ops->config_module(module, 0);
        if (!module->ops->type(module))
                return -EINVAL;
//	printk(KERN_INFO "razorcar_hardware: Module TypeBase: %s \n",module->ops->type(module));
        ret = module->ops->startup(module);

        return ret;
}
/**
 *      razorcar_hardware_remove_one_module - detach a driver defined module structure
 *      @drv: pointer to the razorcar_hardware low level driver structure for this module
 *      @module: razorcar_hardware module structure for this module
 *
 *      This unhooks (and hangs up) the specified module structure from the
 *      core driver.  No further calls will be made to the low-level code
 *      for this module.
 */
int razorcar_hardware_remove_one_module(struct razorcar_hardware_module *module)
{
        struct razorcar_hardware_state *state;

        module->ops->shutdown(module);
        module->ops->release_module(module);

        return 0;
}

/* ---------------------------------------------------------------------
 * Port assignment functions (mapping devices to razorcar_hardware_module structures)
 */

/** razorcar_hardware_assign: register a razorcar_hardware device with the driver
 *
 * @dev: pointer to device structure
 * @id: requested id number.  Pass -1 for automatic module assignment
 * @base: base address of razorcar_hardware registers
 * @irq: irq number for razorcar_hardware
 *
 * Returns: 0 on success, <0 otherwise
 */
static int  razorcar_hardware_assign(struct device *dev, int id, u32 base, int irq)
{
	struct razorcar_hardware_module *module;
	int rc;

	module = razorcar_hardware_get_module(id);
	if (!module) {
		dev_err(dev, "Cannot get razorcar_hardware_module structure\n");
		return -ENODEV;
	}

	/* was it initialized for this device? */
	if ((module->mapbase) && (module->mapbase != base)) {
		pr_debug(KERN_DEBUG "razorcar_hardware: addr mismatch; %x != %x\n",
			 module->mapbase, base);
		return -ENODEV;
	}

	module->mapbase = base;
	module->dev = dev;
	dev_set_drvdata(dev, module);

	/* Register the module */
	rc = razorcar_hardware_add_one_module(module); 
	if (rc) {
		dev_err(dev, "razorcar_hardware_add_one_module() failed; err=%i\n", rc);
		module->mapbase = 0;
		dev_set_drvdata(dev, NULL);
		return rc;
	}

	return 0;
}

/** razorcar_hardware_release: register a razorcar_hardware device with the driver
 *
 * @dev: pointer to device structure
 */
static int  razorcar_hardware_release(struct device *dev)
{
	struct razorcar_hardware_module *module = dev_get_drvdata(dev);
	int rc = 0;

	if (module) {
		rc = razorcar_hardware_remove_one_module(module);
		dev_set_drvdata(dev, NULL);
		module->mapbase = 0;
	}

	return rc;
}

/* ---------------------------------------------------------------------
 * Platform bus binding
 */

static int  razorcar_hardware_probe(struct platform_device *pdev)
{
	struct resource *res, *res2;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	res2 = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res2)
		return -ENODEV;

	return razorcar_hardware_assign(&pdev->dev, pdev->id, res->start, res2->start);
}

static int  razorcar_hardware_remove(struct platform_device *pdev)
{
	return razorcar_hardware_release(&pdev->dev);
}

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:razorcar_hardware_ctrl");

static struct platform_driver razorcar_hardware_platform_driver = {
	.probe	= razorcar_hardware_probe,
	.remove	= razorcar_hardware_remove,
	.driver	= {
		   .owner = THIS_MODULE,
		   .name  = "razorcar_hardware_ctrl",
                   .of_match_table = razorcar_hardware_of_match,
		   },
};

/* ---------------------------------------------------------------------
 * OF bus bindings
 */
#if defined(CONFIG_OF)
static int 
razorcar_hardware_of_probe(struct of_device *op, const struct of_device_id *match)
{
	struct resource res;
	const unsigned int *id;
	int irq, rc;

//	dev_dbg(&op->dev, "%s(%p, %p)\n", __func__, op, match);
	printk(KERN_INFO "%s(%p, %p)\n", __func__, op, match);

	//rc = of_address_to_resource(op->node, 0, &res);
	if (rc) {
		//dev_err(&op->dev, "invalid address\n");
		return rc;
	}

	//irq = irq_of_parse_and_map(op->node, 0);

	//id = of_get_property(op->node, "module-number", NULL);

	//return razorcar_hardware_assign(&op->dev, id ? *id : -1, res.start+3, irq);
}

static int razorcar_hardware_of_remove(struct of_device *op)
{
	//return razorcar_hardware_release(&op->dev);
}

static struct of_platform_driver razorcar_hardware_of_driver = {
	//.owner = THIS_MODULE,
	//.name = "razorcar_hardware",
	//.match_table = razorcar_hardware_of_match,
	.probe = razorcar_hardware_of_probe,
	.remove = razorcar_hardware_of_remove,
	.driver = {
		.name = "razorcar_hardware_ctrl",
                .owner = THIS_MODULE,
                //.match_table = razorcar_hardware_of_match,
	},
};

/* Registration helpers to keep the number of #ifdefs to a minimum */
static inline int __init razorcar_hardware_of_register(void)
{
//	pr_debug("razorcar_hardware: calling of_register_platform_driver()\n");
	printk(KERN_INFO "razorcar_hardware_ctrl: calling of_register_platform_driver()\n");
	//return of_register_platform_driver(&razorcar_hardware_of_driver);
      return platform_driver_register(&razorcar_hardware_of_driver);
}

static inline void __exit razorcar_hardware_of_unregister(void)
{
	//of_unregister_platform_driver(&razorcar_hardware_of_driver);
      platform_driver_unregister(&razorcar_hardware_of_driver);
}
#else /* CONFIG_OF */
/* CONFIG_OF not enabled; do nothing helpers */
static inline int __init razorcar_hardware_of_register(void) { return 0; }
static inline void __exit razorcar_hardware_of_unregister(void) { }
#endif /* CONFIG_OF */


// akzare end

static int __init razorcar_hardware_init(void)
{
	int rtn;
        // akzare start
	// register driver
        //sema_init(&mutex, 1);
        //spin_lock_init(&mr_lock);
	printk(KERN_INFO "razorcar_hardware_ctrl: calling register_driver()\n");
/*	rtn = misc_register(&razorcar_hardware_driver);        
	if (rtn)
		goto err_sdi;
*/		
	// Here we register our device - should not fail thereafter
	rtn = register_chrdev(major, "razorcar_hardware_ctrl", &razorcar_hardware_data_fops);
	if (rtn < 0) {
		goto err_sdi;
	}
	if (major == 0) major = rtn; /* dynamic */
		
	
	//printk(KERN_INFO "razorcar_hardware: calling razorcar_hardware_of_register()\n");
	//rtn = razorcar_hardware_of_register();
	//if (rtn)
	//	goto err_of;
		
	printk(KERN_INFO "razorcar_hardware_ctrl: calling platform_driver_register()\n");
	rtn = platform_driver_register(&razorcar_hardware_platform_driver);
	if (rtn)
		goto err_plat;
        // akzare end
		

	printk(KERN_INFO "-----------------\nrazorcar_hardware_ctrl: compiled: %s, %s\n", __TIME__, __DATE__);

//	plb_data_ioctl(NULL, NULL, razorcar_hardware_IOCSRDWRCONT1MEM, (unsigned long)0);	
	return 0;
	
// akzare start
err_plat:
	//razorcar_hardware_of_unregister();
//err_of:
//	misc_deregister(&razorcar_hardware_driver);
	unregister_chrdev(major, "razorcar_hardware_ctrl");
err_sdi:
	printk(KERN_ERR "registering razorcar_hardware driver failed: err=%i", rtn);
	return rtn;
// akzare end
	
}

static void __exit
razorcar_hardware_cleanup(void)
{

	// akzare start
        platform_driver_unregister(&razorcar_hardware_platform_driver);	
        //razorcar_hardware_of_unregister();
	// akzare end
        
//	misc_deregister(&razorcar_hardware_driver);    
	unregister_chrdev(major, "razorcar_hardware_ctrl");
	
}

EXPORT_NO_SYMBOLS;

module_init(razorcar_hardware_init);
module_exit(razorcar_hardware_cleanup);
