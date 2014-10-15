#include "systemc.h"

SC_MODULE(input) {
	
	sc_in_clk clock;
	sc_in<bool> ready_tosend,filterisdone;
	sc_out<bool> newimage_tofilter,send_tofilter;
	sc_out<sc_int<16> > rows_out, cols_out;
	sc_out<sc_uint<24> > rgb_out;
	sc_signal<bool> newimage,ready; 

	void imagecapture();
	void send_pic();
	
	SC_CTOR(input) {
		SC_CTHREAD(imagecapture, clock);
		SC_CTHREAD(send_pic, clock);
	}
};
