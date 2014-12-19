#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <cv.h>
#include <cv.hpp>
#include <highgui.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>
#include "UdpClient.h"
#include "Packet.h"
#define HARDWARE 0
#define ROTATE 1
#if HARDWARE == 1
#include "usr/drivers/sdi/plb_sdi_controller.h"
#endif
using namespace std;
using namespace cv;
const char *addr = "192.168.1.12";
static const double pi = 3.14159265358979323846;
int port = 9999;
int reversDigits(int num)
{
  static int rev_num = 0;
  static int base_pos = 1;
  if(num > 0)
  {
    reversDigits(num/10);
    rev_num  += (num%10)*base_pos;
    base_pos *= 10;
  }
  return rev_num;
}

UdpClient::UdpClient(const string& ipaddr, unsigned int port) : id(0) {
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout<<"no socket available"<< endl;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(0);
    
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        close(fd);
        cout<<"cannot bind to socket"<< endl;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipaddr.c_str());
    serverAddr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
       close(fd);
       cout<<"cannot connect to socket"<< endl;
    }
}


UdpClient::~UdpClient() {
    close(fd);
}

int UdpClient::sendData(char* buffer, int len) {
        return (::send(fd, buffer, len, 0) );
}


void UdpClient::send() {
    const size_t size = 100;
    char buffer[size];
    for (unsigned int i=0; i<size; ++i) {
        fillPacket(buffer, size);
        if (::send(fd, buffer, size, 0) < 0) {
            cerr << "error sending packet " << i << endl;
        }
    }
}


void UdpClient::fillPacket(char* data, size_t size) {
    assert(size >= sizeof(Packet));
    memset(data, 0, size);
    Packet* header = (Packet*)data;
    ++id;
    header->id = id;
}

typedef unsigned long long timestamp_t;
static timestamp_t get_timestamp ()
{
      struct timeval now;
      gettimeofday (&now, NULL);
      return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}
timestamp_t t1,t0; double secs;
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
double meter;
//int to string helper function
string intToString(int number){

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

int searchForMovement(Mat thresholdImage, Mat &cameraFeed){
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
			::meter = distance / Pix2meters;
			cout << "Pixel distance in one second: " << distance << endl;
			cout << "Meters per second: " << ::meter << endl;
		}
		return 1;
	}
	else
		return 0;
}

int deviceDriverFile;
uint8_t *dst_buffer; 

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
	UdpClient client(addr, port);
	int key_pressed = 0;
	bool objectDetected = false;
	bool debugMode = true;
	bool trackingEnabled = true;
	bool pause = false;
	//Mat frame1,frame2;
	Mat grayImage1,grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	//video capture object.
	
	#if HARDWARE == 1
		openDeviceDriver();
		static int tmp1 = 0;static int tmp = 0;
		uint8_t *dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
		uint8_t *dstb = (uint8_t *) dst_buffer;
	#else
		//this is where the image/video is captured
		//VideoCapture capture;
		CvCapture *input_video;
	#endif
		int risult;
		int udpsize= 15360; int udpi=0; int udpj=0;
		char buf[udpsize]; 		
		
	//#if HARDWARE ==1

		static IplImage *framer1 = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
    	IplImage *framein = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
		IplImage *framein1 = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
		IplImage *frame1Ipl = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);;
 		IplImage *framed = cvCreateImage(cvSize(240,320), IPL_DEPTH_8U, 1);
 		IplImage *fram = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
		IplImage *destin = NULL;
    	//framein = new IplImage(frame1);
   // #endif
    input_video = cvCaptureFromFile("bouncingBall.avi");
	while(key_pressed != 27)
	{
	

		#if HARDWARE == 1
			ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1);
			framein->imageData = (char *) (dst_buffer);  
            cvConvertImage(framein, framein1, 0);
            frame1=framed;
            cvResize(framein1 , frame1Ipl, CV_INTER_LINEAR);
            frame1 = frame1Ipl;
        #else
        	//input the frames camera>>frame
        	//capture >> frame1;
        	destin = cvQueryFrame(input_video);
        	Mat frame1(destin);
    	#endif
    	
    	#if HARDWARE == 1
			tmp1 = 0;tmp = 0;
		    ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1); 
	        framein->imageData = (char *) (dst_buffer);  
			cvConvertImage(framein, framein1, 0);
			frame2=framed;
			cvResize(framein1, frame1Ipl, CV_INTER_LINEAR);
			frame2 = frame1Ipl;
		#else
        	//input the frames camera>>frame
        	//capture >> frame2;
        	destin = cvQueryFrame(input_video);
        	Mat frame2(destin);
    	#endif
    	
    	FrameCount++;
    	cv::cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
		cv::cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
		cv::absdiff(grayImage1,grayImage2,differenceImage);

		cv::threshold(differenceImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		
		cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
			//threshold again to obtain binary image from blur output
		cv::threshold(thresholdImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
		
		int newSpeed = searchForMovement(thresholdImage,frame1);
		std::string output;
		int baseLine = 0;
		
		if(newSpeed){
			output = "Meters per second: " + std::to_string(::meter);
		}
		else{
			output = "Meters per second: " + 0;
		}
		Size textSize = getTextSize(output, 1, 1, 1, &baseLine);
		Point textOrigin(frame1.cols - 2*textSize.width -10, frame1.rows - 2*baseLine -10);
		putText(grayImage1, output, textOrigin,FONT_HERSHEY_TRIPLEX, 1, 8);
		putText(frame1, output, textOrigin,FONT_HERSHEY_TRIPLEX, 1, 8);
		frame1Ipl = new IplImage(grayImage1);
   	    //fram = frame1Ipl;
   	    
   	    cvResize(frame1Ipl, fram);
   	    
   	    uchar* datax1 = (uchar *)fram->imageData;

		#if HARDWARE == 1
			dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
			framer1 = new IplImage(frame1);
			memcpy(dst,framer1->imageData,BUFFER_SIZE);
		    tmp=0;
			ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);		
			for(udpi=0;udpi<5;udpi++)
			{
				for(udpj=0;udpj<udpsize;udpj++) 
					buf[udpj]=datax1[(udpi*udpsize) +udpj];
				risult = client.sendData(buf, udpsize);
				if(risult < 0)
					cout<<"Failed to send data"<<endl;
			}
		#else
			for(udpi=0;udpi<5;udpi++)
			{
				for(udpj=0;udpj<udpsize;udpj++) 
					buf[udpj]=datax1[(udpi*udpsize) +udpj];
				risult = client.sendData(buf, udpsize);
				if(risult < 0)
					cout<<"Failed to send data"<<endl;
			}
			/*std::string output = "Meters per second: " + std::to_string(::meter);
			int baseLine = 0;
			Size textSize = getTextSize(output, 1, 1, 1, &baseLine);
			Point textOrigin(frame1.cols - 2*textSize.width -10, frame1.rows - 2*baseLine -10);
			putText(frame1, output, textOrigin,FONT_HERSHEY_TRIPLEX, 1, 8);*/
		#endif
		
		imshow("Frame1",frame1);
		key_pressed = cvWaitKey(5);
		
	}
	

	
}
