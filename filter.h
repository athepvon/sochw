// filter.h
#include "systemc.h"
#include <iostream>
#include <cmath>


SC_MODULE(filter) {
	

	void laplace();
	void gauss();
	void median();
	void sobel();


	SC_CTOR(filter) {
		SC_METHOD(median);
		sensitive_pos << clk;
	}

};
