#include "systemc.h"
#include "filter.h"
#include <iostream>
#include <fstream>
using namespace std;
int blue[960][1280],green[960][1280],red[960][1280];
int blue_channel_copy[960][1280],green_channel_copy[960][1280],red_channel_copy[960][1280];

void copy_channels(int rows, int cols){
	int size_x = cols;
	int size_y = rows;
		
	for(int i = 0; i < size_y; i++){
		for(int j = 0; j < size_x; j++){
			blue_channel_copy[i][j] = blue[i][j];
			green_channel_copy[i][j] = green[i][j];
			red_channel_copy[i][j] = red[i][j];				
		}
	}
}

bool laplace(int rows, int cols){

		copy_channels(rows,cols);
		
		// create sliding window / mask / kernel and fill it
		int kernel[3][3] =
		{
			0, -1, 0,
			-1, 4, -1,
			0, -1, 0
		};


		// begin looping over the image excluding edges
		for(int i = 1; i < (rows - 1); i++){
			for(int j = 1; j < (cols - 1); j++){

				// pass the kernel over the source
				int laplace_val_blue =
				((blue[i-1][j-1] * kernel[0][0])) +
				((blue[i-1][j] * kernel[0][1])) +
				((blue[i-1][j+1] * kernel[0][2])) +
				((blue[i][j-1] * kernel[1][0])) +
				((blue[i][j] * kernel[1][1])) +
				((blue[i][j+1] * kernel[1][2])) +
				((blue[i+1][j-1] * kernel[2][0])) +
				((blue[i+1][j] * kernel[2][1])) +
				((blue[i+1][j+1] * kernel[2][2]));
				
				int laplace_val_green =
				((green[i-1][j-1] * kernel[0][0])) +
				((green[i-1][j] * kernel[0][1])) +
				((green[i-1][j+1] * kernel[0][2])) +
				((green[i][j-1] * kernel[1][0])) +
				((green[i][j] * kernel[1][1])) +
				((green[i][j+1] * kernel[1][2])) +
				((green[i+1][j-1] * kernel[2][0])) +
				((green[i+1][j] * kernel[2][1])) +
				((green[i+1][j+1] * kernel[2][2]));
				
				int laplace_val_red =
				((red[i-1][j-1] * kernel[0][0])) +
				((red[i-1][j] * kernel[0][1])) +
				((red[i-1][j+1] * kernel[0][2])) +
				((red[i][j-1] * kernel[1][0])) +
				((red[i][j] * kernel[1][1])) +
				((red[i][j+1] * kernel[1][2])) +
				((red[i+1][j-1] * kernel[2][0])) +
				((red[i+1][j] * kernel[2][1])) +
				((red[i+1][j+1] * kernel[2][2]));

				// change source image copy
				if(laplace_val_blue < 0) {
					laplace_val_blue = 0;
				}
				if(laplace_val_blue > 255) {
					laplace_val_blue = 255;
				}
				
				if(laplace_val_green < 0) {
					laplace_val_green = 0;
				}
				if(laplace_val_green > 255) {
					laplace_val_green = 255;
				}
				
				if(laplace_val_red < 0) {
					laplace_val_red = 0;
				}
				if(laplace_val_red > 255) {
					laplace_val_red = 255;
				}
				
				blue_channel_copy[i][j] = laplace_val_blue;
				green_channel_copy[i][j] = laplace_val_green;
				red_channel_copy[i][j] = laplace_val_red;
			}
		}
		return true;
}

bool gauss(int rows, int cols){
		
		copy_channels(rows,cols);
		

		// create sliding window / mask / kernel and fill it
		int kernel[3][3] =
		{
			1, 2, 1,
			2, 4, 2,
			1, 2, 1
		};

		// begin looping over the image excluding edges
		for(int i = 1; i < (rows - 1); i++){
			for(int j = 1; j < (cols - 1); j++){

				// pass the kernel over the source
				int blur_blue =
				((blue[i-1][j-1] * kernel[0][0])/16) +
				((blue[i-1][j] * kernel[0][1])/16) +
				((blue[i-1][j+1] * kernel[0][2])/16) +
				((blue[i][j-1] * kernel[1][0])/16) +
				((blue[i][j] * kernel[1][1])/16) +
				((blue[i][j+1] * kernel[1][2])/16) +
				((blue[i+1][j-1] * kernel[2][0])/16) +
				((blue[i+1][j] * kernel[2][1])/16) +
				((blue[i+1][j+1] * kernel[2][2])/16);
				
				int blur_green =
				((green[i-1][j-1] * kernel[0][0])/16) +
				((green[i-1][j] * kernel[0][1])/16) +
				((green[i-1][j+1] * kernel[0][2])/16) +
				((green[i][j-1] * kernel[1][0])/16) +
				((green[i][j] * kernel[1][1])/16) +
				((green[i][j+1] * kernel[1][2])/16) +
				((green[i+1][j-1] * kernel[2][0])/16) +
				((green[i+1][j] * kernel[2][1])/16) +
				((green[i+1][j+1] * kernel[2][2])/16);


				int blur_red =
				((red[i-1][j-1] * kernel[0][0])/16) +
				((red[i-1][j] * kernel[0][1])/16) +
				((red[i-1][j+1] * kernel[0][2])/16) +
				((red[i][j-1] * kernel[1][0])/16) +
				((red[i][j] * kernel[1][1])/16) +
				((red[i][j+1] * kernel[1][2])/16) +
				((red[i+1][j-1] * kernel[2][0])/16) +
				((red[i+1][j] * kernel[2][1])/16) +
				((red[i+1][j+1] * kernel[2][2])/16);

				// change source image copy
				if(blur_blue < 0) {
					blur_blue = 0;
				}
				if(blur_blue > 255) {
					blur_blue = 255;
				}
				
				if(blur_green < 0) {
					blur_green = 0;
				}
				if(blur_green > 255) {
					blur_green = 255;
				}
				
				if(blur_red < 0) {
					blur_red = 0;
				}
				if(blur_red > 255) {
					blur_red = 255;
				}
				


				blue_channel_copy[i][j] = blur_blue;
				green_channel_copy[i][j] = blur_green;
				red_channel_copy[i][j] = blur_red;
			}
		}
	return true;

}

bool median(int rows, int cols){
	
	copy_channels(rows, cols);

	// create kernel
	int kernel_b[3][3];
	int kernel_r[3][3];
	int kernel_g[3][3];

	// create array for sorting
	int sort_array_b[9];
	int sort_array_r[9];
	int sort_array_g[9];

	// begin looping over the image excluding edges
	for(int i = 1; i < (rows - 1); i++){
		for(int j = 1; j < (cols - 1); j++){

			// fill kernels
			kernel_b[0][0] = blue[i-1][j-1];
			kernel_b[0][1] = blue[i][j-1];
			kernel_b[0][2] = blue[i+1][j-1];
			kernel_b[1][0] = blue[i-1][j];
			kernel_b[1][1] = blue[i][j];
			kernel_b[1][2] = blue[i+1][j];
			kernel_b[2][0] = blue[i-1][j+1];
			kernel_b[2][1] = blue[i][j+1];
			kernel_b[2][2] = blue[i+1][j+1];

			kernel_r[0][0] = red[i-1][j-1];
			kernel_r[0][1] = red[i][j-1];
			kernel_r[0][2] = red[i+1][j-1];
			kernel_r[1][0] = red[i-1][j];
			kernel_r[1][1] = red[i][j];
			kernel_r[1][2] = red[i+1][j];
			kernel_r[2][0] = red[i-1][j+1];
			kernel_r[2][1] = red[i][j+1];
			kernel_r[2][2] = red[i+1][j+1];

			kernel_g[0][0] = green[i-1][j-1];
			kernel_g[0][1] = green[i][j-1];
			kernel_g[0][2] = green[i+1][j-1];
			kernel_g[1][0] = green[i-1][j];
			kernel_g[1][1] = green[i][j];
			kernel_g[1][2] = green[i+1][j];
			kernel_g[2][0] = green[i-1][j+1];
			kernel_g[2][1] = green[i][j+1];
			kernel_g[2][2] = green[i+1][j+1];

			// fill sorting arrays
			int tmp_b = 0;
			for(int k = 0; k < 3; k++){
				for(int l = 0; l < 3; l++){
					sort_array_b[tmp_b] = kernel_b[k][l];
					tmp_b++;
				}
			}

			int tmp_r = 0;
			for(int k = 0; k < 3; k++){
				for(int l = 0; l < 3; l++){
					sort_array_r[tmp_r] = kernel_r[k][l];
					tmp_r++;
				}
			}

			int tmp_g = 0;
			for(int k = 0; k < 3; k++){
				for(int l = 0; l < 3; l++){
					sort_array_g[tmp_g] = kernel_g[k][l];
					tmp_g++;
				}
			}

			// sort arrays and find median value
			for (int m = 8; m > 0; m--){
				if(sort_array_b[m] > sort_array_b[m+1]) {
					int temp = sort_array_b[m];
					sort_array_b[m] = sort_array_b[m+1];
					sort_array_b[m+1] = temp;
				}
			}
			int median_b = sort_array_b[5];

			for (int m = 8; m > 0; m--){
				if(sort_array_r[m] > sort_array_r[m+1]) {
					int temp = sort_array_r[m];
					sort_array_r[m] = sort_array_r[m+1];
					sort_array_r[m+1] = temp;
				}
			}
			int median_r = sort_array_r[5];

			for (int m = 8; m > 0; m--){
				if(sort_array_g[m] > sort_array_g[m+1]) {
					int temp = sort_array_g[m];
					sort_array_g[m] = sort_array_g[m+1];
					sort_array_g[m+1] = temp;
				}
			}
			int median_g = sort_array_g[5];


			// change source image copy
			if(median_b < 0) {
				median_b = 0;
			}
			if(median_b > 255) {
				median_b = 255;
			}
				
			if(median_r < 0) {
				median_r = 0;
			}
			if(median_r > 255) {
				median_r = 255;
			}
				
			if(median_g < 0) {
				median_g = 0;
			}
			if(median_g > 255) {
				median_g = 255;
			}
				

			blue_channel_copy[i][j] = median_b;
			green_channel_copy[i][j] = median_g;
			red_channel_copy[i][j] = median_r;
		}
	}
	return true;

}

bool sobel(int rows, int cols){
		
		copy_channels(rows,cols);
		

		// create horizontal edge sliding window / mask / kernel and fill it
		int kernel_x[3][3] =
		{
			-1, 0, 1,
			-2, 0, 2,
			-1, 0, 1
		};

		// create vertical edge sliding window / mask / kernel and fill it
		int kernel_y[3][3] =
		{
			-1, -2, -1,
			0, 0, 0,
			1, 2, 1
		};

		// begin looping over the image excluding edges
		for(int i = 1; i < (rows - 1); i++){
			for(int j = 1; j < (cols - 1); j++){

				// pass the first kernel over the blue source
				int x_edge_blue =
				((blue[i-1][j-1] * kernel_x[0][0])) +
				((blue[i-1][j] * kernel_x[0][1])) +
				((blue[i-1][j+1] * kernel_x[0][2])) +
				((blue[i][j-1] * kernel_x[1][0])) +
				((blue[i][j] * kernel_x[1][1])) +
				((blue[i][j+1] * kernel_x[1][2])) +
				((blue[i+1][j-1] * kernel_x[2][0])) +
				((blue[i+1][j] * kernel_x[2][1])) +
				((blue[i+1][j+1] * kernel_x[2][2]));

				// pass the second kernel over the blue source
				int y_edge_blue =
				((blue[i-1][j-1] * kernel_y[0][0])) +
				((blue[i-1][j] * kernel_y[0][1])) +
				((blue[i-1][j+1] * kernel_y[0][2])) +
				((blue[i][j-1] * kernel_y[1][0])) +
				((blue[i][j] * kernel_y[1][1])) +
				((blue[i][j+1] * kernel_y[1][2])) +
				((blue[i+1][j-1] * kernel_y[2][0])) +
				((blue[i+1][j] * kernel_y[2][1])) +
				((blue[i+1][j+1] * kernel_y[2][2]));

				// pass the first kernel over the red source
				int x_edge_red =
				((red[i-1][j-1] * kernel_x[0][0])) +
				((red[i-1][j] * kernel_x[0][1])) +
				((red[i-1][j+1] * kernel_x[0][2])) +
				((red[i][j-1] * kernel_x[1][0])) +
				((red[i][j] * kernel_x[1][1])) +
				((red[i][j+1] * kernel_x[1][2])) +
				((red[i+1][j-1] * kernel_x[2][0])) +
				((red[i+1][j] * kernel_x[2][1])) +
				((red[i+1][j+1] * kernel_x[2][2]));

				// pass the second kernel over the red source
				int y_edge_red =
				((red[i-1][j-1] * kernel_y[0][0])) +
				((red[i-1][j] * kernel_y[0][1])) +
				((red[i-1][j+1] * kernel_y[0][2])) +
				((red[i][j-1] * kernel_y[1][0])) +
				((red[i][j] * kernel_y[1][1])) +
				((red[i][j+1] * kernel_y[1][2])) +
				((red[i+1][j-1] * kernel_y[2][0])) +
				((red[i+1][j] * kernel_y[2][1])) +
				((red[i+1][j+1] * kernel_y[2][2]));

				// pass the first kernel over the green source
				int x_edge_green =
				((green[i-1][j-1] * kernel_x[0][0])) +
				((green[i-1][j] * kernel_x[0][1])) +
				((green[i-1][j+1] * kernel_x[0][2])) +
				((green[i][j-1] * kernel_x[1][0])) +
				((green[i][j] * kernel_x[1][1])) +
				((green[i][j+1] * kernel_x[1][2])) +
				((green[i+1][j-1] * kernel_x[2][0])) +
				((green[i+1][j] * kernel_x[2][1])) +
				((green[i+1][j+1] * kernel_x[2][2]));

				// pass the second kernel over the green source
				int y_edge_green =
				((green[i-1][j-1] * kernel_y[0][0])) +
				((green[i-1][j] * kernel_y[0][1])) +
				((green[i-1][j+1] * kernel_y[0][2])) +
				((green[i][j-1] * kernel_y[1][0])) +
				((green[i][j] * kernel_y[1][1])) +
				((green[i][j+1] * kernel_y[1][2])) +
				((green[i+1][j-1] * kernel_y[2][0])) +
				((green[i+1][j] * kernel_y[2][1])) +
				((green[i+1][j+1] * kernel_y[2][2]));

				// approximate the gradient vector g = sqrt(x^2 + y^2) with g = |x| + |y|
				int gradient_blue = std::abs(x_edge_blue) + std::abs(y_edge_blue);
				int gradient_red = std::abs(x_edge_red) + std::abs(y_edge_red);
				int gradient_green = std::abs(x_edge_green) + std::abs(y_edge_green);

				// change source image copy
				if(gradient_blue < 0) {
					gradient_blue = 0;
				}
				if(gradient_blue > 255) {
					gradient_blue = 255;
				}
				
				if(gradient_green < 0) {
					gradient_green = 0;
				}
				if(gradient_green > 255) {
					gradient_green = 255;
				}
				
				if(gradient_red < 0) {
					gradient_red = 0;
				}
				if(gradient_red > 255) {
					gradient_red = 255;
				}
				

				blue_channel_copy[i][j] = gradient_blue;
				green_channel_copy[i][j] = gradient_green;
				red_channel_copy[i][j] = gradient_red;
			}
		}
	return true;

}

void filter::filter_pic(){
	
	int col_size;
	int row_size;
	int rows = 0, cols = 0;
	bool filter_is = false;
	bool output_ready = false;
	int filter = choose_filter.read().to_int();
	unsigned short lsb,isb,msb; //assign least sig bit, inner sig bit, most sig bit variables
	uint32_t rgb_24_bit; //assign 24 bit rgb signal variables	
	
	while(true) {

		//cout << "come in" << endl;
		do wait(); while(!newimage_from_input);
		
		col_size = cols_in.read().to_int();
		row_size = rows_in.read().to_int();
		//cout << "set " << max_cols << " x " << max_rows << endl;
		//cout << "got the newimage" << endl;
		ready_to_input = true;
		do wait();while(!send_from_input);
		
		ready_to_input = false;
		wait();

		byte_to_pix = RGB_24_bit.read();
		wait();

		blue[rows][cols] = byte_to_pix.range(7,0);
		green[rows][cols] = byte_to_pix.range(15,8);
		red[rows][cols] = byte_to_pix.range(23,16);

		ready_to_input = true;
		wait();
		
		cols++;	
		if (cols >= col_size ) {
			cols = 0;
			rows++;
		}

		if(rows >= row_size){
			//cout << "rows : " << rows << endl;
			//cout << "cols : " << cols << endl;
			rows = 0;
			//cout << "inside row if" << endl;
			switch (filter){
			case 1: filter_is = laplace(row_size,col_size); break;
			case 2: filter_is = gauss(row_size,col_size); break;
			case 3: filter_is = median(row_size,col_size); break;
			case 4: filter_is = sobel(row_size, col_size); break;
			}
			output_ready = true;
			break;
		}
		
	}
	
	
	//----------send to output---------------	
	if (output_ready && filter_is){
	
		while(true){
	
			send_to_output = false;
		
			newimage_to_output = true;
		
			do wait();while(!ready_from_output);	
		
			lsb = blue_channel_copy[rows][cols];				
			isb = green_channel_copy[rows][cols];			
			msb = red_channel_copy[rows][cols];

			rgb_24_bit = lsb | isb << 8 | msb << 16;
			wait();
		
			RGB_24_bit_out.write(rgb_24_bit);
			wait();
			
			send_to_output = true;
			wait();
			
			cols++;
			
			if(cols >= col_size){
				cols = 0;
				rows++;
				wait();
			}
			
			if(rows >= row_size){
				rows = 0;
				//cout << "finished the output " << endl;
				break;
			}		
			
		}
	}
}



