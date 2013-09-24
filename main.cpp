#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "saveData.h"
#include "utils.h"

#include "elas.cpp"

using namespace cv;
using namespace std;
  

int main( int argc, char** argv )
{
  
  //timer
  struct timeval start, end;
  long mtime, seconds, useconds; 
  
  //load image
  Mat image;
  image = imread( argv[1], 1 );
  Mat image2 = imread (argv[2],1);
  
  if( argc != 3 || !image.data || !image2.data ){
      printf( "2 images must be provided \n" );
      return -1;
  }
    
  int rows=image.rows, cols=image.cols;
  Mat gray_image,gray_image_right,dispL,dispR;
  cvtColor( image, gray_image, CV_BGR2GRAY );
  cvtColor( image2, gray_image_right, CV_BGR2GRAY );
  

  
  unsigned char *IM1 = (unsigned char *)(gray_image.data);
  
  uint8_t* D1=(uint8_t*)malloc(rows*cols*sizeof(uint8_t));
  uint8_t* D2=(uint8_t*)malloc(rows*cols*sizeof(uint8_t));

  Elas::parameters setting;
  setting.disparity_max=1;
  Elas myelas(IM1,gray_image_right.data,D1,D2,rows,cols);
  myelas.process();
  
  IplImage* stacked = cvCreateImage( cvSize( cols,rows*2), IPL_DEPTH_8U, 1 ); 
  IplImage* stackedc = cvCreateImage( cvSize( cols,rows*2), IPL_DEPTH_8U, 3 ); 
  IplImage* iplLeft=new IplImage(gray_image);
  IplImage* iplRight=new IplImage(gray_image_right);
  cvZero( stacked ); 
   
  cvSetImageROI( stacked, cvRect( 0, 0, cols, rows ) );
  cvCopy(iplLeft, stacked);//, stacked, NULL );
  cvResetImageROI(stacked);
  cvSetImageROI( stacked, cvRect(0,rows, cols, rows) );
  cvCopy(iplRight, stacked);//, stacked, NULL ); 
  cvResetImageROI(stacked); 
 
  
  Mat imgStacked(stacked);      
  Mat imgStackedC(stackedc);   

  cvtColor(imgStacked, imgStackedC, CV_GRAY2RGB);
  IplImage* iplStacked=new IplImage(imgStackedC);
  
  int i=140;
    for(int j=0;j<cols;j++){
      if(*(D1+i*cols+j)<255 && *(D1+i*cols+j)>0)
      cvLine(iplStacked, cvPoint(j,i), cvPoint(j-*(D1+i*cols+j),i+rows), cvScalar((rand() % 256),(rand() % 256),(rand() % 256),0));
    }
  
    
  Mat imgStackedCShow(iplStacked);
//   namedWindow( "Matching", CV_WINDOW_AUTOSIZE );
//   imshow( "Matching", imgStackedCShow );
  
  gray_image.data=D1;
  gray_image_right.data=D2;

        
  namedWindow( "Disparity Left", CV_WINDOW_AUTOSIZE );
  imshow( "Disparity Left", gray_image );
  
  
//    namedWindow( "Disparity Right", CV_WINDOW_AUTOSIZE );
//   imshow( "Disparity Right", gray_image_right );

  IplImage* saveimage=new IplImage(gray_image);
  cvSaveImage("data/disparity_ipol.png",saveimage);
//   saveimage=new IplImage(gray_image_right);
//   cvSaveImage("data/disparityR.png",saveimage);
  waitKey(0);

  free(D1);
  free(D2);
  image.release();
  image2.release();
  gray_image.release();
  gray_image_right.release();

  
  return 0;
}