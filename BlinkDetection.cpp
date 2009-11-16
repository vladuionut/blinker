
#include "BlinkDetection.h"

BlinkDetection::BlinkDetection() {

}

//BlinkDetection::~BlinkDetection() {
//
//}

bool BlinkDetection::detect( IplImage* frame, vector<CvRect*> eyes ) {
	
	IplImage *gray, *lgray, *rGray;

	gray = cvCreateImage( cvSize(frame->width,frame->height), 8, 1 );
	cvCvtColor( frame, gray, CV_BGR2GRAY );

	return false;
}

bool BlinkDetection::creatTemplate() {
	return false;
}