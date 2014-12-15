#export PATH='/opt/pkg/petalinux-v2014.2-final/tools/linux-i386/arm-xilinx-linux-gnueabi/bin':$PATH
#export CROSS_COMPILE=arm-xilinx-linux-gnueabi-

arm-xilinx-linux-gnueabi-g++ -o speed_arm speedTracker.cpp `pkg-config --cflags --libs opencv.pc`
#arm-xilinx-linux-gnueabi-g++ -o speed speedTracker.cpp \
#			     -Wl,-rpath-link, usr/lib/lib*.so.* \
#                             -L arm_build/lib \
#                             -I arm_build/include/opencv \
#                             -I arm_build/include \
#                             -I usr/drivers/sdi \
#                             -I usr/drivers/car_hardware \
#                             -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video
