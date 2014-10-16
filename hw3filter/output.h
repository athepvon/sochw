#include "systemc.h"

SC_MODULE(output){
	sc_in_clk clock;
	sc_in<bool> newimage_ffil, send_ffil;
	sc_out<bool> ready_tofil,issave;
	sc_in<sc_int<16> > rows_in,cols_in;
	sc_in<sc_uint<24> > rgb_in;
	sc_uint<24> byte_to_pix;
	sc_signal<bool> newimage; 
	int blue_out[480][640],green_out[480][640],red_out[480][640];
	bool readytoshow;
	
	//receives the image from the filter module
	void get_image();
	//shows the image on the screen
	void show_image();
	
	SC_CTOR(output) {
		SC_CTHREAD(get_image, clock);
		SC_CTHREAD(show_image, clock);
	}
};
