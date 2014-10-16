#include "systemc.h"
#include "filter.h"

//global array input and a copy of the input
int blue[480][640],green[480][640],red[480][640];
int blue_copy[480][640],green_copy[480][640],red_copy[480][640];

void filter::get_image(){
	int rows = 0, cols = 0;
	int max_rows,max_cols;
	
	while(true){
		//waits for a new image from the input module
		do wait();while(!newimage_finput);
		
		//sets the rows and column max
		max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
	
		while(true){
			//tells the input module that it is ready to receive the transfer
			ready_toinput = true;
			
			//waits for the input module to 
			do wait();while(!send_finput);
			
			//tells the input module to wait until the filter has been finish and transferred to the output module
			ready_toinput = false;
			wait();
			
			//reads in the 24 bit RGB pixels
			byte_to_pix = rgb_in.read();
			wait();
			
			//breaks up the 24 bit Pixels into separate coloured arrays
			blue[rows][cols] = byte_to_pix.range(7,0);
			green[rows][cols] = byte_to_pix.range(15,8);
			red[rows][cols] = byte_to_pix.range(23,16);
			
			cols++;
			
			if (cols >= max_cols){
				cols = 0;
				rows++;
			}
			
			if(rows >= max_rows){
				rows = 0;
				
				//tells the filters that there is a new image to be filtered
				newimage = true;
				wait();
				
				//waits on the filters to be ready for another image
				do wait();while(!waittosend);
				newimage = false;
				wait();
				filterisdone = true;
				wait();
				break;
			}
		}
	}
}

void filter::send_image(){

	unsigned short lsb,isb,msb;
	uint32_t rgb_24_bit; 
	int rows = 0,cols = 0;
	int max_rows,max_cols;
	
	while(true){
		//wait until the filter has finish its process
		do wait();while(!readytosend);
		
		//sets max rows and columns
		max_rows = rows_in.read().to_int();
		wait();
		max_cols = cols_in.read().to_int();
		wait();
	
		//sends the rows and columns to the output module
		rows_out.write(max_rows);
		wait();
		cols_out.write(max_cols);
		wait();
		
		//tells the output module that there is a new transfer to be sent
		newimage_toout = true;
		
		while(true){
			//wait until the filter has finish its process
			do wait();while(!ready_tosend);
			
			lsb = blue_copy[rows][cols];
			isb = green_copy[rows][cols];
			msb = red_copy[rows][cols];
			rgb_24_bit = lsb | isb << 8 | msb << 16;
			
			//transfer the 24 bit to the output
			rgb_out.write(rgb_24_bit);
			wait();
			
			//tells the output module that there is a new transfer
			send_toout = true;
			wait();
			
			cols++;
	
			if(cols >= max_cols){
				cols = 0;
				rows++;
			}
			
			if(rows >= max_rows){
				rows = 0;
				newimage_toout = false;
				wait();
				//waits on the output module to be ready for a new transfer
				do wait();while(!issave);
				sendingimage=true;
				wait();
				break;
			}			
		}
	}
}


void filter::laplace(){
	int rows;
	int cols;
	while(true){
		//waits to see if the user chooses this filter to use
		do wait();while(!laplace_in);
		//waits for a new image from the get_image function
		do wait();while(!newimage);
		waittosend = false;
		wait();
		
		//sets the rows and columns
		rows = rows_in.read().to_int();
		wait();
		cols = cols_in.read().to_int();
		wait();
		
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
				
				blue_copy[i][j] = laplace_val_blue;
				green_copy[i][j] = laplace_val_green;
				red_copy[i][j] = laplace_val_red;
			}
		}
		readytosend = true;
		wait();
		do wait(); while(!sendingimage);
		waittosend = true;
		wait();
		readytosend = false;
		wait();
	}
}

void filter::gauss(){
	int rows;
	int cols;
	while(true){		
		do wait();while(!gauss_in);

		do wait();while(!newimage);

		waittosend = false;
		wait();
		
		rows = rows_in.read().to_int();
		wait();
		
		cols = cols_in.read().to_int();
		wait();
		
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
				


				blue_copy[i][j] = blur_blue;
				green_copy[i][j] = blur_green;
				red_copy[i][j] = blur_red;
			}
		}
		readytosend = true;
		wait();
		do wait(); while(!sendingimage);
		waittosend = true;
		wait();
		readytosend = false;
		wait();
	}

}

void filter::median(){
	int rows;
	int cols;
	while(true){		
		do wait();while(!median_in);

		do wait();while(!newimage);

		waittosend = false;
		wait();
		
		rows = rows_in.read().to_int();
		wait();
		
		cols = cols_in.read().to_int();
		wait();
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
					
	
				blue_copy[i][j] = median_b;
				green_copy[i][j] = median_g;
				red_copy[i][j] = median_r;
			}
		}
		readytosend = true;
		wait();
		do wait(); while(!sendingimage);
		waittosend = true;
		wait();
		readytosend = false;
		wait();
	}
}

void filter::sobel(){
	int rows;
	int cols;
	while(true){		
		do wait();while(!sobel_in);

		do wait();while(!newimage);

		waittosend = false;
		wait();
		
		rows = rows_in.read().to_int();
		wait();
		
		cols = cols_in.read().to_int();
		wait();
		
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

				blue_copy[i][j] = gradient_blue;
				green_copy[i][j] = gradient_green;
				red_copy[i][j] = gradient_red;
			}
		}
		readytosend = true;
		wait();
		do wait(); while(!sendingimage);
		waittosend = true;
		wait();
		
		readytosend = false;
		wait();
	}
}

void filter::nofilter(){
	int rows;
	int cols;
	while(true){		
		do wait();while(!nofilter_in);

		do wait();while(!newimage);
		wait();
		
		waittosend = false;
		wait();
		
		rows = rows_in.read().to_int();
		wait();
		
		cols = cols_in.read().to_int();
		wait();
		

		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				blue_copy[i][j] = blue[i][j];
				green_copy[i][j] = green[i][j];
				red_copy[i][j] = red[i][j];				
			}
		}
		readytosend = true;
		wait();
		
		do wait(); while(!sendingimage);
		waittosend = true;
		wait();
		readytosend = false;
		wait();
	}
}
