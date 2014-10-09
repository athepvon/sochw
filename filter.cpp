// filter.cpp
#include "filter.h"

int in_data;

void filter::laplace(){
		while(true) {
		// wait for an incoming image
		do wait();
		while(!newimage);

		// get the array size
		int size_x;
		int size_y;

		// got an array size, create an empty one
		int color_channel[size_x][size_y];
		int color_channel_copy[size_x][size_y];

		// write the data into the array
		for(int i = 0; i < size_x; i++){
			for(int j = 0; j < size_y; j++){
				// read data on incoming channel
				
				// store data from incoming channel in array
				color_channel[i][j] = in_data;
				// wait one clock cycle between transfers
				wait();
			}
		}
		color_channel_copy = color_channel;


		// create sliding window / mask / kernel and fill it
		int kernel[3][3] =
		{
			0, -1, 0,
			-1, 4, -1,
			0, -1, 0
		};


		// begin looping over the image excluding edges
		for(int i = 1; i < (size_x - 1); i++){
			for(int j = 1; j < (size_y - 1); j++){

				// pass the kernel over the source
				int laplace_val =
				((color_channel[i-1][j-1] * kernel[0][0])) +
				((color_channel[i-1][j] * kernel[0][1])) +
				((color_channel[i-1][j+1] * kernel[0][2])) +
				((color_channel[i][j-1] * kernel[1][0])) +
				((color_channel[i][j] * kernel[1][1])) +
				((color_channel[i][j+1] * kernel[1][2])) +
				((color_channel[i+1][j-1] * kernel[2][0])) +
				((color_channel[i+1][j] * kernel[2][1])) +
				((color_channel[i+1][j+1] * kernel[2][2]));

				// change source image copy
				if(laplace_val < 0) {
					laplace_val = 0;
				}
				if(laplace_val > 255) {
					laplace_val = 255;
				}
				color_channel_copy[i][j] = blur;
			}
		}

		// send copy to output module

		// wait for finished from output module

		// send ready to input module and repeat twice
}

void filter::gauss(){
	while(true) {
		// wait for an incoming image
		do wait();
		while(!newimage);

		// get the array size
		int size_x;
		int size_y;

		// got an array size, create an empty one
		int color_channel[size_x][size_y];
		int color_channel_copy[size_x][size_y];

		// write the data into the array
		for(int i = 0; i < size_x; i++){
			for(int j = 0; j < size_y; j++){
				// read data on incoming channel
				
				// store data from incoming channel in array
				color_channel[i][j] = in_data;
				// wait one clock cycle between transfers
				wait();
			}
		}
		color_channel_copy = color_channel;


		// create sliding window / mask / kernel and fill it
		int kernel[3][3] =
		{
			1, 2, 1,
			2, 4, 2,
			1, 2, 1
		};

		// begin looping over the image excluding edges
		for(int i = 1; i < (size_x - 1); i++){
			for(int j = 1; j < (size_y - 1); j++){

				// pass the kernel over the source
				int blur =
				((color_channel[i-1][j-1] * kernel[0][0])/16) +
				((color_channel[i-1][j] * kernel[0][1])/16) +
				((color_channel[i-1][j+1] * kernel[0][2])/16) +
				((color_channel[i][j-1] * kernel[1][0])/16) +
				((color_channel[i][j] * kernel[1][1])/16) +
				((color_channel[i][j+1] * kernel[1][2])/16) +
				((color_channel[i+1][j-1] * kernel[2][0])/16) +
				((color_channel[i+1][j] * kernel[2][1])/16) +
				((color_channel[i+1][j+1] * kernel[2][2])/16);

				// change source image copy
				if(blur < 0) {
					blur = 0;
				}
				if(blur > 255) {
					blur = 255;
				}
				color_channel_copy[i][j] = blur;
			}
		}

		// send to output module

		// wait for finished from output module

		// send ready to input module and repeat twice
}

void filter::median(){
	while(true) {
		// wait for an incoming image
		do wait();
		while(!newimage);

		// get the array size
		int size_x;
		int size_y;

		// got an array size, create an empty one
		int color_channel[size_x][size_y];
		int color_channel_copy[size_x][size_y];


		// write the data into the array
		for(int i = 0; i < size_x; i++){
			for(int j = 0; j < size_y; j++){
				// read data on incoming channel
				
				// store data from incoming channel in array
				color_channel[i][j] = in_data;
				// wait one clock cycle between transfers
				wait();
			}
		}
		
		color_channel_copy = color_channel;

		// create sliding window / mask / kernel
		int kernel[3][3]

		// create array for sorting
		int sort_array[9];

		// begin looping over the image excluding edges
		for(int i = 1; i < (size_x - 1); i++){
			for(int j = 1; j < (size_y - 1); j++){
				
				// fill kernel
				kernel[0][0] = color_channel[i-1][j-1];
				kernel[0][1] = color_channel[i][j-1];
				kernel[0][2] = color_channel[i+1][j-1];
				kernel[1][0] = color_channel[i-1][j];
				kernel[1][1] = color_channel[i][j];
				kernel[1][2] = color_channel[i+1][j];
				kernel[2][0] = color_channel[i-1][j+1];
				kernel[2][1] = color_channel[i][j+1];
				kernel[2][2] = color_channel[i+1][j+1];

				// fill sorting array
				int tmp = 0;
				for(int k = 0; k < 3; k++){
					for(int l = 0; l < 3; l++){
						sort_array[tmp] = kernel[k][l];
						tmp++;
					}
				}

				// sort array and find median value
				for (int m = 8; m > 0; --m){
					if(sort_array[m] > sort_array[m+1] {
						int temp = sort_array[m];
						sort_array[m] = sort_array[m+1];
						sort_array[m+1] = temp;
					}
				}
				int median = sort_array[5];

				// change source image
				if(median < 0){
					median = 0;
				}
				if(median > 255){
					median = 255;
				}
				color_channel_copy[i][j] = median;
			}
		}

		// send copy array to output module

		// wait for finished from output module

		// send ready to input module and repeat twice
}

void filter::sobel(){
		while(true) {
		// wait for an incoming image
		do wait();
		while(!newimage);

		// get the array size
		int size_x;
		int size_y;

		// got an array size, create an empty one
		int color_channel[size_x][size_y];
		int color_channel_copy[size_x][size_y];

		// write the data into the array
		for(int i = 0; i < size_x; i++){
			for(int j = 0; j < size_y; j++){
				// read data on incoming channel
				
				// store data from incoming channel in array
				color_channel[i][j] = in_data;
				// wait one clock cycle between transfers
				wait();
			}
		}
		color_channel_copy = color_channel;


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
		for(int i = 1; i < (size_x - 1); i++){
			for(int j = 1; j < (size_y - 1); j++){

				// pass the first kernel over the source
				int x_edge =
				((color_channel[i-1][j-1] * kernel_x[0][0])) +
				((color_channel[i-1][j] * kernel_x[0][1])) +
				((color_channel[i-1][j+1] * kernel_x[0][2])) +
				((color_channel[i][j-1] * kernel_x[1][0])) +
				((color_channel[i][j] * kernel_x[1][1])) +
				((color_channel[i][j+1] * kernel_x[1][2])) +
				((color_channel[i+1][j-1] * kernel_x[2][0])) +
				((color_channel[i+1][j] * kernel_x[2][1])) +
				((color_channel[i+1][j+1] * kernel_x[2][2]));

				// pass the second kernel over the source
				int y_edge =
				((color_channel[i-1][j-1] * kernel_y[0][0])) +
				((color_channel[i-1][j] * kernel_y[0][1])) +
				((color_channel[i-1][j+1] * kernel_y[0][2])) +
				((color_channel[i][j-1] * kernel_y[1][0])) +
				((color_channel[i][j] * kernel_y[1][1])) +
				((color_channel[i][j+1] * kernel_y[1][2])) +
				((color_channel[i+1][j-1] * kernel_y[2][0])) +
				((color_channel[i+1][j] * kernel_y[2][1])) +
				((color_channel[i+1][j+1] * kernel_y[2][2]));

				// approximate the gradient vector g = sqrt(x^2 + y^2) with g = |x| + |y|
				int gradient = std::abs(x_edge) + std::abs(y_edge);

				// change source image copy
				if(gradient < 0) {
					gradient = 0;
				}
				if(gradient > 255) {
					gradient = 255;
				}
				color_channel_copy[i][j] = blur;
			}
		}

		// send to output module

		// wait for finished from output module

		// send ready to input module and repeat twice for other two color channels

}
