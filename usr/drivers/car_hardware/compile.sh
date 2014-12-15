#!/bin/sh
export KERNEL_DIR=/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9
export PATH=/home/ulrichyonga/Documents/Dissertation/Embedded_Linux/buildroot/linux-te-3.9:$PATH
# make pl080 DMA testing module 
make clean
make KERNELDIR=$KERNEL_DIR > makeresults.txt 2>&1

