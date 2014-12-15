/*****************************************************************************
* Filename:          /home/fyonga/Documents/Classes/TA/Embedded_Systems/EDK/RazorCar_Camera_AXI_Sobel/drivers/razorcar_hardware_v1_01_a/src/razorcar_hardware.h
* Version:           1.01.a
* Description:       razorcar_hardware Driver Header File
* Date:              Wed Nov 20 22:10:42 2013 (by Create and Import Peripheral Wizard)
*****************************************************************************/

#ifndef RAZORCAR_HARDWARE_H
#define RAZORCAR_HARDWARE_H

/***************************** Include Files *******************************/

#ifndef __KERNEL__
#include <stdint.h>
#endif
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#ifndef NULL
#define NULL		0
#endif

/*
 * Ioctl definitions
 */

/* Use 's' as magic number */
#define RAZOR_CAR_IOC_MAGIC  'q' //'s'
/* Please use a different 8-bit number in your code */

/*
 * S means "Set" through a ptr, "Set: arg points to the value"
 */
#define RAZOR_CAR_IOC_LIGHT       _IOW(RAZOR_CAR_IOC_MAGIC,  0, unsigned long) //use mode & value for a specific light and a specific value
#define RAZOR_CAR_IOC_DRIVING_MODE      _IOW(RAZOR_CAR_IOC_MAGIC,  1, unsigned long)
#define RAZOR_CAR_IOC_STEERING_STRAIGHT     _IOW(RAZOR_CAR_IOC_MAGIC,  2, unsigned long)
#define RAZOR_CAR_IOC_STEERING_LEFT      _IOW(RAZOR_CAR_IOC_MAGIC,  3, unsigned long)
#define RAZOR_CAR_IOC_STEERING_RIGHT      _IOW(RAZOR_CAR_IOC_MAGIC,  4, unsigned long)
#define RAZOR_CAR_IOC_SPEED_STOP     _IOW(RAZOR_CAR_IOC_MAGIC,  5, unsigned long)
#define RAZOR_CAR_IOC_SPEED_FORWARD      _IOW(RAZOR_CAR_IOC_MAGIC,  6, unsigned long)
#define RAZOR_CAR_IOC_SPEED_BACKWARD      _IOW(RAZOR_CAR_IOC_MAGIC,  7, unsigned long)
#define RAZOR_CAR_IOC_READ_FRONT      _IOR(RAZOR_CAR_IOC_MAGIC,  8, unsigned long)
#define RAZOR_CAR_IOC_READ_BACK      _IOR(RAZOR_CAR_IOC_MAGIC,  9, unsigned long)
#define RAZOR_CAR_IOC_GET_STEERING      _IOR(RAZOR_CAR_IOC_MAGIC,  10, unsigned long)
#define RAZOR_CAR_IOC_GET_SPEED     _IOR(RAZOR_CAR_IOC_MAGIC,  11, unsigned long)
/* ... more to come */
#define razorcar_hardware_NR_MODULES 2
#define razorcar_hardware_REGION 0xFFFF
#define RAZOR_CAR_IOC_MAXNR 11
/* Definitions for peripheral PLB_SDI_CONTROLLER_MODULE_0 */
#define XPAR_RAZOR_CAR_0_DEVICE_ID 0
#define XPAR_RAZOR_CAR_0_BASEADDR 0xA4200000
#define XPAR_RAZOR_CAR_0_HIGHADDR 0xA420FFFF
#define MODULE_razorcar_hardware   74
#define MODULE_UNKNOWN    0
#define XPAR_RAZOR_CAR_0_ADDRRANGE	(XPAR_RAZOR_CAR_0_HIGHADDR - XPAR_RAZOR_CAR_0_BASEADDR + 1)
//typedef signed char int8_t ;
//typedef unsigned char uint8_t ;

struct razorcar_hw_regs_t {
  /*Register 0
   * 0 - 15  => adc values
   * 16      => adc busy bit
   * 24      => adc ready bit
   */
  uint32_t adc_reg0;

  /*Register 1
   * 0       => adc start
   * 8 - 10  => adc channel number
   */
  uint32_t  adc_reg1;

  /*Register 2
   * 0 - 7   => LED Values
   * 8     	 => LED enable
   * 16 - 18 => Buzzer frequency
   * 24      => Buzzer enable
   */
   uint32_t leds_buzzer;

   /* Register 3
    * 0 - 6   => Steering inputs bits
    * 8 - 14  => Speed inputs bits
    * 16 - 17 => mode
    * 24 -    => Buttons hardware interrupt enable
    * 25 -    => Switches hardware interrupt enable
    */
   uint32_t steering_speed;

  /* Register 4
   * 0 - 1   => Buttons value
   * 8 - 11  => SWITCHE1 values
   * 16 - 17 => BTN_Intr & SW_Intr - for debugging purpose
   */
   uint32_t bt_sw;

   /*Register 5*/
   uint32_t actual_speed_values;/*given by the accelerometer*/

} __attribute__((aligned (8)));// *razorcar_hw_regs;// = (struct razorcar_hw_regs_t *)XPAR_RAZORCAR_HARDWARE_0_BASEADDR;

//typedef volatile struct razorcar_hw_regs_t razorcar_hw_regs_t;

/************************** Constant Definitions ***************************/
/******************************** LIGHTS **********************************/
#define RAZORCAR_HARDWARE_ENABLE_LIGHT_ID   0x00000100
#define RAZORCAR_HARDWARE_ENABLE_LIGHT_MASK 0xFFFFFEFF
#define RAZORCAR_HARDWARE_SET_LIGHT_MASK    0xFFFFFF00

/* VALUES */
#define REAR_LIGHT          0xF0
#define FRONT_LIGHT         0x0F
#define FRONT_RIGHT_LIGHT   0x03
#define FRONT_LEFT_LIGHT    0x0C
#define REAR_LEFT_LIGHT     0x30
#define REAR_RIGHT_LIGHT    0xC0

/* MODE */
#define OFF          		0x00
#define ON           		0xFF
#define BLINKING     		0x55
#define INV_BLINKING 		0xAA

/******************************** BUZZER **********************************/
#define RAZORCAR_HARDWARE_ENABLE_BUZZER_ID   0x01000000
#define RAZORCAR_HARDWARE_ENABLE_BUZZER_MASK 0xFEFFFFFF
#define RAZORCAR_HARDWARE_SET_BUZZER_MASK    0xFFF0FFFF

/*************************** STEERING AND SPEED***************************/
#define RAZORCAR_HARDWARE_SET_STEERING_MASK 	 0xFFFFFF00
#define RAZORCAR_HARDWARE_GET_STEERING_MASK 	 0x000000FF
#define RAZORCAR_HARDWARE_SET_SPEED_MASK    	 0xFFFF00FF
#define RAZORCAR_HARDWARE_GET_SPEED_MASK    	 0x0000FF00
#define RAZORCAR_HARDWARE_SET_MODE_MASK     	 0xFFFCFFFF
#define RAZORCAR_HARDWARE_GET_MODE_MASK     	 0x00030000
#define RAZORCAR_HARDWARE_RESUME_SEMI_AUTO_MASK  0xFFFBFFFF
#define RAZORCAR_HARDWARE_RESUME_SEMI_AUTO_ID    0x00040000

#define STEERING_MIN    -50
#define STEERING_MAX     50
#define STEERING_OFFSET  50

#define SPEED_MIN     -45
#define SPEED_MAX     45
#define SPEED_OFFSET  45

/* DIRECTION */
#define LEFT      0
#define STRAIGHT  1
#define RIGHT     2
#define FORWARD   0
#define STOP      1
#define BACKWARD  2

/* DRIVING MODE */
#define AUTO   	  0
#define MANUAL 	  1
#define SEMI_AUTO 2

/*************************** INFRARED SENSORS ***************************/
#define RAZORCAR_HARDWARE_HW_IR_START_MASK   0xFFFFFFFE /* Mask of the START signal */
#define RAZORCAR_HARDWARE_HW_IR_BUSY_MASK    0x00010000 /* Mask of the BUSY signal */
#define RAZORCAR_HARDWARE_HW_IR_CHAN_MASK    0xFFFFF8FF /* Mask of the CHANNEL signal */
#define RAZORCAR_HARDWARE_HW_IR_VALUES_MASK  0x00000FFF /* Mask of the IR values */

#define FRONT_SENSOR 0
#define REAR_SENSOR  1

/************************** BUTTONS AND SWITCHES ***********************/
#define RAZORCAR_HARDWARE_HW_BTNINTR_ID      	0x01000000 /* ID of the hardware BTN Interrupt */
#define RAZORCAR_HARDWARE_HW_BTNINTR_MASK    	0xFEFFFFFF /* MASK of the hardware BTN Interrupt */
#define RAZORCAR_HARDWARE_HW_SWINTR_ID       	0x02000000 /* ID of the hardware SW Interrupt */
#define RAZORCAR_HARDWARE_HW_SWINTR_MASK   	 	0xFDFFFFFF /* MASK of the hardware SW Interrupt */
#define RAZORCAR_HARDWARE_HW_GET_BTNVAL_MASK 	0x00000003 /* Get the value of the Buttons */
#define RAZORCAR_HARDWARE_HW_GET_SWVAL_MASK  	0x00000F00 /* Get the value of the SWITCH1 */
#define RAZORCAR_HARDWARE_HW_GET_BTSW_INTR_MASK 0x00030000 /* Get the value of the SWITCH1 */

/**
 * User Logic Slave Space Offsets
 * -- SLV_REG0 : user logic slave module register 0
 * -- SLV_REG1 : user logic slave module register 1
 * -- SLV_REG2 : user logic slave module register 2
 * -- SLV_REG3 : user logic slave module register 3
 * -- SLV_REG4 : user logic slave module register 4
 * -- SLV_REG5 : user logic slave module register 5
 */
#define RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET (0x00000000)
#define RAZORCAR_HARDWARE_SLV_REG0_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x00000000)
#define RAZORCAR_HARDWARE_SLV_REG1_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x00000004)
#define RAZORCAR_HARDWARE_SLV_REG2_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x00000008)
#define RAZORCAR_HARDWARE_SLV_REG3_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x0000000C)
#define RAZORCAR_HARDWARE_SLV_REG4_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x00000010)
#define RAZORCAR_HARDWARE_SLV_REG5_OFFSET (RAZORCAR_HARDWARE_USER_SLV_SPACE_OFFSET + 0x00000014)

/**************************** Type Definitions *****************************/


/***************** Macros (Functions) Definitions *******************/

/**
 *
 * Write a value to a RAZORCAR_HARDWARE register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the RAZORCAR_HARDWARE device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void RAZORCAR_HARDWARE_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define RAZORCAR_HARDWARE_mWriteReg(BaseAddress, RegOffset, Data) \
 	 { (*(volatile uint32_t *)((BaseAddress) + (RegOffset)) = (uint32_t)(Data)); }

/**
 *
 * Read a value from a RAZORCAR_HARDWARE register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the RAZORCAR_HARDWARE device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 RAZORCAR_HARDWARE_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define RAZORCAR_HARDWARE_mReadReg(BaseAddress, RegOffset) \
 	(*((volatile uint32_t *)((BaseAddress) + (RegOffset))))


/**
 *
 * Write/Read 32 bit value to/from RAZORCAR_HARDWARE user logic slave registers.
 *
 * @param   BaseAddress is the base address of the RAZORCAR_HARDWARE device.
 * @param   RegOffset is the offset from the slave register to write to or read from.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	void RAZORCAR_HARDWARE_mWriteSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Value)
 * 	Xuint32 RAZORCAR_HARDWARE_mReadSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define MEM_ADDR 0x39000000
/************************** Function Prototypes ****************************/
//int init_timer();
void RazorCar_set_BaseAdress(uint32_t BaseAddress);
//void get_size(void);
void init_razorcar_platform(uint32_t BaseAddress);

//void delay_us(uint32_t time);
//void delay_ms(uint32_t time);
/**************************** Infrared module *****************************************/
int get_sensor_values(uint8_t channel);
/**************************** Lights *****************************************/
void set_light(uint8_t mode, uint8_t value);
void set_all_light(uint8_t mode);
void enable_light(void);
void disable_light(void);
/**************************** Buzzer *****************************************/
void enable_buzzer(void);
void disable_buzzer(void);
void set_buzzer(uint8_t frequency);
/**************************** Steering & Speed *****************************************/
void set_driving_mode(uint8_t mode);
uint8_t get_driving_mode(void);
void set_steering(uint8_t direction, uint8_t angle);
int8_t   get_steering(void);
int8_t   get_speed(void);
void set_speed(uint8_t direction, uint8_t speed);
void resume_semi_auto_driving(void);
/**************************** Buttons & Switches *****************************************/
void enable_hw_buttons_interrupt(void);
void disable_hw_buttons_interrupt(void);
void enable_hw_switches_interrupt(void);
void disable_hw_switches_interrupt(void);

uint8_t get_buttons_values(void);
uint8_t get_switches_values(void);
uint8_t get_hw_interrupts_state(void);/*for Debugging purpose*/


#endif /** RAZORCAR_HARDWARE_H */
