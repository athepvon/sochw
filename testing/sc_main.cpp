#include "systemc.h"
#include "input.h"
#include "filter.h"
#include "highgui.h"
#include "opencv2/highgui/highgui.hpp"
#include <sstream>
#include <iostream>
#include "stdint.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int sc_main(int argc, char* argv[]){

	VideoCapture cap(0);
	int counter = 0;    
	
	vector<int> compression_params; //vector that stores the compression parameters of the image
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	compression_params.push_back(98); //specify the compression quality
	
	if(!cap.isOpened()){
		cout << "CANNOT OPEN THE VIDEO CAMERA" << endl;
		return -1;
	}
	
	double cam_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double cam_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	
	cout << "Frame size : " << cam_width << " x " << cam_height << endl;
	
	namedWindow("Camera",CV_WINDOW_AUTOSIZE);
	
	Mat camera(cam_height,cam_width,CV_8UC3);
	
	while (1) {
		
		bool cap_success = cap.read(camera);
		
		if(!cap_success){
			cout << "Did not capture an image with the camera" << endl;
			break;
		}
		
		if(counter < 1){
			cout << "Press any key to take a picture." << endl;
			counter++;
		}
		
		imshow("Camera", camera);
		
		if(waitKey(30)==13){
			cap.release();
			break;
		}
	} 
	
	int cols = camera.cols;
	int rows = camera.rows;
	
	for (int i = 0; i < rows; i++)
    {
    	Vec3b *ptr = camera.ptr<Vec3b>(i);

      	for (int j = 0; j < cols; j++)
        {   
      		Vec3b pixel = ptr[j];
      		blue_out[i][j] = pixel.val[0];
      		green_out[i][j] = pixel.val[1];
      		red_out[i][j] = pixel.val[2];
        }
    }
	
	imwrite("before.jpg", camera, compression_params);
	
	sc_clock c1;
	sc_signal<bool> start,newimage,ready,sending;
	sc_signal<sc_uint<24> > RGB;
	sc_signal<sc_int<16> > col_size, row_size;
	
	col_size = cols;
	row_size = rows;
		

	//-----initializes input module ports to signals------
	input input("input");
	input.clock(c1);
	input.newimage(newimage);
	input.sending(sending);
	input.ready(ready);
	input.RGB_24_bit(RGB);
	input.max_cols(col_size);
	input.max_rows(row_size);
	input.start(start);
	//-------------------------------------------
	
	//-----initializes filter module ports to signals------
	filter filter("FILTER");
	filter.clock(c1);
	filter.newimage(newimage);
	filter.ready(ready);
	filter.RGB_24_bit(RGB);
	filter.max_cols(col_size);
	filter.max_rows(row_size);
	filter.sending(sending);
	//-------------------------------------------------
	

	
	
	start = true;
	sc_start();

	return(0);
}