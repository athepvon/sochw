#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "cv.h"
using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
	
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
	
	int rows = camera.rows;
    int cols = camera.cols;

	int blue[rows][cols],green[rows][cols],red[rows][cols];
   	for (int i = 0; i < rows; i++)
    {
      Vec3b *ptr = camera.ptr<Vec3b>(i);

      for (int j = 0; j < cols; j++)
        {   
      		Vec3b pixel = ptr[j];
      		blue[i][j] = pixel.val[0];
      		green[i][j] = pixel.val[1];
      		red[i][j] = pixel.val[2];
        }
    }
    	
    imwrite("before.jpg", camera, compression_params);
    
    //ouput the image after combining the BGR pixels
    Mat image(rows, cols, CV_8UC3 );
    for(int i=0; i<rows; i++){
   		for(int j=0; j<cols; j++){
   			image.at<Vec3b>(i,j) = Vec3b(blue[i][j],green[i][j],red[i][j]);
    	}
	}
	imwrite("after.jpg", image, compression_params);
	return 0;
}
