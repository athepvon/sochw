#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdexcept>
#include <iostream>
#include "Packet.h"
#define HARDWARE 0
#define ROTATE 1
#if HARDWARE == 1
#include "usr/drivers/sdi/plb_sdi_controller.h"
#include "usr/drivers/car_hardware/razorcar_hardware.h" 
#endif

int deviceDriverFile;
uint8_t *dst_buffer; uint8_t *mapped_base;
uint8_t *dst_buffer1; uint8_t *mapped_base1;
#if HARDWARE == 1
void openDeviceDriver( )
{
	if ((deviceDriverFile = open("/dev/plb_sdi_ctrl", O_RDWR)) != -1)
	{  // must be O_RDWR to enable mmap() with MAP_SHARED

		        dst_buffer = (uint8_t*)mmap(0, BUFFER_SIZE*16, PROT_READ | PROT_WRITE, MAP_SHARED, deviceDriverFile, 0); // must be MAP_SHARED                      
			if(dst_buffer == MAP_FAILED) {
				perror("mmap() failed");
				exit(1);
			}
                        else{
                                unsigned long tmp = 0;unsigned long tmp1 = 0;
				ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCSHWREGNUM, &tmp);
				ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCGHWREG, &tmp);
				printf( "HW Version:Magic: %08X\n", tmp );
				printf( "cap dist buffer: %08X\n", dst_buffer );
                                tmp = 9; tmp1=0;
                        }
	}
	else{
             perror("plb_sdi_controller_apps open");
             exit(1);
        }
}
#endif

int main(int argc, char *argv[])
{
	#if HARDWARE == 1
		openDeviceDriver();
	#else
		//this is where the image/video is captured
	#endif
	
	static IplImage *framein = NULL, *imgHSV = NULL,  *h = NULL,  *s = NULL,*framer = NULL, *framer1 = NULL;
	
	#if HARDWARE == 1
		static int tmp1 = 0;static int tmp2 = 0;static int tmp = 0;
		uint8_t *dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
		uint8_t *dstb = (uint8_t *) dst_buffer;
 	#endif
 	
 	IplImage *framed = cvCreateImage(cvSize(240,320), IPL_DEPTH_8U, 1);
 	framein = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
 	IplImage *framein1 = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
 	
	while(true)
	{
		#if HARDWARE == 1
			ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1);
			framein->imageData = (char *) (dst_buffer);  
            cvConvertImage(framein, framein1, 0);
            frame=framed;
            cvResize(framein1 , frame, CV_INTER_LINEAR);
        #else
        	//input the frames camera>>frame
    	#endif
    	
    	#if RAZORCAR == 1
			tmp1 = 0;tmp = 0;
		    ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1); 
	        framein->imageData = (char *) (dst_buffer);  
			cvConvertImage(framein, framein1, 0);
			frame=framed;
			cvResize(framein1, frame, CV_INTER_LINEAR);
		#else
        	//input the frames camera>>frame
    	#endif
    	
		#if RAZORCAR == 1
			dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
			memcpy(dst,framer1->imageData,BUFFER_SIZE);
		    tmp=0;
			ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);		
			/*for(udpi=0;udpi<5;udpi++)
			{for(udpj=0;udpj<udpsize;udpj++) 
				buf[udpj]=datax1[(udpi*udpsize) +udpj];
				risult = client.sendData(buf, udpsize);
				if(risult < 0)
				    cout<<"Failed to send data"<<endl;
			}*/
		#else
		
		    cvWaitKey(10);
		#endif
	}
}
