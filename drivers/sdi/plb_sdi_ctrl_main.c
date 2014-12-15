#include <linux/time.h>
#include <linux/jiffies.h>
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
#include "plb_sdi_controller.h"
#include <linux/wait.h>
typedef struct page mem_map_t;
#undef PDEBUG
#define PDEBUG(fmt, args...) printk(KERN_INFO fmt, ## args)
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
static struct of_device_id plb_sdi_controller_of_match[]  = {
	{ .compatible = "xlnx,axi-sdi-controller-1.00.a", },
	{}
};
MODULE_DEVICE_TABLE(of, plb_sdi_controller_of_match);
#endif
#ifndef VM_RESERVED
//#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#define VM_RESERVED VM_DONTEXPAND
#endif
MODULE_AUTHOR("Felix Muehlbauer & akzare & Michael");
MODULE_DESCRIPTION("Generic device driver for SDI controller hardware modules");
MODULE_LICENSE("Dual BSD/GPL");
#define MODULE_NAME             "plb_sdi_controller"
#define MINOR         0
// akzare start
static int major = 233;	/* dynamic by default */
module_param(major, int, 0);
hw_regs_t *hw_regs = (hw_regs_t *)NULL;

static DECLARE_WAIT_QUEUE_HEAD(intr_wait_queue);
//DEFINE_MUTEX(mutex);
struct mutex mutex;
uint32_t *dst_buffer;
int counter=0;

int sdi_reg_num;
hw_regs_t* mem_base_addresses[2];

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

/*
 * This structure describes all the operations that can be
 * done on the physical hardware.
 */
struct plb_sdi_controller_ops {
        int             (*startup)(struct plb_sdi_controller_module *);
        void            (*shutdown)(struct plb_sdi_controller_module *);
        void            (*flush_buffer)(struct plb_sdi_controller_module *);
 
        /*
          * Return a string describing the type of the module
          */
         const char *(*type)(struct plb_sdi_controller_module *);
 
        /*
         * Release IO and memory resources used by the module.
         * This includes iounmap if necessary.
         */
        void            (*release_module)(struct plb_sdi_controller_module *);

        /*
         * Request IO and memory resources used by the module.
         * This includes iomapping the module if necessary.
         */
        int             (*request_module)(struct plb_sdi_controller_module *);
        void            (*config_module)(struct plb_sdi_controller_module *, int);
//       int             (*verify_module)(struct plb_sdi_controller_module *, struct serial_struct *);
        int             (*ioctl)(struct plb_sdi_controller_module *, unsigned int, unsigned long);
};


typedef unsigned int __bitwise__ sdiif_t;

struct plb_sdi_controller_info {
        sdiif_t                   flags;

/*
 * Definitions for info->flags.  These are _private_ to plb_sdi_controller_core, and
 * are specific to this structure.  They may be queried by low level drivers.
 *
 */
#define SDIIF_RES_0            ((__force sdiif_t) (1 << 25))
#define SDIIF_RES_1            ((__force sdiif_t) (1 << 26))
#define SDIIF_RES_2            ((__force sdiif_t) (1 << 29))
#define SDIIF_RES_3            ((__force sdiif_t) (1 << 31))
#define SDIIF_RES_4            ((__force sdiif_t) (1 << 30))

        struct tasklet_struct   tlet;
        wait_queue_head_t       delta_msr_wait;
};

/*
 * This is the state information which is persistent across opens.
 * The low level driver must not to touch any elements contained
 * within.
 */
struct plb_sdi_controller_state {
        int                     count;
        int                     pu_state;
        struct plb_sdi_controller_info      info;
        struct plb_sdi_controller_module    *module;

        //struct mutex            mutex;
};

struct plb_sdi_controller_icount {
        __u32   reader;
        __u32   writer;
        __u32   reader_tick;
        __u32   writer_tick;
        __u32   master_error;
        __u32   master_timeout_error;
        __u32   reader_start_addr_error;
        __u32   writer_start_addr_error;
        __u32   reader_end_addr_error;
        __u32   writer_end_addr_error;
};

typedef unsigned int __bitwise__ sdimf_t;

struct plb_sdi_controller_module {
        spinlock_t              lock;                   /* module lock */
        unsigned long           iobase;                 /* in/out[bwl] */
        unsigned char __iomem   *membase;               /* read/write[bwl] */
        unsigned char __iomem   *dma_membase;           /* read/write[bwl] */
//        unsigned int            (*serial_in)(struct plb_sdi_controller_module *, int);
//        void                    (*serial_out)(struct plb_sdi_controller_module *, int, int);
        unsigned int            irq;                    /* irq number */
        unsigned int            reader_fifosize;        /* reader fifo size */
        unsigned int            writer_fifosize;        /* writer fifo size */
        unsigned char           iotype;                 /* io access style */

#define SDIPIO_PORT               (0)
#define SDIPIO_HUB6               (1)

        unsigned int            read_status_mask;       /* driver specific */
        unsigned int            ignore_status_mask;     /* driver specific */
        struct plb_sdi_controller_info        *info;    /* pointer to parent info */
        struct plb_sdi_controller_icount      icount;   /* statistics */

        sdimf_t                 flags;

#define SDIMF_RES_0            ((__force sdimf_t) (1 << 1))
#define SDIMF_RES_1            ((__force sdimf_t) (1 << 2))
#define SDIMF_RES_MASK         ((__force sdimf_t) (0x1030))
#define SDIMF_RES_3            ((__force sdimf_t) (0x0010))
#define SDIMF_RES_n            ((__force sdimf_t) (1 << 31))

        unsigned int                          type;           /* module type */
        const struct plb_sdi_controller_ops   *ops;
        resource_size_t                       mapbase;        /* for ioremap */
        struct device                         *dev;           /* parent device */
        void                                  *private_data;  /* generic platform data pointer */
};

static struct plb_sdi_controller_module plb_sdi_ctrl_mod[PLB_SDI_CONTROLLER_NR_MODULES];

/* ---------------------------------------------------------------------
 * Core plb_sdi_controller driver operations
 */

void configureSDIController()
{

      uint16_t hw_v = chage_byte_order_16(do_io_read16( &(hw_regs->hw_version)) );
      uint16_t hw_m = chage_byte_order_16(do_io_read16( &(hw_regs->magic)) );
      printk(KERN_INFO "HW Version: 0x%04X / magic: 0x%04X\n", hw_v, hw_m);
      printk(KERN_INFO "dst buf: 0x%08X\n", dst_buffer);
      PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
      PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
      PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
      PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
      PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();  
	if( counter == 0 )
	{
		//set_video_mode(0); 0==gray, 2==color, 1=color24bit
	      //do_io_write_32(chage_byte_order_32((unsigned long)0), &(hw_regs->pu_control_status_regs[0])); // VGA
	      //do_io_write_32(chage_byte_order_32((unsigned long)((X_SIZE << 16) | 0)), &(hw_regs->pu_control_status_regs[3])); // ISI 
	      //do_io_write_32(chage_byte_order_32((unsigned long)((Y_SIZE << 16) | 0)), &(hw_regs->pu_control_status_regs[4])); // ISIz
		//reader (VGA) address space (3 byte per pixel)
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + BUFFER_SIZE)), &(hw_regs->src_start_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));
		 // writer (image sensor) address space
		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR) , &(hw_regs->dest_start_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));
	}
	else
	{
		//reader (filter bg) address space (4 byte per pixel)
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + 3*BUFFER_SIZE )), &(hw_regs->src_start_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (4*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (4*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));
		 // writer (filter bg) address space
	      do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR+ 2*BUFFER_SIZE), &(hw_regs->dest_start_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR +(3*BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
	      do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (3*BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));
	}

      printk(KERN_INFO "Copy to: 0x%08X-0x%08X \n",  chage_byte_order_32(do_io_read( &(hw_regs->dest_start_addr))),
		 chage_byte_order_32(do_io_read( &(hw_regs->dest_end_addr))) );
      printk(KERN_INFO "Read from: 0x%08X-0x%08X\n",  chage_byte_order_32(do_io_read( &(hw_regs->src_start_addr))),
		 chage_byte_order_32(do_io_read( &(hw_regs->src_end_addr))) );
    
}
static irqreturn_t plb_sdi_controller_isr(int irq, void *dev_id)
{
	//printk(KERN_INFO "plb_sdi_controller_isr \n");
        struct plb_sdi_controller_module *module = dev_id;
        PLB_SDI_CONTROLLER_Intr_Handler((unsigned long long) module->membase);
	wake_up_interruptible(&intr_wait_queue);
	return IRQ_HANDLED;
}
static int plb_sdi_controller_startup(struct plb_sdi_controller_module *module)
{
	int ret;
	/*
	 * Just to be safe, stop the device first.  
	 */
	PLB_SDI_CONTROLLER_DisableInterrupt((unsigned long long) module->membase);

	ret = request_irq(module->irq, plb_sdi_controller_isr,
			  IRQF_DISABLED | 0, "plb_sdi_ctrl", module);
	if (ret){
                printk(KERN_ERR "plb_sdi_ctrl: Can not allocate interrupt #%d!\n", module->irq);	
		return ret;
	}
	printk(KERN_INFO "plb_sdi_ctrl: IRQ#%d is allocated.\n", module->irq);	
        PLB_SDI_CONTROLLER_EnableInterrupt((unsigned long long) module->membase);
	configureSDIController();
	++counter;
	return 0;
}

static void plb_sdi_controller_shutdown(struct plb_sdi_controller_module *module)
{
	PLB_SDI_CONTROLLER_DisableInterrupt((unsigned long long) module->membase);
	free_irq(module->irq, module);
}

static void plb_sdi_controller_release_module(struct plb_sdi_controller_module *module)
{
	release_mem_region(module->mapbase, PLB_SDI_CONTROLLER_REGION);
	iounmap(module->membase);
	module->membase = NULL;
	
}

static const char *plb_sdi_controller_type(struct plb_sdi_controller_module *module)
{
	return module->type == MODULE_PLB_SDI_CONTROLLER ? "plb_sdi_ctrl" : NULL;
}

static int plb_sdi_controller_request_module(struct plb_sdi_controller_module *module)
{

       // module->mapbase = module->mapbase - 3;  /* FIXME : I don't know why??? */
        int ret;
	printk(KERN_INFO "plb_sdi_ctrl: module=%p; module->mapbase=%llx; module->dma_membase=%llx\n",
		 module, (unsigned long long) module->mapbase, (unsigned long long) module-> dma_membase);
	// request memory
	if (!request_mem_region(module->mapbase, PLB_SDI_CONTROLLER_REGION, "plb_sdi_ctrl")) {
		dev_err(module->dev, "Memory region busy\n");
		return -EBUSY;
	}

	// convert base address to virtual address
	module->membase = ioremap(module->mapbase, PLB_SDI_CONTROLLER_REGION);
	if (!module->membase) {
		dev_err(module->dev, "Unable to map registers\n");
		release_mem_region(module->mapbase, PLB_SDI_CONTROLLER_REGION);
		return -EBUSY;
	}

        hw_regs = (hw_regs_t *)module->membase;	
        PLB_SDI_CONTROLLER_Set_BaseAddress(module->membase);
        PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
        PLB_SDI_CONTROLLER_Reset_Writer_FIFO();

	mem_base_addresses[counter] = hw_regs;


	// convert base address to virtual address (for DMA)
	module->dma_membase = ioremap(module->dma_membase, DMA_MEM_REGION);
	if (!module->dma_membase) {
		dev_err(module->dev, "Unable to map DMA region\n");
		release_mem_region(module->mapbase, PLB_SDI_CONTROLLER_REGION);
		return -EBUSY;
	}
		
	dst_buffer = kmalloc(BUFFER_SIZE*8, GFP_KERNEL);
        if (!dst_buffer) {
                dev_err(module->dev, "can't allocate DATA BUFFER\n");
               return -EBUSY;
        }
        //memset(dst_buffer,0,BUFFER_SIZE*4);
	dst_buffer = (uint32_t*)module->dma_membase;
        mutex_init(&mutex);
	printk(KERN_INFO "plb_sdi_ctrl: after map module=%p; module->membase=%llx; module->dma_membase=%llx\n",
		 module, (unsigned long long) module->membase, (unsigned long long) module-> dma_membase);
	return 0;
}

static void plb_sdi_controller_config_module(struct plb_sdi_controller_module *module, int flags)
{
	if (!plb_sdi_controller_request_module(module))
		module->type = MODULE_PLB_SDI_CONTROLLER;
}

static struct plb_sdi_controller_ops plb_sdi_controller_ops = {
	.startup	= plb_sdi_controller_startup,
	.shutdown	= plb_sdi_controller_shutdown,
        .type		= plb_sdi_controller_type,
	.release_module	= plb_sdi_controller_release_module,
	.request_module	= plb_sdi_controller_request_module,
	.config_module	= plb_sdi_controller_config_module,
};


// akzare end



static void plb_data_intr(int irq, void *dev_id, struct pt_regs *regs)
{
}

//static int plb_data_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg)
static long plb_data_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;
	unsigned long i; unsigned long j, stamp_n;
	unsigned long tmp;
	uint32_t dma_buffer_size = DMA_MEM_BASEADDR; //DMA_MEM_REGION;
	
	uint32_t *reg_pntr = (uint32_t *)hw_regs;

	
        DEFINE_WAIT(wait);

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != PLB_SDI_CTRL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > PLB_SDI_CTRL_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	//PDEBUG(KERN_INFO "plb_sdi_ctrl: ioctl call with command: 0x%X\n", cmd);

	if (mutex_lock_interruptible(&mutex)) return -ERESTARTSYS;

	switch (cmd) {
        case PLB_SDI_CTRL_IOCSHWREGNUM:
		//PDEBUG(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSHWREGNUM \n");
		retval = __get_user(sdi_reg_num, (int __user *)arg);
		break;
        case PLB_SDI_CTRL_IOCGHWREG: 
		//PDEBUG(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCGHWREG \n");
                for (i = 0; i < sdi_reg_num; i++)
                        reg_pntr ++;
                tmp = chage_byte_order_32(do_io_read(reg_pntr));        
		retval = __put_user(tmp, (unsigned long __user *)arg);

		break;
        case PLB_SDI_CTRL_IOCSHWREG: 
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSHWREG \n");
                for (i = 0; i < sdi_reg_num; i++)
                        reg_pntr ++;
		retval = __get_user(tmp, (unsigned long __user *)arg);
	        do_io_write_32(chage_byte_order_32(tmp), reg_pntr);
		break;		
	case PLB_SDI_CTRL_IOCSRDSNG:        
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDSNG in\n");
		//if (! capable (CAP_SYS_ADMIN)){
		//	retval = -EPERM;
		//	goto out;
		//}

		PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
		PLB_SDI_CONTROLLER_Reset_Writer_FIFO();
		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
                PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
		PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + BUFFER_SIZE)), &(hw_regs->src_start_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
                ( PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_end_addr)));
		}
		 i=0;
                 //PDEBUG(KERN_INFO "first wait:\n");
                
	         while (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_READER_BUSY_MASK) {//
                   //i=i+1;//PDEBUG(KERN_INFO "plb_sdi_ctrl:ir: %d\n",i);
                   //if(i==500000){break;}
                  //
		     //   prepare_to_wait(&intr_wait_queue, &wait, TASK_INTERRUPTIBLE);
		     //   if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_READER_BUSY_MASK)
			//        schedule();
		      //  finish_wait(&intr_wait_queue, &wait);
                      //  i=i+1;
		      //  if (signal_pending (current))  // a signal arrived
			//        return -ERESTARTSYS; // tell the fs layer to handle it
	       } 
                i=0;
                //PDEBUG(KERN_INFO "second wait:\n");
                while (!PLB_SDI_CONTROLLER_Check_ReaderDone_Flg()){  i=i+1;//PDEBUG(KERN_INFO "plb_sdi_ctrl:ir: %d\n",i);
                //   if(i>500000){break;}
                }
                do_io_write_32(chage_byte_order_32((unsigned long)0), &(hw_regs->control_status_reg));
		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		//if (PLB_SDI_CONTROLLER_Check_ReaderFlushed_Flg()){PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();}
                //PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		retval = __put_user(chage_byte_order_32(do_io_read(&(hw_regs->tick_counter))), (unsigned long __user *)arg);
                //printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDSNG  out\n");
		break;

	case PLB_SDI_CTRL_IOCSWRSNG:
			//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSWRSNG in\n");
					//if (! capable (CAP_SYS_ADMIN)){
					//	retval = -EPERM;
					//	goto out;
					//}

                        retval = __get_user(tmp, (unsigned long __user *)arg); 
                        i=0;
			PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
			PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
			PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
			PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
			PLB_SDI_CONTROLLER_Reset_Writer_FIFO();
			PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
			PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();
                        do_io_write_32(chage_byte_order_32((unsigned long)0), &(hw_regs->pu_control_status_regs[0]));
                        //while(i<500){i=i+1;}
                        //while(i<3)
                        //{
			    // PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
			    // PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();                        
                        	do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR) , &(hw_regs->dest_start_addr));
		        do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		            do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));
			  flush_cache_all(); 
			do_io_write_32(chage_byte_order_32((unsigned long)
		        (PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK  | 
			PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
		                &(hw_regs->control_status_reg));

			if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
			    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
			}
			if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
			    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
			}
			//while ((chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_WRITER_BUSY_MASK  ) && (!PLB_SDI_CONTROLLER_Check_WriterDone_Flg())) {
                         while ((chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_WRITER_BUSY_MASK  )){//& (i<5000)
                          i=i+1;//PDEBUG(KERN_INFO "plb_sdi_ctrl:iw: %d\n", i);
                          //if(i>500000){break;}
				//prepare_to_wait(&intr_wait_queue, &wait, TASK_INTERRUPTIBLE);
				//if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_WRITER_BUSY_MASK)
				//	schedule();
				//finish_wait(&intr_wait_queue, &wait);
				//if (signal_pending (current))  // a signal arrived
				//	return -ERESTARTSYS; // tell the fs layer to handle it
			 } 
                         i=0;
                         while (!PLB_SDI_CONTROLLER_Check_WriterDone_Flg()){i=i+1;
                          //if(i>500000){break;}
                         }
                         do_io_write_32(chage_byte_order_32((unsigned long)0), &(hw_regs->control_status_reg));
                         PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
                         i=i+1;
                        //printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSWRSNG out\n");
			retval = __put_user(chage_byte_order_32(do_io_read(&(hw_regs->tick_counter))), (unsigned long __user *)arg);
		break;

        case PLB_SDI_CTRL_IOCSWRCORSNG:
		       printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSWRCORSNG in\n");
					//if (! capable (CAP_SYS_ADMIN)){
					//	retval = -EPERM;
					//	goto out;
					//}

                        retval = __get_user(tmp, (unsigned long __user *)arg); 
                        //PDEBUG(KERN_INFO "plb_sdi_ctrl: ioctl call 0x%X\n", tmp);
                        i=0;
			PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
			PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
			PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
			PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
			PLB_SDI_CONTROLLER_Reset_Writer_FIFO();
			PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
			PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();
                        do_io_write_32(chage_byte_order_32((unsigned long)0xFFFFFFFF), &(hw_regs->pu_control_status_regs[0]));
                        //while(i<500){i=i+1;}
                        //while(i<2)
                        //{    //do_io_write_32(chage_byte_order_32((unsigned long)0xFFFFFFFF), &(hw_regs->pu_control_status_regs[0]));
			    // PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
			    // PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();                        
                        	do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR + 2*BUFFER_SIZE) , &(hw_regs->dest_start_addr));
		        do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (3*BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		            do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (3*BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));
			  flush_cache_all(); 
			do_io_write_32(chage_byte_order_32((unsigned long)
		        (PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK | 
			PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
		                &(hw_regs->control_status_reg));

			if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
			    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
			}
			if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
			    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
			}
			while ((chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_WRITER_BUSY_MASK  ) && (!PLB_SDI_CONTROLLER_Check_WriterDone_Flg())){//& (i<5000)
                          i=i+1;//PDEBUG(KERN_INFO "plb_sdi_ctrl:iw: %d\n", i);
                          //if(i>500000){break;}
				/*prepare_to_wait(&intr_wait_queue, &wait, TASK_INTERRUPTIBLE);
				if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_WRITER_BUSY_MASK)
					schedule();
				finish_wait(&intr_wait_queue, &wait);
				if (signal_pending (current))  // a signal arrived
					return -ERESTARTSYS; // tell the fs layer to handle it*/
			 } 
                         i=0;
                         while (!PLB_SDI_CONTROLLER_Check_WriterDone_Flg()){i=i+1;
                          //if(i>500000){break;}
                         }
                         do_io_write_32(chage_byte_order_32((unsigned long)0), &(hw_regs->control_status_reg));
                         PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
                         i=i+1;
			retval = __put_user(chage_byte_order_32(do_io_read(&(hw_regs->tick_counter))), (unsigned long __user *)arg);
                        //printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSWRCORSNG out\n");
		break;
	case PLB_SDI_CTRL_IOCSRDWRSNG:      
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDWRSNG \n");
		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
		PLB_SDI_CONTROLLER_Reset_Writer_FIFO();
		PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
		PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();

		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + BUFFER_SIZE)), &(hw_regs->src_start_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (2*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));

		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR), &(hw_regs->dest_start_addr)); // image sensor start address
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
		( PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK | 
		PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_end_addr)));
		}

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
		}
		
	        while (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_READER_BUSY_MASK)//PLB_SDI_CONTROLLER_WRITER_BUSY_MASK) 
                     {
		        prepare_to_wait(&intr_wait_queue, &wait, TASK_INTERRUPTIBLE);
		        if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_READER_BUSY_MASK)//PLB_SDI_CONTROLLER_WRITER_BUSY_MASK)
			        schedule();
		        finish_wait(&intr_wait_queue, &wait);
		        if (signal_pending (current))  // a signal arrived
			        return -ERESTARTSYS; // tell the fs layer to handle it
	        } 

		retval = __put_user(chage_byte_order_32(do_io_read(&(hw_regs->tick_counter))), (unsigned long __user *)arg);
		break;

	case PLB_SDI_CTRL_IOCSRDCONT:       
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDCONT \n");
		if (! capable (CAP_SYS_ADMIN)){
			retval = -EPERM;
			goto out;
		}

		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();

		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + BUFFER_SIZE)), &(hw_regs->src_start_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (5*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (5*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
                ( PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_end_addr)));
		}
		
		break;

	case PLB_SDI_CTRL_IOCSWRCONT:       
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSWRCONT \n");
		if (! capable (CAP_SYS_ADMIN)){
			retval = -EPERM;
			goto out;
		}

		PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
		PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();

		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR), &(hw_regs->dest_start_addr)); // image sensor start address
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
                ( PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
		}
		break;

	case PLB_SDI_CTRL_IOCSRDWRCONT2MEM: 
		printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDWRCONT2MEM \n");
		if (! capable (CAP_SYS_ADMIN)){
			retval = -EPERM;
			goto out;
		}
		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();

		PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
		PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();

                // ISI and VGA are using two separate memory spaces
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + BUFFER_SIZE)), &(hw_regs->src_start_addr)); // VGA
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (5*BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (5*BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));

		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR), &(hw_regs->dest_start_addr)); // image sensor start address
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
		( PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_end_addr)));
		}

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
		}

		break;

	case PLB_SDI_CTRL_IOCSRDWRCONT1MEM: 
		//printk(KERN_INFO "plb_sdi_ctrl: ioctl PLB_SDI_CTRL_IOCSRDWRCONT1MEM \n");
		if (! capable (CAP_SYS_ADMIN)){
			retval = -EPERM;
			goto out;
		}
		
		PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
		PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
		PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();

		PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
		PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();


                // ISI and VGA are using the same memory spaces
		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR), &(hw_regs->src_start_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->src_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->src_trigger_addr));

		do_io_write_32(chage_byte_order_32((unsigned long)DMA_MEM_BASEADDR), &(hw_regs->dest_start_addr)); // image sensor start address
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_end_addr));
		do_io_write_32(chage_byte_order_32((unsigned long)(DMA_MEM_BASEADDR + (BUFFER_SIZE-1))), &(hw_regs->dest_trigger_addr));

		flush_cache_all(); 

		do_io_write_32(chage_byte_order_32((unsigned long)
		( PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK | 
		PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER | PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ )), 
                        &(hw_regs->control_status_reg));

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Reader End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->src_end_addr)));
		}

		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer Start Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_start_addr)));
		}
		if (chage_byte_order_32(do_io_read(&(hw_regs->control_status_reg))) & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK){
		    printk(KERN_ERR "Error(SDI_CONTROLLER): Writer End Address is not alligned! [0x%08X]\n", do_io_read(&(hw_regs->dest_end_addr)));
		}

		break;

        default:  /* redundant, as cmd was checked against MAXNR */
	        return -ENOTTY;
		
	}

out:
	mutex_unlock(&mutex);
	return retval;
	
}
/*
struct mmap_info {
	char *data;
	int reference; 	
};*/
//static struct miscdevice plb_sdi_controller_driver;

static int plb_data_open(struct inode *inode,struct file *filp)
{
	//MOD_INC_USE_COUNT;
       /* struct mmap_info *info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
	//obtain new memory /
    	info->data = (char *)dst_buffer;//get_zeroed_page(GFP_KERNEL);
	//memcpy(info->data, "hello from kernel this is file: ", 32);
	memcpy(info->data , filp->f_dentry->d_name.name, strlen(filp->f_dentry->d_name.name));
	// assign this info struct to the file 
	filp->private_data = info;*/
	return 0;
}

static int plb_data_release(struct inode *inode, struct file *filp)
{
	//MOD_DEC_USE_COUNT;
      /*  struct mmap_info *info = filp->private_data;
	// obtain new memory 
	free_page((unsigned long)info->data);
    	kfree(info);
	filp->private_data = NULL;*/
	return 0;
}
/*
void mmap_open(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference++;
}
void mmap_close(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference--;
}
static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
        struct page *page;
 	struct mmap_info *info;
	info = (struct mmap_info *)vma->vm_private_data;
	if (!info->data) {
		printk("no data\n");
		return NULL;	
	}
 	page = virt_to_page(info->data);
 	get_page(page);
	vmf->page = page;
	return 0;
}
static struct vm_operations_struct plb_vm_ops = {
.open =  mmap_open,
.close = mmap_close,
.fault = mmap_fault,
};
int plb_data_mmap(struct file *filp, struct vm_area_struct *vma)
{
	vma->vm_ops = &plb_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	// assign the file private data to the vm private data 
	vma->vm_private_data = filp->private_data;
	mmap_open(vma);
	return 0;
}*/
static int plb_data_mmap(struct file *file, struct vm_area_struct *vma)
{
	/* mm semaphore is already held within mmap functions */
	PDEBUG(KERN_INFO "plb_sdi_ctrl: call to mmap: vm_start=0x%08lX vm_end=0x%08lX\n", vma->vm_start, vma->vm_end);

	//vma->vm_flags |= VM_RESERVED | VM_IO;
	vma->vm_flags |= VM_RESERVED ;
	if (remap_pfn_range(vma, vma->vm_start, DMA_MEM_BASEADDR >> PAGE_SHIFT, 
				vma->vm_end-vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}


static struct file_operations plb_data_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = plb_data_ioctl,
	.open = plb_data_open,
	.release = plb_data_release,
	.mmap = plb_data_mmap,
};

/*
 * We get to all of the GPIOs through one minor number.  Here's the
 * miscdevice that gets registered for that minor number.
 */
/* 
static struct miscdevice plb_sdi_controller_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "plb_sdi_ctrl",
	.fops = &plb_data_fops
};
*/
// akzare start
/**
 * plb_sdi_controller_get_module: Get the plb_sdi_controller_module for a given module number and base addr
 */
static struct plb_sdi_controller_module * plb_sdi_controller_get_module(int id)
{
	struct plb_sdi_controller_module *module;

	/* if id = -1; then scan for a free id and use that */
	if (id < 0) {
		for (id = 0; id < PLB_SDI_CONTROLLER_NR_MODULES; id++)         
			if (plb_sdi_ctrl_mod[id].mapbase == 0)
				break;
	}

	if ((id < 0) || (id >= PLB_SDI_CONTROLLER_NR_MODULES)) {               
		printk(KERN_WARNING "plb_sdi_ctrl: invalid id: %i\n", id);
		return NULL;
	}

	/* The ID is valid, so get the address of the plb_sdi_controller_module structure */
	module = &plb_sdi_ctrl_mod[id];

	/* Is the structure is already initialized? */
	if (module->mapbase)
		return module;

	/* At this point, we've got an empty plb_sdi_controller_module struct, initialize it */
	spin_lock_init(&module->lock);
	module->membase = NULL;
	module->dma_membase = (unsigned char __iomem *)DMA_MEM_BASEADDR;
	module->reader_fifosize = 256;
	module->writer_fifosize = 256;
	module->iotype = SDIPIO_PORT;
	module->iobase = 1; /* mark module in use */
	module->ops = &plb_sdi_controller_ops;
	module->irq = NO_IRQ;
	module->flags = SDIMF_RES_0;
	module->dev = NULL;
	module->type = MODULE_UNKNOWN;

	return module;
}
/**
 *      plb_sdi_controller_add_one_module - attach a driver-defined module structure
 *      @drv: pointer to the uart low level driver structure for this module
 *      @module: plb_sdi_controller_module structure to use for this module.
 *
 *      This allows the driver to register its own plb_sdi_controller_module structure
 *      with the core driver.  The main purpose is to allow the low
 *      level plb_sdi_controller drivers to expand plb_sdi_controller_module, rather than having yet
 *      more levels of structures.
 */
int plb_sdi_controller_add_one_module(struct plb_sdi_controller_module *module)
{
        struct plb_sdi_controller_state *state;
        int ret = 0;

        module->ops->config_module(module, 0);
        if (!module->ops->type(module))
                return -EINVAL;
//	printk(KERN_INFO "plb_sdi_ctrl: Module TypeBase: %s \n",module->ops->type(module));
        ret = module->ops->startup(module);

        return ret;
}
/**
 *      plb_sdi_controller_remove_one_module - detach a driver defined module structure
 *      @drv: pointer to the plb_sdi_controller low level driver structure for this module
 *      @module: plb_sdi_controller module structure for this module
 *
 *      This unhooks (and hangs up) the specified module structure from the
 *      core driver.  No further calls will be made to the low-level code
 *      for this module.
 */
int plb_sdi_controller_remove_one_module(struct plb_sdi_controller_module *module)
{
        struct plb_sdi_controller_state *state;
        struct plb_sdi_controller_info *info;

        module->ops->shutdown(module);
        module->ops->release_module(module);

        return 0;
}

/* ---------------------------------------------------------------------
 * Port assignment functions (mapping devices to plb_sdi_controller_module structures)
 */

/** plb_sdi_controller_assign: register a plb_sdi_ctrl device with the driver
 *
 * @dev: pointer to device structure
 * @id: requested id number.  Pass -1 for automatic module assignment
 * @base: base address of plb_sdi_ctrl registers
 * @irq: irq number for plb_sdi_ctrl
 *
 * Returns: 0 on success, <0 otherwise
 */
static int  plb_sdi_controller_assign(struct device *dev, int id, u32 base, int irq)
{
	struct plb_sdi_controller_module *module;
	int rc;

	module = plb_sdi_controller_get_module(id);
	if (!module) {
		dev_err(dev, "Cannot get plb_sdi_controller_module structure\n");
		return -ENODEV;
	}

	/* was it initialized for this device? */
	if ((module->mapbase) && (module->mapbase != base)) {
		pr_debug(KERN_DEBUG "plb_sdi_ctrl: addr mismatch; %x != %x\n",
			 module->mapbase, base);
		return -ENODEV;
	}

	module->mapbase = base;
	module->irq = irq;
	module->dev = dev;
	dev_set_drvdata(dev, module);

	/* Register the module */
	rc = plb_sdi_controller_add_one_module(module); 
	if (rc) {
		dev_err(dev, "plb_sdi_controller_add_one_module() failed; err=%i\n", rc);
		module->mapbase = 0;
		dev_set_drvdata(dev, NULL);
		return rc;
	}

	return 0;
}

/** plb_sdi_controller_release: register a plb_sdi_controller device with the driver
 *
 * @dev: pointer to device structure
 */
static int  plb_sdi_controller_release(struct device *dev)
{
	struct plb_sdi_controller_module *module = dev_get_drvdata(dev);
	int rc = 0;

	if (module) {
		rc = plb_sdi_controller_remove_one_module(module);
		dev_set_drvdata(dev, NULL);
		module->mapbase = 0;
	}

	return rc;
}

/* ---------------------------------------------------------------------
 * Platform bus binding
 */

static int  plb_sdi_controller_probe(struct platform_device *pdev)
{
	struct resource *res, *res2;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	res2 = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res2)
		return -ENODEV;

	return plb_sdi_controller_assign(&pdev->dev, pdev->id, res->start, res2->start);
}

static int  plb_sdi_controller_remove(struct platform_device *pdev)
{
	return plb_sdi_controller_release(&pdev->dev);
}

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:plb_sdi_ctrl");

static struct platform_driver plb_sdi_controller_platform_driver = {
	.probe	= plb_sdi_controller_probe,
	.remove	= plb_sdi_controller_remove,
	.driver	= {
		   .owner = THIS_MODULE,
		   .name  = "plb_sdi_ctrl",
                   .of_match_table = plb_sdi_controller_of_match,
		   },
};

/* ---------------------------------------------------------------------
 * OF bus bindings
 */
#if defined(CONFIG_OF)
static int 
plb_sdi_controller_of_probe(struct of_device *op, const struct of_device_id *match)
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

	//return plb_sdi_controller_assign(&op->dev, id ? *id : -1, res.start+3, irq);
}

static int plb_sdi_controller_of_remove(struct of_device *op)
{
	//return plb_sdi_controller_release(&op->dev);
}

static struct of_platform_driver plb_sdi_controller_of_driver = {
	//.owner = THIS_MODULE,
	//.name = "plb_sdi_ctrl",
	//.match_table = plb_sdi_controller_of_match,
	.probe = plb_sdi_controller_of_probe,
	.remove = plb_sdi_controller_of_remove,
	.driver = {
		.name = "plb_sdi_ctrl",
                .owner = THIS_MODULE,
                //.match_table = plb_sdi_controller_of_match,
	},
};

/* Registration helpers to keep the number of #ifdefs to a minimum */
static inline int __init plb_sdi_controller_of_register(void)
{
//	pr_debug("plb_sdi_ctrl: calling of_register_platform_driver()\n");
	printk(KERN_INFO "plb_sdi_ctrl: calling of_register_platform_driver()\n");
	//return of_register_platform_driver(&plb_sdi_controller_of_driver);
      return platform_driver_register(&plb_sdi_controller_of_driver);
}

static inline void __exit plb_sdi_controller_of_unregister(void)
{
	//of_unregister_platform_driver(&plb_sdi_controller_of_driver);
      platform_driver_unregister(&plb_sdi_controller_of_driver);
}
#else /* CONFIG_OF */
/* CONFIG_OF not enabled; do nothing helpers */
static inline int __init plb_sdi_controller_of_register(void) { return 0; }
static inline void __exit plb_sdi_controller_of_unregister(void) { }
#endif /* CONFIG_OF */


// akzare end

static int __init plb_sdi_controller_init(void)
{
	int rtn;
        // akzare start
	// register driver
        //sema_init(&mutex, 1);
        //spin_lock_init(&mr_lock);
	printk(KERN_INFO "plb_sdi_ctrl: calling register_driver()\n");
/*	rtn = misc_register(&plb_sdi_controller_driver);        
	if (rtn)
		goto err_sdi;
*/		
	// Here we register our device - should not fail thereafter
	rtn = register_chrdev(major, "plb_sdi_ctrl", &plb_data_fops);
	if (rtn < 0) {
		goto err_sdi;
	}
	if (major == 0) major = rtn; /* dynamic */
		
	
	//printk(KERN_INFO "plb_sdi_ctrl: calling plb_sdi_controller_of_register()\n");
	//rtn = plb_sdi_controller_of_register();
	//if (rtn)
	//	goto err_of;
		
	printk(KERN_INFO "plb_sdi_ctrl: calling platform_driver_register()\n");
	rtn = platform_driver_register(&plb_sdi_controller_platform_driver);
	if (rtn)
		goto err_plat;
        // akzare end
		

	printk(KERN_INFO "-----------------\nplb_sdi_ctrl: compiled: %s, %s\n", __TIME__, __DATE__);
        unsigned long tmp=0;
	plb_data_ioctl( NULL, PLB_SDI_CTRL_IOCSHWREGNUM, &tmp); 
	plb_data_ioctl( NULL, PLB_SDI_CTRL_IOCGHWREG, &tmp); 
	printk(KERN_INFO "plb_sdi_ctrl: HW Version:Magic: 0x%08X\n", tmp);

//	plb_data_ioctl(NULL, NULL, PLB_SDI_CTRL_IOCSRDWRCONT1MEM, (unsigned long)0);
	
	return 0;
	
// akzare start
err_plat:
	//plb_sdi_controller_of_unregister();
err_of:
//	misc_deregister(&plb_sdi_controller_driver);
	unregister_chrdev(major, "plb_sdi_ctrl");
err_sdi:
	printk(KERN_ERR "registering plb_sdi_controller driver failed: err=%i", rtn);
	return rtn;
// akzare end
	
}

static void __exit
plb_sdi_controller_cleanup(void)
{

	// akzare start
        platform_driver_unregister(&plb_sdi_controller_platform_driver);	
        //plb_sdi_controller_of_unregister();
	// akzare end
        
//	misc_deregister(&plb_sdi_controller_driver);    
	unregister_chrdev(major, "plb_sdi_ctrl");
	
}

EXPORT_NO_SYMBOLS;

module_init(plb_sdi_controller_init);
module_exit(plb_sdi_controller_cleanup);
