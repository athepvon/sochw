export PATH=$PATH:/opt/pkg/petalinux-v2014.2-final/tools/linux-i386/arm-xilinx-linux-gnueabi/bin
export CROSS_COMPILE=arm-xilinx-linux-gnueabi-

arm-xilinx-linux-gnueabi-gcc speedTracker.cpp -o speed_arm `pkg-config --cflags --libs opencv`
