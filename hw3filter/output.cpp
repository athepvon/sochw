#include "systemc.h"
#include "output.h"
#include "highgui.h"
#include <sstream>
#include "stdint.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void output::get_image(){
	int rows = 0, cols = 0;
	int max_rows,max_cols;
	
	
	while(true){
	
		do wait();while(!newimage_ffil);
		//cout << " new image in output" << endl;
		max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
		//cout << "row and cols" << max_rows << " "<< max_cols << endl;
		while(true){
			ready_tofil = true;
			do wait();while(!send_ffil);
			ready_tofil = false;
			wait();
			//cout << "new image being recieved in output" << endl;
			byte_to_pix = rgb_in.read();
			wait();
			
			blue_out[rows][cols] = byte_to_pix.range(7,0);
			green_out[rows][cols] = byte_to_pix.range(15,8);
			red_out[rows][cols] = byte_to_pix.range(23,16);
			
			cols++;
			
			if (cols >= max_cols){
				cols = 0;
				rows++;
			}
			
			if(rows >= max_rows){
				//cout << "rows and working " << rows << endl;
				rows = 0;
				newimage = true;
				wait();
				do wait();while(!readytoshow);
				//cout<< "setting new image to false "<<endl;
				newimage = false;
				wait();
				issave = true;
				wait();
				break;
			}
		}
	}
}

void output::show_image(){
	int max_rows,max_cols;
	
	while(true){
	    do wait();while(!newimage);
	   
	    wait();
	   	max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
	   
	    wait();
	    Mat image(max_rows,max_cols,CV_8UC3);
	    
    	for(int i=0; i<max_rows; i++){
   			for(int j=0; j<max_cols; j++){
   				image.at<Vec3b>(i,j) = Vec3b(blue_out[i][j],green_out[i][j],red_out[i][j]);
    		}
		}
		//cout << "showing the image" <<endl;
		imshow("image",image);
		
		readytoshow=true;
		wait();
		
		if(waitKey(30) == 13){
			sc_stop();
			break;
		}
	}
}	
	
