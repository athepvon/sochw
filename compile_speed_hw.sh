export PATH=$PATH:/opt/pkg/petalinux-v2014.2-final/tools/linux-i386/arm-xilinx-linux-gnueabi/bin
#export CROSS_COMPILE=arm-xilinx-linux-gnueabi-

arm-xilinx-linux-gnueabi-g++ -o speed speedTracker.cpp \
			     -Wl,-rpath-link, usr/lib/lib*.so.* \
                             -L usr/opencv-2.4.2/lib \
                             -I usr/opencv-2.4.2/include/opencv \
                             -I usr/opencv-2.4.2/include \
                             -I usr/drivers/sdi \
                             -I usr/drivers/car_hardware \
                             -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video
