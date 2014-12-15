//////////////////////////////////////////////////////////////////////////////
// Filename:          drivers/plb_sdi_controller_v1_00_a/src/plb_sdi_controller.h
// Version:           1.00.a
// Description:       plb_sdi_controller Driver Header File
// Date:              Thu Mar 26 21:03:20 2009 
// By:                University of Potsdam - Department of Computer Sceince 
//                    Felix & akzare
//////////////////////////////////////////////////////////////////////////////

#ifndef PLB_SDI_CONTROLLER_H
#define PLB_SDI_CONTROLLER_H

/***************************** Include Files *******************************/
#ifndef __KERNEL__
#include <stdint.h>
#endif

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#ifndef NULL
#define NULL		0
#endif

#define PLBSDICTRL_DEBUG
/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef PLBSDICTRL_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "plb_sdi_ctrl: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

/*
*********************************************************************************************************
*  data types
*********************************************************************************************************
*/
struct hw_regs_t {
  uint16_t magic;
  uint16_t hw_version;
  uint32_t control_status_reg;
  uint32_t src_start_addr;
  uint32_t src_end_addr;
  uint32_t src_trigger_addr;
  uint32_t dest_start_addr;
  uint32_t dest_end_addr;
  uint32_t dest_trigger_addr;
  uint32_t tick_counter;
  uint32_t pu_control_status_regs[];
} __attribute__((aligned (8))) ;//

//typedef volatile struct hw_regs_t hw_regs_t;

struct hw_regs_imagesensor_config {
  uint16_t xmin;
  uint16_t xmax;
  uint16_t ymin;
  uint16_t ymax;
} __attribute__((aligned (8)));

struct camera_i2c_status_regs_t { 
	uint8_t bits;
	uint8_t read_register_address;
	uint16_t read_data;
}  __attribute__((aligned (8))) ;//__attribute__ ((packed));
//typedef volatile struct camera_i2c_status_regs_t camera_i2c_status_regs_t;

/* camera i2c registers */
struct camera_i2c_config_regs_t { 
	uint32_t control; //config register 31..0 in picsyi2c_mux_light.vhdl
	uint16_t select; // config register 63..48 in picsyi2c_mux_light.vhdl
	uint16_t startReadWrite; // config register 47..32 in picsyi2c_mux_light.vhdl
	
} __attribute__((aligned (8))) ;//__attribute__ ((packed));
//typedef volatile struct camera_i2c_config_regs_t camera_i2c_config_regs_t;

/* control module bits */
#define CAM_I2C_READ (0x0)
#define CAM_I2C_WRITE (0x80)
//selection bits
#define CAM_I2C_SELECT_0 (0x8000)
#define CAM_I2C_SELECT_1 (0x4000)
//start bit
#define CAM_I2C_START 0x8000
//status bits
#define CAM_I2C_BUSY_BIT  (0x80) //first bit from left, as in VHDL slv register definition
#define CAM_I2C_ERROR_BIT (0x40)
#define CAM_I2C_VALID_BIT (0x20)
#define CAM_I2C_SELECT_0_READ (0x10)
#define CAM_I2C_SELECT_1_READ (0x08)
/* limit tryings */
#define CAM_I2C_READ_NUM_RETRY 7
#define CAM_I2C_WRITE_NUM_RETRY 7
/* ERRORS */
#define CAM_I2C_ERROR_NOT_VALID 0x1
#define CAM_I2C_ERROR_DATA_NOT_VALID 0x2
/* Image sensor register address definitions */
#define MT9T031_REG_CHIP_VERSION_1 0x00
#define MT9T031_REG_ROW_START 0x01
#define MT9T031_REG_COLUMN_START 0x02
#define MT9T031_REG_ROW_SIZE 0x03
#define MT9T031_REG_COLUMN_SIZE 0x04
#define MT9T031_REG_HORIZENTAL_BLANKING 0x05
#define MT9T031_REG_VERTICAL_BLANKING 0x06
#define MT9T031_REG_OUTPUT_CONTROL 0x07
#define MT9T031_REG_SHUTTER_WIDTH_UPPER 0x08
#define MT9T031_REG_SHUTTER_WIDTH 0x09
#define MT9T031_REG_PIXEL_CLK_CONTROL 0x0A
#define MT9T031_REG_FRAME_RESTART 0x0B
#define MT9T031_REG_SHUTTER_DELAY 0x0C
#define MT9T031_REG_RESET 0x0D
#define MT9T031_REG_READ_MODE_1 0x1E
#define MT9T031_REG_READ_MODE_2 0x20
#define MT9T031_REG_READ_MODE_3 0x21
#define MT9T031_REG_ROW_ADDRESS_MODE 0x22
#define MT9T031_REG_COLUMN_ADDRESS_MODE 0x23
#define MT9T031_REG_GREEN_1_GAIN 0x2B
#define MT9T031_REG_BLUE_GAIN 0x2C
#define MT9T031_REG_RED_GAIN 0x2D
#define MT9T031_REG_GREEN_2_GAIN 0x2E
#define MT9T031_REG_TEST_DATA 0x32
#define MT9T031_REG_GLOBAL_GAIN 0x35
#define MT9T031_REG_BLACK_LEVEL 0x49
#define MT9T031_REG_ROW_BLACK_DEFAULT_OFFSET 0x4B
#define MT9T031_REG_BLC_DELTA_THRESHOLDS 0x5D
#define MT9T031_REG_CAL_THRESHOLDS 0x5F
#define MT9T031_REG_CAL_GREEN_1_OFFSET 0x60
#define MT9T031_REG_CAL_GREEN_2_OFFSET 0x61
#define MT9T031_REG_BLACK_LEVEL_CALIBRATION 0x62
#define MT9T031_REG_RED_OFFSET 0x63
#define MT9T031_REG_BLUE_OFFSET 0x64
#define MT9T031_REG_CHIP_ENABLE 0xF8
#define MT9T031_REG_CHIP_VERSION_2 0xFF


struct replace_order_of_byte{
  unsigned int byte1 : 8;
  unsigned int byte2 : 8;
  unsigned int byte3 : 8;
  unsigned int byte4 : 8;
} __attribute__((aligned (8))) ;//__attribute__ ((packed));

//typedef volatile struct replace_order_of_byte replace_order_of_byte;

struct plb_data_buffer_t {
	uint32_t src_start_addr;
	uint32_t src_end_addr;
	uint32_t dest_start_addr;
	uint32_t dest_end_addr;
	pid_t pid;
};

/************************** Constant Definitions ***************************/
#define PLB_DATA_GET_DMA_BUFFER_SIZE 10

/* Definitions for peripheral PLB_SDI_CONTROLLER_MODULE_0 */
#define XPAR_PLB_SDI_CONTROLLER_MODULE_0_DEVICE_ID 0
#define XPAR_PLB_SDI_CONTROLLER_MODULE_0_BASEADDR 0x76000000
#define XPAR_PLB_SDI_CONTROLLER_MODULE_0_HIGHADDR 0x7600FFFF


/* Definitions for peripheral PLB_SDI_CONTROLLER_1 */
#define XPAR_PLB_SDI_CONTROLLER_1_DEVICE_ID 1
#define XPAR_PLB_SDI_CONTROLLER_1_BASEADDR 0x76010000
#define XPAR_PLB_SDI_CONTROLLER_1_HIGHADDR 0x7601FFFF

#define XPAR_PLB_DATA_MODULE_0_ADDRRANGE	(XPAR_PLB_SDI_CONTROLLER_MODULE_0_HIGHADDR - XPAR_PLB_SDI_CONTROLLER_MODULE_0_BASEADDR + 1)

#define XPAR_PLB_DATA_1_ADDRRANGE	(XPAR_PLB_SDI_CONTROLLER_1_HIGHADDR - XPAR_PLB_SDI_CONTROLLER_1_BASEADDR + 1)

/* from 0x7600000 up to 0x8000000 (10M) of memory is reserved for DMA transactions and is hidded from Linux Kernel */
#define DMA_MEM_BASEADDR 0x37600000
#define DMA_MEM_HIGHADDR 0x38000000
#define DMA_MEM_REGION (DMA_MEM_HIGHADDR - DMA_MEM_BASEADDR + 1)

#define XPAR_XPS_INTC_0_PLB_SDI_CONTROLLER_MODULE_0_IP2INTC_IRPT_INTR 0

#define X_SIZE 640//2048//640
#define Y_SIZE 480//1536//480

#define BUFFER_SIZE (X_SIZE*Y_SIZE) /* Size of VGA buffer in 32 bit words */

/*#define BUFFER_SIZE (640*480/4)*/

#define PLB_SDI_CONTROLLER_REGION   0X10000
#define MODULE_UNKNOWN    0
#define MODULE_PLB_SDI_CONTROLLER   74
#define PLB_SDI_CONTROLLER_NR_MODULES	4


/*
 * Ioctl definitions
 */

/* Use 's' as magic number */
#define PLB_SDI_CTRL_IOC_MAGIC  'q' //'s'
/* Please use a different 8-bit number in your code */

/*
 * S means "Set" through a ptr, "Set: arg points to the value"
 */
#define PLB_SDI_CTRL_IOCSHWREGNUM     _IOW(PLB_SDI_CTRL_IOC_MAGIC,  0, int)
#define PLB_SDI_CTRL_IOCGHWREG        _IOR(PLB_SDI_CTRL_IOC_MAGIC,  1, unsigned long)
#define PLB_SDI_CTRL_IOCSHWREG        _IOW(PLB_SDI_CTRL_IOC_MAGIC,  2, unsigned long)
#define PLB_SDI_CTRL_IOCSRDSNG        _IOR(PLB_SDI_CTRL_IOC_MAGIC,  3, unsigned long)
#define PLB_SDI_CTRL_IOCSWRSNG        _IOR(PLB_SDI_CTRL_IOC_MAGIC,  4, unsigned long)
#define PLB_SDI_CTRL_IOCSWRCORSNG     _IOR(PLB_SDI_CTRL_IOC_MAGIC,  5, unsigned long)
#define PLB_SDI_CTRL_IOCSRDWRSNG      _IOR(PLB_SDI_CTRL_IOC_MAGIC,  6, unsigned long)
#define PLB_SDI_CTRL_IOCSRDCONT       _IO(PLB_SDI_CTRL_IOC_MAGIC,   7)
#define PLB_SDI_CTRL_IOCSWRCONT       _IO(PLB_SDI_CTRL_IOC_MAGIC,   8)
#define PLB_SDI_CTRL_IOCSRDWRCONT2MEM _IO(PLB_SDI_CTRL_IOC_MAGIC,   9)
#define PLB_SDI_CTRL_IOCSRDWRCONT1MEM _IO(PLB_SDI_CTRL_IOC_MAGIC,   10)

/* ... more to come */

#define PLB_SDI_CTRL_IOC_MAXNR 10

/************************** Constant Definitions ***************************/

/**
 * Control Register
 * -- PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK  	: defined logic start commned to reader module in single mode (Bit-0)
 * -- PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK  	: defined logic start commned to writer module in single mode (Bit-1)
 * -- PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK   	: defined logic clear commned to reader module FIFO (Bit-2)
 * -- PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK   	: defined logic clear commned to writer module FIFO (Bit-3)
 * -- PLB_SDI_CONTROLLER_RST_PU_MASK   			: defined logic reset commned to processor units (Bit-4)
 * -- PLB_SDI_CONTROLLER_STRT_READER_CONT_MASK 		: defined logic start commned to reader module in continues mode (Bit-5)
 * -- PLB_SDI_CONTROLLER_STRT_WRITER_CONT_MASK 		: defined logic start commned to writer module in continues mode (Bit-6)
 * -- PLB_SDI_CONTROLLER_FORCE_GO_MASK 			: defined logic Go commned to start operation of the reader/writer modules (Bit-7)
 * -- PLB_SDI_CONTROLLER_MST_PLB_IP2BUS_BE_MASK 	: defined logic to control master PLB IP2BUS_BE signal (Bits-8 to 15)
 * -- PLB_SDI_CONTROLLER_MST_PLB_CNTL_BUS_LOCK_MASK 	: defined logic to control master PLB BUS LOCK signal (Bit-16)
 * -- PLB_SDI_CONTROLLER_MST_PLB_CNTL_BURST_MASK 	: defined logic to control master PLB BURST signal (Bit-17)
 * -- PLB_SDI_CONTROLLER_TICK_COUNTR_CTRL_MASK 		: defined logic control commned to Tick-Counter (Bits-18 to 21)
 */  
#define PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK 	(0x80000000UL)
#define PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK 	(0x40000000UL)
#define PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK 	(0x20000000UL)
#define PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK 	(0x10000000UL)
#define PLB_SDI_CONTROLLER_RST_PU_MASK 			(0x08000000UL)
#define PLB_SDI_CONTROLLER_START_TICK_COUNTER           (0x04000000UL)
#define PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER           (0x02000000UL)
#define PLB_SDI_CONTROLLER_STOP_TICK_COUNTER            (0x01000000UL)
#define PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_RD_IRQ  (0x00800000UL)
#define PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ  (0x00400000UL)
#define PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK	(0x00200000UL)
#define PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK   (0x00100000UL)
#define PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK 		(0x00000001UL)



/**
 * Status Register
 * -- PLB_SDI_CONTROLLER_READER_DONE_MASK  		: defined logic status of reader done single (Bit-0)
 * -- PLB_SDI_CONTROLLER_READER_BUSY_MASK  		: defined logic status of reader busy single (Bit-1)
 * -- PLB_SDI_CONTROLLER_WRITER_DONE_MASK  		: defined logic status of writer done single (Bit-2)
 * -- PLB_SDI_CONTROLLER_WRITER_BUSY_MASK  		: defined logic status of writer busy single (Bit-3)
 * -- PLB_SDI_CONTROLLER_STATUS_ERROR_MASK  		: defined logic status of error register (Bits-24 to 31)
 */ 
#define PLB_SDI_CONTROLLER_READER_DONE_MASK 		(0x80000000UL)
#define PLB_SDI_CONTROLLER_READER_BUSY_MASK 		(0x40000000UL)
#define PLB_SDI_CONTROLLER_WRITER_DONE_MASK 		(0x20000000UL)
#define PLB_SDI_CONTROLLER_WRITER_BUSY_MASK 		(0x10000000UL)
#define PLB_SDI_CONTROLLER_RD_DONE_RD_FIFOEMPTY_MASK 	(0x08000000UL)
#define PLB_SDI_CONTROLLER_READER_END_OF_BLOCK_MASK 	(0x04000000UL)
#define PLB_SDI_CONTROLLER_WRITER_END_OF_BLOCK_MASK 	(0x02000000UL)
#define PLB_SDI_CONTROLLER_ERROR_REG_MASK 		(0x000000FFUL)
#define PLB_SDI_CONTROLLER_ERROR_MST_ERROR_MASK		(0x00000080UL)
#define PLB_SDI_CONTROLLER_ERROR_MST_TIMEOUT_MASK	(0x00000040UL)
#define PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK	(0x00000020UL)
#define PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK	(0x00000010UL)
#define PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK	(0x00000008UL)
#define PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK	(0x00000004UL)
#define PLB_SDI_CONTROLLER_ERROR_NOT_USED0_MASK		(0x00000002UL)
#define PLB_SDI_CONTROLLER_ERROR_NOT_USED1_MASK         (0x00000001UL)

/**
 * Defined Logic Slave Module Control Register Masks
 * -- PLB_SDI_CONTROLLER_HDWR_VER_MASK  		: defined logic hardware version (Bits-0 to 15)
 * -- PLB_SDI_CONTROLLER_MAGIC_WORD_MASK  		: defined logic magic word (Bits-16 to 31)
 */ 
#define PLB_SDI_CONTROLLER_HW_VER_MASK  		(0xFFFF0000UL)
#define PLB_SDI_CONTROLLER_MAGIC_WORD_MASK 		(0x0000FFFFUL)

/**
 * Interrupt Controller Space Offsets
 * -- INTR_DISR  : device (peripheral) interrupt status register
 * -- INTR_DIPR  : device (peripheral) interrupt pending register
 * -- INTR_DIER  : device (peripheral) interrupt enable register
 * -- INTR_DIIR  : device (peripheral) interrupt id (priority encoder) register
 * -- INTR_DGIER : device (peripheral) global interrupt enable register
 * -- INTR_ISR   : ip (user logic) interrupt status register
 * -- INTR_IER   : ip (user logic) interrupt enable register
 */
#define PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET (0x00000100)
#define PLB_SDI_CONTROLLER_INTR_DISR_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000000)
#define PLB_SDI_CONTROLLER_INTR_DIPR_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000004)
#define PLB_SDI_CONTROLLER_INTR_DIER_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000008)
#define PLB_SDI_CONTROLLER_INTR_DIIR_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000018)
#define PLB_SDI_CONTROLLER_INTR_DGIER_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x0000001C)
#define PLB_SDI_CONTROLLER_INTR_IPISR_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000020)
#define PLB_SDI_CONTROLLER_INTR_IPIER_OFFSET (PLB_SDI_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000028)

/**
 * Interrupt Controller Masks
 * -- INTR_TERR_MASK : transaction error
 * -- INTR_DPTO_MASK : data phase time-out
 * -- INTR_IPIR_MASK : ip interrupt requeset
 * -- INTR_RFDL_MASK : read packet fifo deadlock interrupt request
 * -- INTR_WFDL_MASK : write packet fifo deadlock interrupt request
 * -- INTR_IID_MASK  : interrupt id
 * -- INTR_GIE_MASK  : global interrupt enable
 * -- INTR_NOPEND    : the DIPR has no pending interrupts
 */
#define INTR_TERR_MASK (0x00000001UL)
#define INTR_DPTO_MASK (0x00000002UL)
#define INTR_IPIR_MASK (0x00000004UL)
#define INTR_RFDL_MASK (0x00000020UL)
#define INTR_WFDL_MASK (0x00000040UL)
#define INTR_IID_MASK (0x000000FFUL)
#define INTR_GIE_MASK (0x80000000UL)
#define INTR_NOPEND (0x80)



#define PLB_SDI_CONTROLLER_IRQ_READER_DONE_MASK           (0x00000001UL)
#define PLB_SDI_CONTROLLER_IRQ_WRITER_DONE_MASK           (0x00000002UL)
#define PLB_SDI_CONTROLLER_IRQ_READER_FLUSHED_MASK        (0x00000004UL)
#define PLB_SDI_CONTROLLER_IRQ_READER_TRIGGER_ADDR_MASK   (0x00000008UL)
#define PLB_SDI_CONTROLLER_IRQ_WRITER_TRIGGER_ADDR_MASK   (0x00000010UL)


/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a PLB_SDI_CONTROLLER register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PLB_SDI_CONTROLLER device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void PLB_SDI_CONTROLLER_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
 
/*
#define PLB_SDI_CONTROLLER_mWriteReg(BaseAddress, RegOffset, Data) \
 	XIo_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))
*/  
#define PLB_SDI_CONTROLLER_mWriteReg(BaseAddress, RegOffset, Data) \
  { (*(volatile uint32_t *)((BaseAddress) + (RegOffset)) = (uint32_t)(Data)); }
/**
 *
 * Read a value from a PLB_SDI_CONTROLLER register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PLB_SDI_CONTROLLER device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 PLB_SDI_CONTROLLER_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */

/* 
#define PLB_SDI_CONTROLLER_mReadReg(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (RegOffset))
*/
#define PLB_SDI_CONTROLLER_mReadReg(BaseAddress, RegOffset) \
 	(*((volatile uint32_t *)((BaseAddress) + (RegOffset))))
/**
 *
 * Check status of PLB_SDI_CONTROLLER user logic master module.
 *
 * @param   BaseAddress is the base address of the PLB_SDI_CONTROLLER device.
 *
 * @return  Status is the result of status checking.
 *
 * @note
 * C-style signature:
 * 	bool PLB_SDI_CONTROLLER_mMasterDone(Xuint32 BaseAddress)
 * 	bool PLB_SDI_CONTROLLER_mMasterBusy(Xuint32 BaseAddress)
 * 	bool PLB_SDI_CONTROLLER_mMasterError(Xuint32 BaseAddress)
 * 	bool PLB_SDI_CONTROLLER_mMasterTimeout(Xuint32 BaseAddress)
 *
 */


/************************** Function Prototypes ****************************/


void PLB_SDI_CONTROLLER_Set_BaseAddress(uint32_t BaseAddress);
int PLB_SDI_CONTROLLER_Writer_Done();
int PLB_SDI_CONTROLLER_Reader_Done();
int PLB_SDI_CONTROLLER_Reader_Done_ReaderFIFO_Empty();
int PLB_SDI_CONTROLLER_Reader_End_of_Block_Done();
int PLB_SDI_CONTROLLER_Writer_End_of_Block_Done();
int PLB_SDI_CONTROLLER_Is_MST_Error();
int PLB_SDI_CONTROLLER_Is_MST_TIMEOUT_Error();
int PLB_SDI_CONTROLLER_Is_Reader_StartAddr_Error();
int PLB_SDI_CONTROLLER_Is_Reader_EndAddr_Error();
int PLB_SDI_CONTROLLER_Is_Writer_StartAddr_Error();
int PLB_SDI_CONTROLLER_Is_Writer_EndAddr_Error();
uint32_t PLB_SDI_CONTROLLER_Get_HDWR_Ver();
uint32_t PLB_SDI_CONTROLLER_Get_Src_Start_Addr();
uint32_t PLB_SDI_CONTROLLER_Get_Src_End_Addr();
uint32_t PLB_SDI_CONTROLLER_Get_Dst_Start_Addr();
uint32_t PLB_SDI_CONTROLLER_Get_Dst_End_Addr();
uint32_t PLB_SDI_CONTROLLER_Get_Tick_Cntr();
void PLB_SDI_CONTROLLER_Set_Src_Start_Addr(uint32_t SrcAddress_Reader);
void PLB_SDI_CONTROLLER_Set_Src_End_Addr(uint32_t SrcAddress_Reader);
void PLB_SDI_CONTROLLER_Set_Dst_Start_Addr(uint32_t DstAddress_Reader);
void PLB_SDI_CONTROLLER_Set_Dst_End_Addr(uint32_t DstAddress_Reader);
void PLB_SDI_CONTROLLER_Reset_Reader_FIFO();
void PLB_SDI_CONTROLLER_Reset_Writer_FIFO();
void PLB_SDI_CONTROLLER_Master_StartReaderWriter_SnapShot();
void PLB_SDI_CONTROLLER_Master_StartWriter_SnapShot();
void PLB_SDI_CONTROLLER_Master_StartReader_SnapShot();
void PLB_SDI_CONTROLLER_Master_StartReaderWriter_Continous();
void PLB_SDI_CONTROLLER_Master_StartWriter_Continous();
void PLB_SDI_CONTROLLER_Master_StartReader_Continous();

void PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg();
void PLB_SDI_CONTROLLER_Reset_WriterDone_Flg();
void PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg();
void PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg();
void PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg();

int PLB_SDI_CONTROLLER_Check_ReaderDone_Flg();
int PLB_SDI_CONTROLLER_Check_WriterDone_Flg();
int PLB_SDI_CONTROLLER_Check_ReaderFlushed_Flg();
int PLB_SDI_CONTROLLER_Check_ReaderTriggerAdrr_Flg();
int PLB_SDI_CONTROLLER_Check_WriterTriggerAdrr_Flg();

/**
 *
 * Enable all possible interrupts from PLB_SDI_CONTROLLER device.
 *
 * @param   baseaddr_p is the base address of the PLB_SDI_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_SDI_CONTROLLER_EnableInterrupt(void * baseaddr_p);

/**
 *
 * Example interrupt controller handler.
 *
 * @param   baseaddr_p is the base address of the PLB_SDI_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_SDI_CONTROLLER_Intr_DefaultHandler(void * baseaddr_p);
void PLB_SDI_CONTROLLER_Intr_Handler(void * baseaddr_p);
void PLB_SDI_CONTROLLER_DisableInterrupt(void * baseaddr_p);


#endif // PLB_SDI_CONTROLLER_H
