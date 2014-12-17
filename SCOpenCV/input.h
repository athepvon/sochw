#include "systemc.h"
#include <cv.h>
#include <highgui.h>

SC_MODULE(input) {
	//I/O ports
	sc_in_clk clock;
	sc_in<bool> ready_tosend,filterisdone;
	sc_out<bool> newimage_tofilter,send_tofilter;
	sc_out<sc_int<16> > rows_out, cols_out;
	//sc_out<sc_uint<24> > rgb_out;
	sc_vector < sc_out<int> > gray_out1[480][640];
	sc_vector < sc_out<int> > gray_out2[480][640];

	sc_signal<bool> newimage,ready;
	//int gray_in[480][640];
	cv::Mat grayImage1, grayImage2;
	//int blue_in[240][320],green_in[240][320],red_in[240][320];
	int max_rows,max_cols;
	//image capture function in OpenCV
	void imagecapture();
	
	//send image to filter module 24 bit RGB at a time in SystemC
	void send_pic();
	void MatCaptures(cv::Mat ,cv::Mat);
	//constructor for the input module
	SC_CTOR(input) {
		SC_CTHREAD(imagecapture, clock);
		SC_CTHREAD(send_pic, clock);
	}
};
