#include "systemc.h"
#include "input.h"
#include "filter.h"
#include "output.h"
#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace std;
int motionTracking_main();
int sc_main(int argc, char* argv[]){
	//signal variables
	sc_clock c1;
	sc_signal<bool> ready_if, image_if, send_if, newimage_fo, median_in,sobel_in;
	sc_signal<bool> send_fo, ready_fo, laplace_in, issave, filterisdone, gauss_in, nofilter_in;
	sc_signal<sc_int<16> >rows_if,cols_if,rows_fo,cols_fo;
	sc_signal<sc_uint<24> >rgb_infil,rgb_filout;
	sc_vector < sc_signal<int> > frame1_input2filter[480][640];
	sc_vector < sc_signal<int> > frame2_input2filter[480][640];
	sc_vector < sc_signal<int> > frame3_filter2output[480][640];
		
	int choose_filter;
	
	//input module signal binding
	input input("input");
	input.clock(c1);
	input.ready_tosend(ready_if);
	input.newimage_tofilter(image_if);
	input.send_tofilter(send_if);

	input.gray_out1(frame1_input2filter);
	input.gray_out2(frame2_input2filter);
	
	input.rows_out(rows_if);
	input.cols_out(cols_if);
	input.filterisdone(filterisdone);
	
	//filter module signal binding
	filter filter("filter");
	filter.clock(c1);
	filter.newimage_finput(image_if);
	filter.send_finput(send_if);
	filter.ready_tosend(ready_fo);
	filter.ready_toinput(ready_if);
	filter.newimage_toout(newimage_fo);
	filter.send_toout(send_fo);
	filter.rows_out(rows_fo);
	filter.cols_out(cols_fo);
	filter.cols_in(cols_if);
	filter.rows_in(rows_if);

	//filter.rgb_in(rgb_infil);
	filter.gray_in1(frame1_input2filter);
	filter.gray_in2(frame2_input2filter);

	
	filter.gray_out(frame3_filter2output);

	filter.laplace_in(laplace_in);
	filter.gauss_in(gauss_in);
	filter.median_in(median_in);
	filter.sobel_in(sobel_in);
	filter.nofilter_in(nofilter_in);
	filter.filterisdone(filterisdone);
	filter.issave(issave);
	
	//output module signal binding
	output output("output");
	output.clock(c1);
	output.newimage_ffil(newimage_fo);
	output.send_ffil(send_fo);
	output.ready_tofil(ready_fo);
	output.rows_in(rows_fo);
	output.cols_in(cols_fo);
	
	//output.rgb_in(rgb_filout);
	output.result_frame(frame3_filter2output);
	output.issave(issave);

	
	/*
	//input command to determine which filter to use
	cout << "Please choose a filter." << endl;
	cout << "1: No Filter 2: Laplace Filter | 3: Gaussian Filter | 4: Median Filter | 5: Sobel Filter" << endl;
	cin >> choose_filter;
	
	switch(choose_filter){
		case 1: nofilter_in = true; break;
		case 2: laplace_in = true; break;
		case 3: gauss_in = true; break;
		case 4: median_in = true; break;
		case 5: sobel_in = true; break;
	}
	cout << "loading........." << endl;
	*/
	sc_start();
	motionTracking_main();
	return 0;
}

//motionTracking.cpp

//Written by  Kyle Hounslow, January 2014

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

//danny is the coolest
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

//our sensitivity value to be used in the threshold() function
const static int SENSITIVITY_VALUE = 20;
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
const static int BLUR_SIZE = 10;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = {0,0};
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0,0,0,0);
/* count the frames and keep track of the fps of the source video */
int FrameCount = 0;
int sourceFPS = 30; 
/* create a vector to store the beginning and ending points for the speed calc. */
Point middlePoints[2] = {Point(0,0), Point(0,0)};
/*pixels to meters conversion metric: 177pixels/2ft = 177pixels/2*0.3048meters
 = 290.354pixels/meter.  meters/pixel 1/290.354 */
float Pix2meters = 290.354;
double quartmeter;
//int to string helper function
string intToString(int number){

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	//notice how we use the '&' operator for the cameraFeed. This is because we wish
	//to take the values passed into the function and manipulate them, rather than just working with a copy.
	//eg. we draw to the cameraFeed in this function which is then displayed in the main() function.
	bool objectDetected=false;
	Mat temp;
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

	//if contours vector is not empty, we have found some objects
	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		//the largest contour is found at the end of the contours vector
		//we will simply assume that the biggest contour is the object we are looking for.
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos , theObject[1] = ypos;
		if (FrameCount > sourceFPS/4) {//Do the calculation every quarter second.
			FrameCount = 0;
			middlePoints[0] = middlePoints[1];
			middlePoints[1] = Point(xpos, ypos);
			double distance = cv::norm(middlePoints[1] - middlePoints[0]);
			::quartmeter = (distance / Pix2meters)/4;
			cout << "Pixel distance in quarter second: " << distance/4 << endl;
			cout << "Meters per quarter second: " << ::quartmeter << endl;
		}
	}
}
int motionTracking_main(){

	//some boolean variables for added functionality
	bool objectDetected = false;
	//these two can be toggled by pressing 'd' or 't'
	bool debugMode = true;
	bool trackingEnabled = true;
	//pause and resume code
	bool pause = false;
	//set up the matrices that we will need
	//the two frames we will be comparing
	Mat frame1,frame2;
	//their grayscale images (needed for absdiff() function)
	Mat grayImage1,grayImage2;
	//resulting difference image
	Mat differenceImage;
	//thresholded difference image (for use in findContours() function)
	Mat thresholdImage;
	//video capture object.
	VideoCapture capture(0);
	//CvCapture *capture=cvCaptureFromCAM(0); 
	while(1){

		//we can loop the video by re-opening the capture every time the video reaches its last frame

		//capture.open("bouncingBall.avi");

		if(!capture.isOpened()){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}
		/*calculate time by fps of video */
		//sourceFPS = capture.get(CV_CAP_PROP_FPS);
		//sourceFPS = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
	//	sourceFPS = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS);
		//check if the video has reach its last frame.
		//we add '-1' because we are reading two frames from the video at a time.
		//if this is not included, we get a memory error!
		/*while(capture.get(CV_CAP_PROP_POS_FRAMES)<capture.get(CV_CAP_PROP_FRAME_COUNT)-1){*/

			//read first frame
			/*capture.read(frame1);*/
			capture >> frame1;
			FrameCount++;
			//convert frame1 to gray scale for frame differencing
			cv::cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
			//copy second frame
			capture.read(frame2);
			FrameCount++;
			//convert frame2 to gray scale for frame differencing
			cv::cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
			//perform frame differencing with the sequential images. This will output an "intensity image"
			//do not conf`use this with a threshold image, we will need to perform thresholding afterwards.
			/*Insert call to systemC hardware */
			//wait until the hardware is ready
			do wait();while(!filterisdone);
			while(!hwisready)
			{
			}
			filter::absdiff(grayImage1,grayImage2,differenceImage);
			//cv::absdiff(grayImage1,grayImage2,differenceImage);
			//threshold intensity image at a given sensitivity value
			cv::threshold(differenceImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
			if(debugMode==true){
				//show the difference image and threshold image
				cv::imshow("Difference Image", differenceImage);
				cv::imshow("Threshold Image", thresholdImage);

			}else{
				//if not in debug mode, destroy the windows so we don't see them anymore
				cv::destroyWindow("Difference Image");
				cv::destroyWindow("Threshold Image");
			}
			//use blur() to smooth the image, remove possible noise and
			//increase the size of the object we are trying to track. (Much like dilate and erode)
			cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
			//threshold again to obtain binary image from blur output
			cv::threshold(thresholdImage, thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
			if(debugMode==true){
				//show the threshold image after it's been "blurred"
				cv::imshow("Final Threshold Image", thresholdImage);
			}
			else {
				//if not in debug mode, destroy the windows so we don't see them anymore
				cv::destroyWindow("Final Threshold Image");
			}

			//if tracking enabled, search for contours in our thresholded image
			if (trackingEnabled)
			{
				searchForMovement(thresholdImage,frame1);
			}
			std::string output = "Meters per quarter second: " + std::to_string(::quartmeter);
			int baseLine = 0;
			Size textSize = getTextSize(output, 1, 1, 1, &baseLine);
			Point textOrigin(frame1.cols - 2*textSize.width - 10, frame1.rows - 2*baseLine - 10);	
			putText(frame1, output, textOrigin, CV_FONT_HERSHEY_SIMPLEX, 1, 8);
			
			//show our captured frame
			imshow("Frame1",frame1);
			//check to see if a button has been pressed.
			//this 10ms delay is necessary for proper operation of this program
			//if removed, frames will not have enough time to referesh and a blank
			//image will appear.
			switch(waitKey(40)){

			case 27: //'esc' key has been pressed, exit program.
				return 0;
			case 116: //'t' has been pressed. this will toggle tracking
				trackingEnabled = !trackingEnabled;
				if(trackingEnabled == false) cout<<"Tracking disabled."<<endl;
				else cout<<"Tracking enabled."<<endl;
				break;
			case 100: //'d' has been pressed. this will debug mode
				debugMode = !debugMode;
				if(debugMode == false) cout<<"Debug mode disabled."<<endl;
				else cout<<"Debug mode enabled."<<endl;
				break;
			case 112: //'p' has been pressed. this will pause/resume the code.
				pause = !pause;
				if(pause == true){ cout<<"Code paused, press 'p' again to resume"<<endl;
				while (pause == true){
					//stay in this loop until
					switch (waitKey()){
						//a switch statement inside a switch statement? Mind blown.
					case 112:
						//change pause back to false
						pause = false;
						cout<<"Code resumed."<<endl;
						break;
					}
				}
				}


			}


		/*}*/
		//release the capture before re-opening and looping again.
		/*capture.release();*/

	}

	return 0;

}
