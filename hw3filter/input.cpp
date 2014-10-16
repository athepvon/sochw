#include "systemc.h"
#include "input.h"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;

//global array to store the pixels
int blue_in[480][640],green_in[480][640],red_in[480][640];
int max_rows,max_cols;

void input::imagecapture(){
	//create video capture variable
	VideoCapture cap(0);
	
	//get the size of the camera frame
	max_cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	max_rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	Mat camera(max_rows,max_cols,CV_8UC3);

	while(true){	
		//open camera and read and image
		cap.read(camera);

		//Separate the RGB channels to separate colours to send pixels at a time
		for (int i = 0; i < max_rows; i++)
    	{
    		Vec3b *ptr = camera.ptr<Vec3b>(i);

      		for (int j = 0; j < max_cols; j++)
        	{   
      			Vec3b pixel = ptr[j];
      			blue_in[i][j] = pixel.val[0];
      			green_in[i][j] = pixel.val[1];
      			red_in[i][j] = pixel.val[2];
        	}
    	}
		//tells the send_pic function there is a new image to be transferred
		newimage = true;
		wait();
		
		//hitting the enter key will exit the program
		if(waitKey(30) == 13){
			cap.release();
			sc_stop();
			break;
		}
		
		//wait for a response from the send_pic function to continue and capture another image
		do wait();while(!ready);
		newimage = false;
		wait();
	}
}

void input::send_pic(){
	
	unsigned short lsb,isb,msb;
	uint32_t rgb_24_bit; 
	int rows = 0,cols = 0;
		
	while(true){
		//wait for a new image from the image capture function
		do wait();while(!newimage);
		ready = false;
		wait();
		
		//sends the row size to the filter function
		rows_out.write(max_rows);
		wait();
		
		//sends the column size to the filter function
		cols_out.write(max_cols);
		wait();
		
		//tells the filter that there is a new image to be received
		newimage_tofilter = true;
		wait();
		
		while(true){
			//wait for the filter module to get ready
			do wait();while(!ready_tosend);
			
			//grab 1 pixel from the blue channel which is 8 bits and initializes it as the least significant bit
			lsb = blue_in[rows][cols];
			
			//grab 1 pixel from the green channel which is 8 bits and initializes it as the inner significant bit
			isb = green_in[rows][cols];
			
			//grab 1 pixel from the red channel which is 8 bits and initializes it as the most significant bit
			msb = red_in[rows][cols];
			
			//converts all the bits into a 24 bit hex number
			rgb_24_bit = lsb | isb << 8 | msb << 16;
			
			//transfers the 24 bit to the filter module
			rgb_out.write(rgb_24_bit);
			wait();
			
			//tells the filter module to get ready to recieve the transfer
			send_tofilter = true;
			wait();
			
			cols++;
			
			if(cols >= max_cols){
				cols = 0;
				rows++;
			}
			//if the row count exceeds the max_row amount then it will start the row over
			if(rows >= max_rows){
				rows = 0;
				//tells the filter module that there is not another image to be sent yet
				newimage_tofilter = false;
				wait();
				//waits for a response from the filter module to capture another image
				do wait();while(!filterisdone);
				ready = true;
				wait();
				break;
			}
		}			
	}
}
