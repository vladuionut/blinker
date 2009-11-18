
#include "BlinkDetection.h"

BlinkDetection::BlinkDetection() {
	init = true;
	tmpL = (vector<IplImage*>)0;
	tmpR = (vector<IplImage*>)0;
	storage	= (CvMemStorage*)0;
}

//BlinkDetection::~BlinkDetection() {
//
//}

bool BlinkDetection::detect( IplImage* frame, vector<CvRect*> eyes ) {
	
	int width, height, x, y, help1, help2;

	if(storage)cvClearMemStorage( storage );
	storage = cvCreateMemStorage(0);

	if( !frame )
		return false;
	
	if(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) )
		return false;

	x = min(eyes.at(0)->x,eyes.at(1)->x);
	y = min(eyes.at(0)->y,eyes.at(1)->y);
	help1 =  eyes.at(0)->x + eyes.at(0)->width;
	help2 =	 eyes.at(1)->x + eyes.at(1)->width;
	width =	 max(help1,help2);
	width -= x;
	help1 =  eyes.at(0)->y + eyes.at(0)->height;
	help2 =	 eyes.at(1)->y + eyes.at(1)->height;
	height = max(help1, help2);
	height -=  y;
		

	IplImage *tmpArea = cvCreateImage( cvSize( width, height ), 
									   frame->depth, frame->nChannels ); 


	if(init) { // templates has to be initial

		cvRectangle( frame, 
						 cvPoint(x,y),
						 cvPoint(x+width,y+height),
						 CV_RGB(0,0,255), 3 );
	

		
		/*if( findTemplate( leftTmp, rightTmp ) )
			init = false;*/
	}else{ // template matching
		
	}

	return false;
}

bool BlinkDetection::findTemplate( IplImage * ltmp, IplImage * rtmp ) {
	return false;
}

int BlinkDetection::findEyeColor() {
	return -1;
}
