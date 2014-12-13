obj-m := plb_sdi_ctrl.o
plb_sdi_ctrl-objs += plb_sdi_controller.o plb_sdi_ctrl_main.o

KERNELDIR := /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9
 
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-

clean:
	rm -rf .tmp_versions
	rm -rf plb_sdi_ctrl.ko plb_sdi_ctrl.mod.c plb_sdi_ctrl.mod.o plb_sdi_ctrl_main.o plb_sdi_controller.o plb_sdi_ctrl.o Modules.symvers .plb_sdi_ctrl.ko.cmd .plb_sdi_ctrl.mod.o.cmd .plb_sdi_ctrl.o.cmd

