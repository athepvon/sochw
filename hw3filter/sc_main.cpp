#include "systemc.h"
#include "input.h"
#include "filter.h"
#include "output.h"
#include <iostream>

using namespace std;

int sc_main(int argc, char* argv[]){
	//signal variables
	sc_clock c1;
	sc_signal<bool> ready_if, image_if, send_if, newimage_fo, median_in,sobel_in;
	sc_signal<bool> send_fo, ready_fo, laplace_in, issave, filterisdone, gauss_in, nofilter_in;
	sc_signal<sc_int<16> >rows_if,cols_if,rows_fo,cols_fo;
	sc_signal<sc_uint<24> >rgb_infil,rgb_filout;
	int choose_filter;
	
	//input module signal binding
	input input("input");
	input.clock(c1);
	input.ready_tosend(ready_if);
	input.newimage_tofilter(image_if);
	input.send_tofilter(send_if);
	input.rgb_out(rgb_infil);
	input.rows_out(rows_if);
	input.cols_out(cols_if);
	input.filterisdone(filterisdone);
	
	//filter module signal binding
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
	filter.gauss_in(gauss_in);
	filter.median_in(median_in);
	filter.sobel_in(sobel_in);
	filter.nofilter_in(nofilter_in);
	filter.filterisdone(filterisdone);
	filter.issave(issave);
	
	//output module signal binding
	output output("output");
	output.clock(c1);
	output.newimage_ffil(newimage_fo);
	output.send_ffil(send_fo);
	output.ready_tofil(ready_fo);
	output.rows_in(rows_fo);
	output.cols_in(cols_fo);
	output.rgb_in(rgb_filout);
	output.issave(issave);
	
	//input command to determine which filter to use
	cout << "Please choose a filter." << endl;
	cout << "1: No Filter 2: Laplace Filter | 3: Gaussian Filter | 4: Median Filter | 5: Sobel Filter" << endl;
	cin >> choose_filter;
	
	switch(choose_filter){
		case 1: nofilter_in = true; break;
		case 2: laplace_in = true; break;
		case 3: gauss_in = true; break;
		case 4: median_in = true; break;
		case 5: sobel_in = true; break;
	}
	cout << "loading........." << endl;
	
	sc_start();
	return 0;
}
