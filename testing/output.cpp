#include "systemc.h"
#include "output.h"
#include "highgui.h"
#include <sstream>
#include <iostream>
#include "stdint.h"
#include <fstream>
#include <stdio.h>

using namespace cv;
using namespace std;

int blue_[960][1280],green_[960][1280],red_[960][1280];

void saveimage(int rows, int cols){

	vector<int> compression_params; //vector that stores the compression parameters of the image
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	compression_params.push_back(98); //specify the compression quality

    Mat image(rows, cols, CV_8UC3 );
    for(int i=0; i<rows; i++){
   		for(int j=0; j<cols; j++){
   			image.at<Vec3b>(i,j) = Vec3b(blue_[i][j],green_[i][j],red_[i][j]);
    	}
	}
	imwrite("after.jpg", image, compression_params);
}

void output::save_pic(){
	int col_size;
	int row_size;
	int rows = 0, cols = 0;
	
	while(true) {

		//cout << "come in" << endl;
		do wait(); while(!newimage_from_filter);
		
		col_size = cols_in.read().to_int();
		row_size = rows_in.read().to_int();
		//cout << "set " << max_cols << " x " << max_rows << endl;
		//cout << "got the newimage" << endl;
		ready_to_filter = true;
		do wait();while(!send_from_filter);
		
		ready_to_filter = false;
		wait();

		byte_to_pix = RGB_24_bit_in.read();
		wait();

		blue_[rows][cols] = byte_to_pix.range(7,0);
		green_[rows][cols] = byte_to_pix.range(15,8);
		red_[rows][cols] = byte_to_pix.range(23,16);

		ready_to_filter = true;
		wait();
		
		cols++;	
		if (cols >= col_size ) {
			cols = 0;
			rows++;
		}

		if(rows >= row_size){
			cout << "rows : " << rows << endl;
			cout << "cols : " << cols << endl;
			rows = 0;
			cout << "inside output" << endl;
			break;
		}
	}
	saveimage(row_size,col_size);
	
	sc_stop();
}


