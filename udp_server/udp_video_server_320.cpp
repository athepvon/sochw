#include "highgui.h"
#include "cv.h"
#define CV_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include "UdpServer.h"
#include "Packet.h"
#include <stdexcept>
using namespace std;
using namespace cv;
 
int UDPMAX = 65507; // max buffer size
int port1 = 9999; int port2 = 9999-1; int port3 = 9999-2;

static void print_line(const unsigned char* buffer,  int offset, int num) {
    static char hexval[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    int i;
    unsigned char* hex_cp;
    unsigned char* char_cp;
    unsigned char line[80];
    memset(line, ' ', sizeof(line));
    line[79] = 0;
    hex_cp = &line[0];
    char_cp = &line[16 * 3 + 4];
    for (i=0; i< num; i++) {
        unsigned char input = buffer[offset + i];
        *hex_cp++ = hexval[input >> 4];
        *hex_cp++ = hexval[input & 0x0F];
        hex_cp++;
        *char_cp++ = (isprint(input) ? input : '.');
    }
    printf("%s\n", line);
}

static void print_buffer(const unsigned char* buffer, int buffer_len) {
    int offset = 0;
    while (offset < buffer_len) {
        int remaining = buffer_len - offset;
        print_line(buffer, offset, (remaining >= 16) ? 16 : remaining);
        offset += 16;
    }
}

UdpServer::UdpServer(unsigned int port) {
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw std::logic_error("no socket available");
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        close(fd);
        throw std::logic_error("cannot bind to socket");
    }
    printf("listening to port %d\n", port);
}


UdpServer::~UdpServer() {
    close(fd);   
}
int UdpServer::receiveData(char* msgbuf, int len){

        struct sockaddr_in cliAddr; 
        socklen_t cliLen = sizeof(cliAddr);
        ssize_t recv_bytes = recvfrom(fd, msgbuf, len , 0, reinterpret_cast<struct sockaddr*>(&cliAddr), &cliLen);
        if (recv_bytes < 0) {
            throw std::logic_error("cannot receive");
        }
       //printf("received %d bytes from %s\n", recv_bytes, inet_ntoa(cliAddr.sin_addr));
       // print_buffer((const unsigned char*)msgbuf, recv_bytes);
       return recv_bytes;
}

void UdpServer::run() {
    const size_t MSGBUFSIZE = 1500;
    char msgbuf[MSGBUFSIZE];
    while (1) {
        struct sockaddr_in cliAddr; 
        socklen_t cliLen = sizeof(cliAddr);
        ssize_t recv_bytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, reinterpret_cast<struct sockaddr*>(&cliAddr), &cliLen);
        if (recv_bytes < 0) {
            throw std::logic_error("cannot receive");
        }

        printf("received %lu bytes from %s\n", recv_bytes, inet_ntoa(cliAddr.sin_addr));
        print_buffer((const unsigned char*)msgbuf, recv_bytes);
    }
}


void *receive_cam1_thread(void*){
    cout<<"=== VIDEO RECEIVER 1 ==="<<endl;
 
    //setup UDP
    UdpServer server1(port1);
    char *buff = (char*)malloc(UDPMAX);
 
    //setup openCV
    //cvNamedWindow("UDP Video Receiver 1", CV_WINDOW_AUTOSIZE);
    //vector<uchar> videoBuffer;
    int i=0; int j=0;  int size= 15360;
    while(1){
        //read data
        IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1), *frameO = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1), *frameD = cvCreateImage(cvSize(240,320), IPL_DEPTH_8U, 1);
        char * data=(char*)frame->imageData;
        for(i=0;i<5;i++)
        {       
               int result=-1;
               while(result<size)
               {
			result = server1.receiveData(buff, UDPMAX);
			if(result <= 0){
			    cout<<"Failed to receive frame 1."<<endl;
			    continue;
			}
               }
		cout<<"Got a frame 1 of size "<<result<<endl;
               	for(j=0;j<size;j++) data[(i*size) +j]=buff[j];
        }
        //IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
        //frame->imageData = (char *) (buff);
	cvTranspose(frame, frameD);
        cvFlip(frameD, frameD, 0);
	cvTranspose(frameD, frameO);
        cvFlip(frameO, frameO, 0);
        cvShowImage("UDP Video Receiver 1", frameO);
        cvMoveWindow("UDP Video Receiver 3",50, 100);
        cvWaitKey(1);
    }
}

void *receive_cam2_thread(void*){
    cout<<"=== VIDEO RECEIVER 2 ==="<<endl;
 
    //setup UDP
    UdpServer server2(port2);
    char *buff = (char*)malloc(UDPMAX);
 
    //setup openCV
    //cvNamedWindow("UDP Video Receiver 2", CV_WINDOW_AUTOSIZE);
    //vector<uchar> videoBuffer;
    int i=0; int j=0;  int size= 15360;
    while(1){
        //read data
        IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
        char * data=(char*)frame->imageData;
        for(i=0;i<5;i++)
        {       
               int result=-1;
               while(result<size)
               {
			result = server2.receiveData(buff, UDPMAX);
			if(result <= 0){
			    cout<<"Failed to receive frame 2."<<endl;
			    continue;
			}
               }
		cout<<"Got a frame 2 of size "<<result<<endl;
               	for(j=0;j<size;j++) data[(i*size) +j]=buff[j];
        }
        //IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
        //frame->imageData = (char *) (buff);
        cvShowImage("UDP Video Receiver 2", frame);
        cvMoveWindow("UDP Video Receiver 3",400,100);
        cvWaitKey(10);
    }
}

void *receive_cam3_thread(void*){
    cout<<"=== VIDEO RECEIVER 3==="<<endl;
 
    //setup UDP
    UdpServer server3(port3);
    char *buff = (char*)malloc(UDPMAX);
 
    //setup openCV
    //cvNamedWindow("UDP Video Receiver 3", CV_WINDOW_AUTOSIZE);
    //vector<uchar> videoBuffer;
    int i=0; int j=0;  int size= 15360;
    while(1){
        //read data
        IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
        char * data=(char*)frame->imageData;
        for(i=0;i<5;i++)
        {       
               int result=-1;
               while(result<size)
               {
			result = server3.receiveData(buff, UDPMAX);
			if(result <= 0){
			    cout<<"Failed to receive frame 3."<<endl;
			    continue;
			}
               }
		cout<<"Got a frame 3 of size "<<result<<endl;
               	for(j=0;j<size;j++) data[(i*size) +j]=buff[j];
        }
        //IplImage *frame = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
        //frame->imageData = (char *) (buff);
        cvShowImage("UDP Video Receiver 3", frame);
        cvMoveWindow("UDP Video Receiver 3",400,400);
        cvWaitKey(10);
    }
}
 
int main(){
   XInitThreads();
   pthread_t threads[3];
   int rc;
   long t;
   t=0;
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, receive_cam1_thread, NULL);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   t++;
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, receive_cam2_thread, NULL);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   t++;
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, receive_cam3_thread, NULL);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   /* Last thing that main() should do */
   pthread_exit(NULL);
}


