#include "systemc.h"

SC_MODULE(output){

	sc_in_clk clock;
	sc_in<bool> newimage_from_filter,send_from_filter;
	sc_out<bool> ready_to_filter;
	sc_in<sc_uint<24> > RGB_24_bit_in;
	sc_in<sc_int<16> > cols_in;
	sc_in<sc_int<16> > rows_in;
	sc_uint<24> byte_to_pix;
	
	void save_pic();
	
	SC_CTOR(output){
		SC_CTHREAD(save_pic, clock.pos());
	}
};


