 #include "systemc.h"

SC_MODULE(filter) {
	
	sc_in_clk clock;
	sc_in<bool> newimage,sending;//newimage verifies there is a new image. sending: verfies data sending
	sc_in<sc_uint<24> > RGB_24_bit;//24 bit rgb pixel port
	sc_out<bool> ready; //signal to tell the input module ready to recieve data
	sc_in<sc_int<16> >max_cols;
	sc_in<sc_int<16> >max_rows;
	sc_uint<24> byte_to_pix; // converts bytes to pixels

	void get_pic();
	
	SC_CTOR(filter) {
		SC_CTHREAD(get_pic, clock.pos());
	}
};