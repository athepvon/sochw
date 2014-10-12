#include "systemc.h"

SC_MODULE(input) {
	
	sc_in_clk clock;
	sc_out<bool> newimage,sending,start; //newimage verifies there is a new image. 
										//sending: verfies data sending, start: verifies to start the input module
	sc_out<sc_uint<24> > RGB_24_bit; //24 bit rgb pixel port
	sc_in<bool> ready;	//signal from filter module to start the transfer
	sc_out<sc_int<16> >cols_out;
	sc_out<sc_int<16> >rows_out;
	
	void send_pic();
	
	SC_CTOR(input) {
		SC_CTHREAD(send_pic, clock.pos());
	}
};
