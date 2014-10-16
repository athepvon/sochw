#include "systemc.h"

SC_MODULE(input) {
	
	//I/O ports
	sc_in_clk clock;
	sc_in<bool> ready_tosend,filterisdone;
	sc_out<bool> newimage_tofilter,send_tofilter;
	sc_out<sc_int<16> > rows_out, cols_out;
	sc_out<sc_uint<24> > rgb_out;
	sc_signal<bool> newimage,ready; 
	
	//image capture function in OpenCV
	void imagecapture();
	
	//send image to filter module 24 bit RGB at a time in SystemC
	void send_pic();
	
	//constructor for the input module
	SC_CTOR(input) {
		SC_CTHREAD(imagecapture, clock);
		SC_CTHREAD(send_pic, clock);
	}
};
