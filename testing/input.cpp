#include "systemc.h"
#include "input.h"
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

//global variables
int blue_out[960][1280],green_out[960][1280],red_out[960][1280];
int set_cols, set_rows;

bool cameracapture(){
	
	VideoCapture cap(0);
	int counter = 0;    
	bool imagecaptured;
	
	vector<int> compression_params; //vector that stores the compression parameters of the image
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	compression_params.push_back(98); //specify the compression quality
	
	if(!cap.isOpened()){
		cout << "CANNOT OPEN THE VIDEO CAMERA" << endl;
		return imagecaptured;
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
			return imagecaptured;
		}
		
		if(counter < 1){
			cout << "Press the enter key to take a picture." << endl;
			counter++;
		}
		
		imshow("Camera", camera);
		
		if(waitKey(30)==13){
			cap.release();
			imagecaptured = true;
			break;
		}
	} 
	
	set_cols = camera.cols;
	set_rows = camera.rows;
	
	for (int i = 0; i < set_rows; i++)
    {
    	Vec3b *ptr = camera.ptr<Vec3b>(i);

      	for (int j = 0; j < set_cols; j++)
        {   
      		Vec3b pixel = ptr[j];
      		blue_out[i][j] = pixel.val[0];
      		green_out[i][j] = pixel.val[1];
      		red_out[i][j] = pixel.val[2];
        }
    }
    
    /*ofstream myfile;
	myfile.open("text.txt");
	for(int i = 0; i < set_rows;i++){
		for(int j = 0 ; j < set_cols;j++){
			blue_out[i][j] = rand()%256;
			green_out[i][j] = rand()%256;
			red_out[i][j] = rand()%256;
			myfile << blue_out[i][j] << " ";
			myfile << green_out[i][j] << " ";
			myfile << red_out[i][j] << " ";
		}
		myfile << "\n";
	}
	myfile.close();*/
	
	imwrite("before.jpg", camera, compression_params);
	return imagecaptured;
}

void input::send_pixel(){

	bool imagecaptured = cameracapture();
	wait();
	
	unsigned short lsb,isb,msb; //assign least sig bit, inner sig bit, most sig bit variables
	uint32_t rgb_24_bit; //assign 24 bit rgb signal variables
	int col_size,row_size;	
	
	if(imagecaptured){
		//set size of rols and cols 
		col_size = set_cols;
		row_size = set_rows;
		//set rows and cols to the output for the filter module
		max_cols.write(set_cols);
		max_rows.write(set_rows);
		wait();
	
		cout << "set " << max_cols << " x " << max_rows << endl;
		int rows = 0,cols = 0; // rows and columns counter
		wait();
	
		while(true){
	
			sending = false;
	
			do wait();while(!start);
		
			newimage = true;
		
			do wait();while(!ready);
			//cout << "got the ready" << endl;
	
			//cout << "cols " << cols << endl;
			//cout << "rows " << rows << endl;		
		
			lsb = blue_out[rows][cols];
			wait();
				
			isb = green_out[rows][cols];
			wait();
			
			msb = red_out[rows][cols];
			wait();
			
			cols++;
			
			rgb_24_bit = lsb | isb << 8 | msb << 16;
			wait();
		
			RGB_24_bit.write(rgb_24_bit);
			wait();
			
			//cout << "24 bit write" << endl;
			sending = true;
			wait();
			
			if(cols >= col_size){
				cols = 0;
				rows++;
			}
			wait();
		
			if(rows >= row_size){
				rows = 0;
				break;
				//cout << " finished if on top side: " << endl;
			}		
			wait();	
		}
	}
}
