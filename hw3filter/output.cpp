#include "systemc.h"
#include "output.h"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;

void output::get_image(){
	int rows = 0, cols = 0;
	int max_rows,max_cols;
	
	while(true){
		//waits for a new image from the filter module
		do wait();while(!newimage_ffil);
		
		//sets the rows and columns
		max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
		
		while(true){
			//tells the filter module that its ready to receive the transfer
			ready_tofil = true;
			
			//waits for the filter module to send the transfer
			do wait();while(!send_ffil);
			ready_tofil = false;
			wait();
			
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
				rows = 0;
				newimage = true;
				wait();
				//waits for the show_function to be ready for another image
				do wait();while(!readytoshow);
				newimage = false;
				wait();
				//tells the filter module that it is ready for another image
				issave = true;
				wait();
				break;
			}
		}
	}
}

void output::show_image(){
	int max_rows,max_cols;
	int counter;
	
	while(true){
		//waits or a new image from the get_image function
	    do wait();while(!newimage);
	   	max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
		
		//creates matrix type array of pixels and combine them into and image
	    Mat image(max_rows,max_cols,CV_8UC3);
	    
		//combine all the arrays to create an image
    	for(int i=0; i<max_rows; i++){
   			for(int j=0; j<max_cols; j++){
   				image.at<Vec3b>(i,j) = Vec3b(blue_out[i][j],green_out[i][j],red_out[i][j]);
    		}
		}
		//shows the image to a window
		imshow("image",image);
		
		readytoshow=true;
		wait();
		
		//hitting the enter button will escape the program
		if(counter < 1)
			cout << "Press the enter button to escape." << endl;
		counter++;
		if(waitKey(30) == 13){
			sc_stop();
			break;
		}
	}
}
