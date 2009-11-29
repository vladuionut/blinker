#include "stdafx.h"
#include "CDetection.h"

#include <iostream>
using namespace std;

CDetection::CDetection() {
	cascade_face		= (CvHaarClassifierCascade*)0;
	blinkDetector = new BlinkDetection();
	rFace = (CvRect*)0;
	rEyes = (vector<CvRect*>)0;
	loadHaarClassifier();
}

CDetection::~CDetection() {
	if (cascade_face)
		cvReleaseHaarClassifierCascade(&cascade_face);
	if(blinkDetector)
		delete blinkDetector;
}

bool CDetection::detectBlink(IplImage* frame ) { 
	if(!frame)
		return false;

	return detectBlink( frame, 
					    detectEyes(frame) );
}

bool CDetection::detectBlink(IplImage* frame, vector<CvRect*> eyes) {
	if(eyes == (vector<CvRect*>)0)
		eyes = detectEyes(frame);
	if(!frame )
		return false;

	return blinkDetector->detect( frame, eyes );
}

IplImage* CDetection::detectVideo(IplImage* frame) {
	CvRect* face			= (CvRect*)0;
	vector<CvRect*> eyes	= (vector<CvRect*>)0;
	
	if( !frame )
		return frame;

	face = detectFace( frame );
	if (face) {
		cvRectangle( frame, 
					 cvPoint(face->x,face->y),
					 cvPoint((face->x+face->width),(face->y+face->height)),
					 CV_RGB(255,0,0), 3 );  // blue, rectangle for face

		rFace = face;
	} else {
		rFace = (CvRect*)0;
	}

	eyes = detectEyes( frame, face );

	if(eyes != (vector<CvRect*>)0) {
		if ( eyes.at(0) ) {
			/*cvRectangle( frame, 
						 cvPoint(eyes.at(0)->x,eyes.at(0)->y),
						 cvPoint((eyes.at(0)->x+eyes.at(0)->width),(eyes.at(0)->y+eyes.at(0)->height)),
						 CV_RGB(0,255,0), 3 );*/
		}

		if (  eyes.at(1) ) {
			/*cvRectangle( frame, 
						 cvPoint(eyes.at(1)->x,eyes.at(1)->y),
						 cvPoint((eyes.at(1)->x+eyes.at(1)->width),(eyes.at(1)->y+eyes.at(1)->height)),
						 CV_RGB(0,255,0), 3 );*/
		}

		rEyes = eyes;
	}else {
		rEyes = (vector<CvRect*>)0;
	}

	return frame;
}

vector<CvRect*> CDetection::detectEyes( IplImage* frame, CvRect* rFace ) {

	vector<CvRect*> vEyes = (vector<CvRect*>)0;
	CvRect* lRect = (CvRect*)0;
	CvRect* rRect = (CvRect*)0;
	float x, y, width, height;

	if ( !frame || !rFace)
		return (vector<CvRect*>)0;

	// left eye
	x = rFace->x + rFace->width*0.125;
	y = rFace->y + rFace->height*0.25;
	width = (rFace->x + rFace->width*0.45) - x;
	height = (rFace->y + rFace->height*0.5) - y;
	
	lRect = new CvRect();
	lRect->x = x;
	lRect->y = y;
	lRect->width = width;
	lRect->height = height;

	// right eye
	x = rFace->x + rFace->width*0.55;
	y = rFace->y + rFace->height*0.25;
	width = (rFace->x + rFace->width*0.875) - x;
	height = (rFace->y + rFace->height*0.5) - y;

	rRect = new CvRect();
	rRect->x = x;
	rRect->y = y;
	rRect->width = width;
	rRect->height = height;

	vEyes = vector<CvRect*>();
	vEyes.insert(vEyes.begin(),rRect);
	vEyes.insert(vEyes.begin(),lRect);

	return vEyes;
}

vector<CvRect*> CDetection::detectEyes( IplImage* frame ) {
	if ( !frame )
		return (vector<CvRect*>)0;

	return detectEyes( frame, detectFace(frame) );
}

bool CDetection::loadHaarClassifier() {
	if (cascade_face)
		cvReleaseHaarClassifierCascade( &cascade_face );

	cascade_face = (CvHaarClassifierCascade*) cvLoad( CASC_FACE, 0, 0, 0);

	if ( !cascade_face  ) {
		fl_alert( "Error: Could not load cascade face classifier!" );
		return false;
	}

	return true;
}

CvRect* CDetection::detectFace( IplImage* frame ) {
	CvRect* rFace			= (CvRect*)0;
	CvRect* rect			= (CvRect*)0;
	CvSeq* faces			= (CvSeq*)0;
	CvMemStorage* storage	= (CvMemStorage*)0; 
	IplImage* img_help		= (IplImage*)0;
	int pos					= 0;
	int size				= 0;
	int help				= 0;
	int scale_factor		= 3;	

	if(frame && cascade_face) {
		if(storage)
			cvClearMemStorage( storage );
		storage = cvCreateMemStorage(0);

		// speed up detection by performing on smaller image
		img_help = cvCreateImage( cvSize(frame->width/scale_factor,frame->height/scale_factor), IPL_DEPTH_8U, 3 );
		cvResize( frame, img_help, CV_INTER_LINEAR );

		faces = cvHaarDetectObjects( img_help, cascade_face, storage, 1.2, 2, 
									 CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		// faces in frame detected
		if ( faces->total != 0 ) {

			// select largest rectangle
			for ( int i = 0; i < (faces ? faces->total : 0); i++) {
				rect = (CvRect*)cvGetSeqElem( faces, i );
				help = 2*rect->height + 2*rect->width;
				if ( help > size ) {
					pos = i;
					size = help;
				}
			}
			rFace = (CvRect*)cvGetSeqElem( faces, pos );

			// resize rectangle
			rFace->height *= scale_factor;
			rFace->width  *= scale_factor;
			rFace->x	  *= scale_factor;
			rFace->y	  *= scale_factor;
		}
	}	

	if (storage)
		cvClearMemStorage( storage );

	if (img_help)
		cvReleaseImage(&img_help);

	return rFace;
}

CvHistogram* CDetection::createHist( const IplImage* img ) {

	int hdims = 16;
	float hranges_arr[] = {0,180};
	float * hranges = hranges_arr;

	CvHistogram* hist = 0;
	IplImage *hue = 0;
	IplImage *sat = 0;
	IplImage *val = 0;
	IplImage *_img = 0;

	if(img) {
		_img = cvCreateImage( cvGetSize( img ), img->depth, img->nChannels );
		cvCopyImage(img,_img);

		// convert from BGR to HSV
		cvCvtColor(_img, _img, CV_BGR2HSV);

		hue = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		sat = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		val = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

		cvCvtPixToPlane( _img, hue, sat, val, 0 );
		cvCalcHist( &hue, hist );
		cvNormalizeHist( hist, 1000 );
		
		// release
		if(hue)cvReleaseImage(&hue);
		if(sat)cvReleaseImage(&sat);
		if(val)cvReleaseImage(&val);
		if(_img)cvReleaseImage(&_img);
	}

	return hist;
}	
