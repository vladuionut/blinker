
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
	
	if(storage)cvClearMemStorage( storage );
	storage = cvCreateMemStorage(0);

	if( !frame )
		return false;
	
	if(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) )
		return false;

	return false;
}

bool BlinkDetection::findTemplate( IplImage * ltmp, IplImage * rtmp ) {
	return false;
}

int BlinkDetection::findEyeColor() {
	return -1;
}
