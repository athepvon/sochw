cmd_/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o := arm-xilinx-linux-gnueabi-gcc -Wp,-MD,/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/.razorcar_hardware.o.d  -nostdinc -isystem /opt/PetaLinux/petalinux-v2014.2-final/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/../lib/gcc/arm-xilinx-linux-gnueabi/4.8.1/include -I/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/uapi -Iinclude/generated/uapi -include /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -Uarm -msoft-float -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -pg -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(razorcar_hardware)"  -D"KBUILD_MODNAME=KBUILD_STR(razorcar_hardware_ctrl)" -c -o /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/.tmp_razorcar_hardware.o /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.c

source_/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o := /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.c

deps_/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o := \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/arm/include/generated/asm/types.h \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/arch/arm/include/uapi/asm/posix_types.h \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/uapi/asm-generic/posix_types.h \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.h \
  /home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9/include/uapi/linux/ioctl.h \
  arch/arm/include/generated/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \

/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o: $(deps_/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o)

$(deps_/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/Test-version/Mike/drivers/car_hardware/razorcar_hardware.o):
