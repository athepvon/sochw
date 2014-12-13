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
#endif
using namespace std;
using namespace cv;
typedef unsigned long long timestamp_t;
static timestamp_t get_timestamp ()
{
      struct timeval now;
      gettimeofday (&now, NULL);
      return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}
timestamp_t t1,t0; double secs;
timestamp_t tx1,tx0; double secsx;
//our sensitivity value to be used in the threshold() function
const static int SENSITIVITY_VALUE = 20;
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
const static int BLUR_SIZE = 10;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = {0,0};
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0,0,0,0);
/* count the frames and keep track of the fps of the source video */
int FrameCount = 0;
int sourceFPS ;
/* create a vector to store the beginning and ending points for the speed calc. */
Point middlePoints[2] = {Point(0,0), Point(0,0)};
/*pixels to meters conversion metric: 177pixels/2ft = 177pixels/2*0.3048meters
 = 290.354pixels/meter.  meters/pixel 1/290.354 */
float Pix2meters = 290.354;
//int to string helper function
string intToString(int number){

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	//notice how we use the '&' operator for the cameraFeed. This is because we wish
	//to take the values passed into the function and manipulate them, rather than just working with a copy.
	//eg. we draw to the cameraFeed in this function which is then displayed in the main() function.
	bool objectDetected=false;
	Mat temp;
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

	//if contours vector is not empty, we have found some objects
	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		//the largest contour is found at the end of the contours vector
		//we will simply assume that the biggest contour is the object we are looking for.
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos , theObject[1] = ypos;
		if (FrameCount > sourceFPS) {
			FrameCount = 0;
			middlePoints[0] = middlePoints[1];
			middlePoints[1] = Point(xpos, ypos);
			double distance = cv::norm(middlePoints[1] - middlePoints[0]);
			cout << "Pixel distance in one second: " << distance << endl;
			cout << "Meters per second: " << distance / Pix2meters << endl;
		}
	}
}

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
	bool objectDetected = false;
	bool debugMode = true;
	bool trackingEnabled = true;
	bool pause = false;
	Mat frame1,frame2;
	Mat grayImage1,grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	//video capture object.
	
	#if HARDWARE == 1
		openDeviceDriver();
	#else
		//this is where the image/video is captured
		VideoCapture capture(0);
	#endif

 
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
        	capture >> frame1;
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
        	capture >> frame2;
    	#endif
    	FrameCount++;
		cv::cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
		cv::absdiff(grayImage1,grayImage2,differenceImage);
		
		cv::threshold(differenceImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		
		cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
			//threshold again to obtain binary image from blur output
		cv::threshold(thresholdImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		
		searchForMovement(thresholdImage,frame1);
		
		t1 = get_timestamp();
   	    secs = (t1 - t0) / 1000000.0L;
		#if RAZORCAR == 1
			dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
			memcpy(dst,framer1->imageData,BUFFER_SIZE);
		    tmp=0;
			ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);		
			for(udpi=0;udpi<5;udpi++)
			{for(udpj=0;udpj<udpsize;udpj++) 
				buf[udpj]=datax1[(udpi*udpsize) +udpj];
				risult = client.sendData(buf, udpsize);
				if(risult < 0)
				    cout<<"Failed to send data"<<endl;
			}
		#else
			imshow("Frame1",frame1);
		    cvWaitKey(10);
		#endif
	}
	
	//make sure to release IplImages before ending program
	//cvReleaseImage( & framed);
	//cvReleaseImage( & framein);
	//cvReleaseImage( & framein1);
	//cvReleaseImage( & framer1);
	//cvReleaseImage( & imgHSV); //currently not being used
	//cvReleaseImage( & h); //currently not being used
	//cvReleaseImage( & s); //currently not being used
	//cvReleaseImage( & framer); //currently not being used
	
}
