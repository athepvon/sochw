obj-m := razorcar_hardware_ctrl.o
razorcar_hardware_ctrl-objs += razorcar_hardware.o razorcar_hardware_ctrl_main.o

KERNELDIR := /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9
 
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-

clean:
	rm -rf .tmp_versions
	rm -rf razorcar_hardware_ctrl.ko razorcar_hardware_ctrl.mod.c razorcar_hardware_ctrl.mod.o razorcar_hardware_ctrl.o Modules.symvers .razorcar_hardware_ctrl.ko.cmd .razorcar_hardware_ctrl.mod.o.cmd .razorcar_hardware_ctrl.o.cmd *.o *.cmd

