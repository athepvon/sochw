#include "systemc.h"
#include "input.h"
#include "filter.h"
#include "output.h"
#include <iostream>

using namespace std;

void top(){
	sc_clock c1;
	sc_signal<bool> ready_if, image_if, send_if, newimage_fo;
	sc_signal<bool> send_fo, ready_fo, laplace_in, issave_, filterisdone_;
	sc_signal<sc_int<16> >rows_if,cols_if,rows_fo,cols_fo;
	sc_signal<sc_uint<24> >rgb_infil,rgb_filout;
	
	input input("input");
	input.clock(c1);
	input.ready_tosend(ready_if);
	input.newimage_tofilter(image_if);
	input.send_tofilter(send_if);
	input.rgb_out(rgb_infil);
	input.rows_out(rows_if);
	input.cols_out(cols_if);
	input.filterisdone(filterisdone_);
	
	filter filter("filter");
	filter.clock(c1);
	filter.newimage_finput(image_if);
	filter.send_finput(send_if);
	filter.ready_tosend(ready_fo);
	filter.ready_toinput(ready_if);
	filter.newimage_toout(newimage_fo);
	filter.send_toout(send_fo);
	filter.rows_out(rows_fo);
	filter.cols_out(cols_fo);
	filter.cols_in(cols_if);
	filter.rows_in(rows_if);
	filter.rgb_in(rgb_infil);
	filter.rgb_out(rgb_filout);
	filter.laplace_in(laplace_in);
	filter.filterisdone(filterisdone_);
	filter.issave(issave_);
	
	
	output output("output");
	output.clock(c1);
	output.newimage_ffil(newimage_fo);
	output.send_ffil(send_fo);
	output.ready_tofil(ready_fo);
	output.rows_in(rows_fo);
	output.cols_in(cols_fo);
	output.rgb_in(rgb_filout);
	output.issave(issave_);
	
	laplace_in = true;
}
