#include "systemc.h"
#include "input.h"
#include "filter.h"
//#include "input.cpp"

using namespace std;

int sc_main(int argc, char* argv[]){
	
	//cameracapture();
	
	sc_clock c1;
	sc_signal<bool> start,newimage,ready,sending;
	sc_signal<sc_uint<24> > RGB;
	sc_signal<sc_int<16> > col_size, row_size;

	//-----initializes input module ports to signals------
	input input("input");
	input.clock(c1);
	input.newimage(newimage);
	input.sending(sending);
	input.ready(ready);
	input.RGB_24_bit(RGB);
	input.max_cols(col_size);
	input.max_rows(row_size);
	input.start(start);
	//-------------------------------------------
	
	//-----initializes filter module ports to signals------
	filter filter("FILTER");
	filter.clock(c1);
	filter.newimage(newimage);
	filter.ready(ready);
	filter.RGB_24_bit(RGB);
	filter.max_cols(col_size);
	filter.max_rows(row_size);
	filter.sending(sending);
	//-------------------------------------------------
	

	
	
	start = true;
	//sc_start(5, SC_MS);
	sc_start();
	return(0);
}
