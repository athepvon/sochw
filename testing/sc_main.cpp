#include "systemc.h"
#include "input.h"
#include "filter.h"
#include "output.h"
#include <iostream>
using namespace std;

int sc_main(int argc, char* argv[]){
	
	sc_clock c1;
	sc_signal<bool> start,newimage_in_fil,newimage_fil_out,ready_fil_in;
	sc_signal<bool> send_in_fil,ready_out_fil,send_fil_out;
	
	sc_signal<sc_uint<24> > RGB_in_fil,RGB_fil_out;
	sc_signal<sc_int<16> > col_size, row_size;
	sc_signal<sc_int<4> > sig_filter;
	
	//-----initializes input module ports to signals------
	input input("input");
	input.clock(c1);
	input.newimage(newimage_in_fil);
	input.sending(send_in_fil);
	input.ready(ready_fil_in);
	input.RGB_24_bit(RGB_in_fil);
	input.cols_out(col_size);
	input.rows_out(row_size);
	input.start(start);
	//-------------------------------------------
	
	//-----initializes filter module ports to signals------
	filter filter("FILTER");
	filter.clock(c1);
	filter.newimage_from_input(newimage_in_fil);
	filter.ready_to_input(ready_fil_in);
	filter.ready_from_output(ready_out_fil);
	filter.RGB_24_bit(RGB_in_fil);
	filter.RGB_24_bit_out(RGB_fil_out);
	filter.cols_in(col_size);
	filter.rows_in(row_size);
	filter.send_from_input(send_in_fil);
	filter.send_to_output(send_fil_out);
	filter.newimage_to_output(newimage_fil_out);
	filter.choose_filter(sig_filter);
	//-------------------------------------------------
	

	//------initializes output module ports to signals---------
	output output("output");
	output.clock(c1);
	output.newimage_from_filter(newimage_fil_out);
	output.send_from_filter(send_fil_out);
	output.ready_to_filter(ready_out_fil);
	output.RGB_24_bit_in(RGB_fil_out);
	output.cols_in(col_size);
	output.rows_in(row_size);
	//to determine which filter to use 1 = laplace; 2 = gauss; 3 = median; 4 = sobel
	sig_filter = 2;
	start = true;
	sc_start();
	return(0);
}
