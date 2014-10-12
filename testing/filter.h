 #include "systemc.h"

SC_MODULE(filter) {
	
	sc_in_clk clock;
	sc_in<bool> newimage,sending;//newimage verifies there is a new image. sending: verfies data sending
	sc_in<sc_uint<24> > RGB_24_bit;//24 bit rgb pixel port
	sc_out<bool> ready; //signal to tell the input module ready to recieve data
	sc_in<sc_int<16> >max_cols;
	sc_in<sc_int<16> >max_rows;
	sc_uint<24> byte_to_pix; // converts bytes to pixels
	sc_int<4> choose_filter;
	//1 = laplace; 2 = gauss; 3 = median; 4 = sobel
	
	void get_pic();
	
	//void laplace();
	//void gauss();
	//void median();
	//void sobel();
	
	SC_CTOR(filter) {
		SC_CTHREAD(get_pic, clock.pos());
		/*SC_METHOD(
		switch (choose_filter) {
			case 1: cout << "laplace" << endl; break;
			case 2: cout << "gauss" << endl; break;
			case 3: cout << "median" << endl; break;
			case 4: cout << "sobel" << endl; break;
		}
		);
			sensitive_pos << clock;*/
	}
};
