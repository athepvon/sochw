 #include "systemc.h"

SC_MODULE(filter) {
	
	sc_in_clk clock;
	sc_in<bool> newimage_from_input,send_from_input,ready_from_output;//newimage verifies there is a new image. sending: verfies data sending
	sc_in<sc_uint<24> > RGB_24_bit;//24 bit rgb pixel port in
	sc_out<sc_uint<24> > RGB_24_bit_out;//24 bit rgb pixel port out
	sc_out<bool> ready_to_input,newimage_to_output,send_to_output; 
	sc_in<sc_int<16> >cols_in;
	sc_in<sc_int<16> >rows_in;
	sc_uint<24> byte_to_pix; // converts bytes to pixels
	sc_in<sc_int<4> >choose_filter;

	//1 = laplace; 2 = gauss; 3 = median; 4 = sobel
	
	void filter_pic();
	
	//void laplace();
	//void gauss();
	//void median();
	//void sobel();

	SC_CTOR(filter) {
		SC_CTHREAD(filter_pic, clock.pos());

		//SC_METHOD(gauss);
		//sensitive_pos << clock;
/*
		SC_METHOD(laplace);
		sensitive_pos << clock;

		SC_METHOD(laplace);
		sensitive_pos << clock;

		SC_METHOD(laplace);
		sensitive_pos << clock;*/
	}
};
