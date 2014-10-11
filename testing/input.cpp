#include "systemc.h"
#include "input.h"
//#include "filter.h"
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

int blue_out[960][1280],green_out[960][1280],red_out[960][1280];
int set_cols, set_rows;

int cameracapture(){
	
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
}

void input::send_pixel(){

	cameracapture();
	
	unsigned short lsb,isb,msb; //assign least sig bit, inner sig bit, most sig bit variables
	uint32_t rgb_24_bit; //assign 24 bit rgb signal variables
	
	//int col_size = max_cols.read().to_int(); //the size of the rows and columns of the array
	//int row_size = max_rows.read().to_int();
	int col_size = set_cols; //the size of the rows and columns of the array
	int row_size = set_rows;

	max_cols.write(set_cols);
	max_rows.write(set_rows);
	wait();
	
	cout << "set " << max_cols << " x " << max_rows << endl;
	int rows = 0,cols = 0; // rows and columns counter
	
	bool notFinish = true; //verify if the job has been completed
	wait();
	
	while(notFinish){
	
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
			notFinish = false;
			//cout << " finished if on top side: " << endl;
		}		
		wait();	
	}
}

/*int sc_main(int argc, char* argv[]){
	
	cameracapture();
	
	sc_clock c1;
	sc_signal<bool> start,newimage,ready,sending;
	sc_signal<sc_uint<24> > RGB;
	sc_signal<sc_int<16> > col_size, row_size;
	
	col_size = set_cols;
	row_size = set_rows;
		
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
	sc_start(5,SC_MS);

	return(0);
}*/
