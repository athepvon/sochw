#include "systemc.h"

SC_MODULE(filter) {
	
	sc_in_clk clock;
	sc_in<bool> newimage_finput,send_finput,ready_tosend,issave;
	sc_in<bool> laplace_in, gauss_in, median_in, sobel_in, nofilter_in;
	sc_out<bool> ready_toinput,newimage_toout,send_toout,filterisdone;
	sc_out<sc_int<16> >rows_out,cols_out;
	sc_in<sc_int<16> >rows_in,cols_in;
	sc_out<sc_uint<24> >rgb_out;
	sc_in<sc_uint<24> >rgb_in;
	sc_uint<24> byte_to_pix;
	sc_signal<bool> newimage; 
	bool waittosend,readytosend,sendingimage;
	
	//get the image from the input module
	void get_image();
	
	//sends the image to the output module after filters
	void send_image();
	
	//laplace filter
	void laplace();
	
	//gaussian filter
	void gauss();
	
	//median blur filter
	void median();
	
	//sobel filter
	void sobel();
	
	//no filter
	void nofilter();
	
	//constructor for the filter module
	SC_CTOR(filter){
		SC_CTHREAD(get_image, clock);
		SC_CTHREAD(send_image, clock);
		SC_CTHREAD(laplace, clock);		
		SC_CTHREAD(gauss, clock);
		SC_CTHREAD(median, clock);		
		SC_CTHREAD(sobel, clock);
		SC_CTHREAD(nofilter, clock);
	}
};
