/*
 * main.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: arnaud
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "plb_sdi_controller.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
int deviceDriverFile;
uint8_t *dst_buffer; uint8_t *mapped_base;
IplImage  *gray_image;
void openDeviceDriver( )
{

	if ((deviceDriverFile = open("/dev/plb_sdi_ctrl", O_RDWR)) != -1)
	{  // must be O_RDWR to enable mmap() with MAP_SHARED

		        dst_buffer = (uint8_t*)mmap(0, BUFFER_SIZE*16, PROT_READ | PROT_WRITE, MAP_SHARED, deviceDriverFile, 0); // must be MAP_SHARED                      
			if(dst_buffer == MAP_FAILED) {
				perror("mmap() failed");
				exit(1);
			}
                        else{
                                unsigned long tmp = 0;unsigned long tmp1 = 0;
				ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCSHWREGNUM, &tmp);
				ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCGHWREG, &tmp);
				printf( "HW Version:Magic: %08X\n", tmp );
				printf( "cap dist buffer: %08X\n", dst_buffer );
                                tmp = 9; tmp1=0;
                                //printf( "video mode color\n");  
				//ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCSHWREGNUM, &tmp);
				//ioctl(deviceDriverFile, PLB_SDI_CTRL_IOCGHWREG, &tmp1);
                        }
	}
	else{
             perror("plb_sdi_controller_apps open");
             exit(1);
        }
}


int n_boards = 0;		//Number of snapshots of the chessboard
int frame_step;			//Frames to be skipped
int board_w;			//Enclosed corners horizontally on the chessboard
int board_h;			//Enclosed corners vertically on the chessboard

int main()
{
openDeviceDriver();
uint8_t *dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
static int tmp1 = 0;static int tmp2 = 0;static int tmp = 0;
	//CvCapture* capture;
         printf("Enter Calibratrion code...\n");
	// printf("Enter the numbers of spanspots = ");
	//scanf("%d",&n_boards);

	//printf("\rEnter the numbers of frames to skip = ");
	//scanf("%d",&frame_step);
        n_boards =5;
        frame_step =3;
	board_w  = 3;
	board_h  = 3;

	int board_total  = board_w * board_h;		  //Total enclosed corners on the board
	CvSize board_sz = cvSize( board_w, board_h );

	/*capture = cvCreateCameraCapture( 0 );
	if(!capture)
	{
		printf("\nCouldn't open the camera\n");
		return -1;
	}*/

	//cvNamedWindow( "Snapshot" );
	//cvNamedWindow( "Raw Video");

	//Allocate storage for the parameters according to total number of corners and number of snapshots
	CvMat* image_points      = cvCreateMat(n_boards*board_total,2,CV_32FC1);
	CvMat* object_points     = cvCreateMat(n_boards*board_total,3,CV_32FC1);
	CvMat* point_counts      = cvCreateMat(n_boards,1,CV_32SC1);
	CvMat* intrinsic_matrix  = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs = cvCreateMat(4,1,CV_32FC1);

	//Note:
	//Intrinsic Matrix - 3x3			   Lens Distorstion Matrix - 4x1
	//	[fx 0 cx]							[k1 k2 p1 p2   k3(optional)]
	//	[0 fy cy]
	//	[0  0  1]


	CvPoint2D32f* corners = new CvPoint2D32f[ board_total ];
	int corner_count;
	int successes = 0;
	int step, frame = 0;

	//IplImage *image = cvQueryFrame( capture );
	//IplImage *gray_image = cvCreateImage(cvGetSize(image),8,1);			//subpixel
        gray_image = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
        ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1); 
        gray_image->imageData = (char *) (dst_buffer);  

	//Loop while successful captures equals total snapshots
	//Successful captures implies when all the enclosed corners are detected from a snapshot
        printf("# corners to detect : %d ...\n",board_total);
        printf("begin corner detection...\n");
	while(successes < n_boards)
	{
		if((frame++ % frame_step) == 0)									//Skip frames
		{
			//Find chessboard corners:
			//int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
                        int found = cvFindChessboardCorners(gray_image, board_sz, corners, &corner_count,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
			//cvCvtColor(image, gray_image, CV_BGR2GRAY);										//Get Subpixel accuracy on those corners
			//cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11,11),cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
			//cvDrawChessboardCorners(gray_image, board_sz, corners, corner_count, found);			//Draw it
                        printf("\r corners %d collected",corner_count);
                        //sleep(2);
			// If we got a good board, add it to our data
			if( corner_count == board_total )
			{
				//cvShowImage( "Snapshot", image );										//show in color if we did collect the image
                                        tmp1 = 0;tmp2 = 0;tmp = 0;
					dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
					memcpy(dst,(uint8_t *)gray_image->imageData,BUFFER_SIZE);
					ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);
				step = successes*board_total;
				for( int i=step, j=0; j<board_total; ++i,++j ) {
				CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
				CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
				CV_MAT_ELEM(*object_points,float,i,0) = (float) j/board_w;
				CV_MAT_ELEM(*object_points,float,i,1) = (float) (j%board_w);
				CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;
			}
			CV_MAT_ELEM(*point_counts, int,successes,0) = board_total;
			successes++;
			printf("\r%d successful Snapshots out of %d collected.",successes,n_boards);
		}
		else
			//cvShowImage( "Snapshot", gray_image );										//Show Gray if we didn't collect the image
                {               tmp1 = 0;tmp2 = 0;tmp = 0;
					dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
					memcpy(dst,(uint8_t *)gray_image->imageData,BUFFER_SIZE);
					ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);
                }
	}


		//Handle pause/unpause and ESC
		/*int c = cvWaitKey(15);
		if(c == 'p')
		{
			c = 0;
			while(c != 'p' && c != 27)
			{
				c = cvWaitKey(250);
			}
		}
		if(c == 27)
			return 0;*/

		//image = cvQueryFrame( capture );								//Get next image
              tmp1 = 0;tmp2 = 0;tmp = 0;
              ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1); 
              gray_image->imageData = (char *) (dst_buffer);  
		//cvShowImage("Raw Video", image);
	}

	//End WHILE loop with enough successful captures

	//cvDestroyWindow("Snapshot");

	printf("\n\n *** Calbrating the camera now...\n");

	//Allocate matrices according to successful number of captures
	CvMat* object_points2  = cvCreateMat(successes*board_total,3,CV_32FC1);
	CvMat* image_points2   = cvCreateMat(successes*board_total,2,CV_32FC1);
	CvMat* point_counts2   = cvCreateMat(successes,1,CV_32SC1);

	//Tranfer the points to matrices
	for(int i = 0; i<successes*board_total; ++i)
	{
      CV_MAT_ELEM( *image_points2, float, i, 0)  =	CV_MAT_ELEM( *image_points, float, i, 0);
      CV_MAT_ELEM( *image_points2, float,i,1)    =	CV_MAT_ELEM( *image_points, float, i, 1);
      CV_MAT_ELEM(*object_points2, float, i, 0)  =  CV_MAT_ELEM( *object_points, float, i, 0) ;
      CV_MAT_ELEM( *object_points2, float, i, 1) =  CV_MAT_ELEM( *object_points, float, i, 1) ;
      CV_MAT_ELEM( *object_points2, float, i, 2) =  CV_MAT_ELEM( *object_points, float, i, 2) ;
	}

	for(int i=0; i<successes; ++i)
	{
		CV_MAT_ELEM( *point_counts2, int, i, 0) =  CV_MAT_ELEM( *point_counts, int, i, 0);			//These are all the same number
	}
	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);

	// Initialize the intrinsic matrix with both the two focal lengths in a ratio of 1.0

	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0f;

	//Calibrate the camera
	//_____________________________________________________________________________________

	cvCalibrateCamera2(object_points2, image_points2, point_counts2,  cvGetSize( gray_image ), intrinsic_matrix, distortion_coeffs, NULL, NULL,0 );
																																		//CV_CALIB_FIX_ASPECT_RATIO

	//_____________________________________________________________________________________

	//Save values to file
	printf(" *** Calibration Done!\n\n");
	printf("Storing Intrinsics.xml and Distortions.xml files...\n");
	cvSave("Intrinsics.xml",intrinsic_matrix);
	cvSave("Distortion.xml",distortion_coeffs);
	printf("Files saved.\n\n");

	printf("Starting corrected display....");

	//Sample: load the matrices from the file
	CvMat *intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
	CvMat *distortion = (CvMat*)cvLoad("Distortion.xml");

	// Build the undistort map used for all subsequent frames.

	IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	cvInitUndistortMap(intrinsic,distortion,mapx,mapy);

	// Run the camera to the screen, showing the raw and the undistorted image.

	//cvNamedWindow( "Undistort" );
	while(1)
	{
	      tmp1 = 0;tmp2 = 0;tmp = 0;
              ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSWRSNG,&tmp1); 
              gray_image->imageData = (char *) (dst_buffer);  
              IplImage *t = cvCloneImage(gray_image);
	      cvRemap( t, gray_image, mapx, mapy );			// Undistort image
	      cvReleaseImage(&t);
	      tmp1 = 0;tmp2 = 0;tmp = 0;
	      dst = (uint8_t *) (dst_buffer + BUFFER_SIZE);
	      memcpy(dst,(uint8_t *)gray_image->imageData,BUFFER_SIZE);
	      ioctl(deviceDriverFile,PLB_SDI_CTRL_IOCSRDSNG,&tmp);
	}

	return 0;
}


