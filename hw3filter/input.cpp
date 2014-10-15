#include "systemc.h"
#include "input.h"
#include "highgui.h"
#include <sstream>
#include "stdint.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int blue_in[480][640],green_in[480][640],red_in[480][640];
int max_rows,max_cols;

void input::imagecapture(){
	VideoCapture cap(0);

	max_cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	max_rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	Mat camera(max_rows,max_cols,CV_8UC3);

	while(true){	
		
		cap.read(camera);
		//imshow("camera", camera);

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
    	//cout << "image captured" << endl;
		newimage = true;
		wait();
		
		if(waitKey(30) == 13){
			cap.release();
			sc_stop();
			break;
		}
		
		do wait();while(!ready);
		newimage = false;
		wait();
	}
}

void input::send_pic(){
	
	unsigned short lsb,isb,msb;
	uint32_t rgb_24_bit; 
	int rows = 0,cols = 0;
		
	ready = true;
	newimage_tofilter = false;
	while(true){
	
		send_tofilter = false;
		
		do wait();while(!newimage);
		ready = false;
		wait();
		//cout << "ready to send image to filter" << endl;
		rows_out.write(max_rows);
		wait();
		cols_out.write(max_cols);
		wait();
		
		newimage_tofilter = true;
		wait();
		
		while(true){
			do wait();while(!ready_tosend);
		
			lsb = blue_in[rows][cols];
			isb = green_in[rows][cols];
			msb = red_in[rows][cols];
			rgb_24_bit = lsb | isb << 8 | msb << 16;
			rgb_out.write(rgb_24_bit);
			wait();
			//cout << "in input send out" << endl;
			send_tofilter = true;
			wait();
			
			cols++;
			
			if(cols >= max_cols){
				cols = 0;
				rows++;
			}
			if(rows >= max_rows){
				rows = 0;
				newimage_tofilter = false;
				wait();
				do wait();while(!filterisdone);
				ready = true;
				wait();
				break;
			}
		}			
	}
	
}
