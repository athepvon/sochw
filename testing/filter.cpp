#include "systemc.h"
#include "filter.h"
#include <iostream>
using namespace std;
int blue[960][1280],green[960][1280],red[960][1280];

void filter::get_pic(){
	int col_size;// = max_cols.read().to_int();
	int row_size;// = max_rows.read().to_int();
	
	int rows = 0, cols = 0;
	
	while(true) {

		//cout << "come in" << endl;
		do wait(); while(!newimage);
		
		col_size = max_cols.read().to_int();
		row_size = max_rows.read().to_int();
		//cout << "set " << max_cols << " x " << max_rows << endl;
		//cout << "got the newimage" << endl;
		ready = true;
		wait();
		
		//ready = false;
		//wait();
		do wait();while(!sending);
		ready = false;
		wait();
		
		//cout << "got the sending" << endl;
		byte_to_pix = RGB_24_bit.read();
		wait();
		
		//loop = false;
		//wait();

		//cout << "cols in filter " << cols << endl;
		//cout << "rows in filter " << rows << endl;	

		blue[rows][cols] = byte_to_pix.range(7,0);
		//cout << "blue: " << blue[rows][cols] << endl;
		wait();
		
		green[rows][cols] = byte_to_pix.range(15,8);
		//cout << "green: " << green[rows][cols] << endl;
		wait();
		
		red[rows][cols] = byte_to_pix.range(23,16);
		//cout << "red: " << red[rows][cols] << endl;
		wait();

		cols++;					
		
		//cout << "rows : " << rows << endl;
		//cout << "cols : " << cols << endl;
		ready = true;
		wait();

		if (cols >= col_size ) {
			cols = 0;
			rows++;
		}

		if(rows >= row_size){
			cout << "rows : " << rows << endl;
			cout << "cols : " << cols << endl;
			rows = 0;
			cout << "inside row if" << endl;
			break;
		}
		//loop = true;
	}
	sc_stop();
}
