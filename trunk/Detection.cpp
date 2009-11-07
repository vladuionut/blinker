#include "stdafx.h"
#include "Detection.h"

#include <iostream>
using namespace std;


Detection::Detection() {
	cascade_face		= (CvHaarClassifierCascade*)0;
	cascade_eye			= (CvHaarClassifierCascade*)0;
	storage				= (CvMemStorage*)0;
	loadHaarClassifier();
}

//Detection::~Detection() {
//}

bool Detection::loadHaarClassifier() {
	if(cascade_face != ((CvHaarClassifierCascade*) 0) )
		cvReleaseHaarClassifierCascade(&cascade_face);
	if(cascade_eye != ((CvHaarClassifierCascade*) 0))
		cvReleaseHaarClassifierCascade(&cascade_eye);

	cascade_face = (CvHaarClassifierCascade*)cvLoad( CASC_FACE, 0, 0, 0 );
	cascade_eye = (CvHaarClassifierCascade*)cvLoad( CASC_EYE, 0, 0, 0 );

	if ( !cascade_face  ) {
		fl_alert( "Error: Could not load cascade face classifier!" );
		return false;
	}

	if ( !cascade_eye ) {
		fl_alert( "Error: Could not load cascade eye classifier!" );
		return false;
	}

	storage = cvCreateMemStorage(0);
	return true;
}

IplImage* Detection::detectVideo(IplImage* frame) {
	return detectEyes( detectFace( frame ) );
}

IplImage* Detection::detectFace( IplImage* frame ) {
	CvRect* rect = (CvRect*)0;
	CvSeq* faces = (CvSeq*)0;

	if(frame && cascade_face) {
		if(storage)cvClearMemStorage( storage );

		faces = cvHaarDetectObjects( frame, cascade_face, storage, 1.2, 2, 
									 CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		for ( int i = 0; i < (faces ? faces->total : 0); i++) { //so lange ein videostrom besteht
			rect = (CvRect*)cvGetSeqElem( faces, i );
			cvRectangle( frame, 
						 cvPoint(rect->x,rect->y),
						 cvPoint((rect->x+rect->width),
								 (rect->y+rect->height)),
						 CV_RGB(255,0,0), 3 ); // blue, rectangle for face

			cvRectangle( frame, 
						 cvPoint(rect->x,rect->y+rect->height/5),
						 cvPoint((rect->x+rect->width),
								 (rect->y+rect->height/2)),
						 CV_RGB(0,255,0), 3 ); // green, rectangle for eyearea
		}
		if(storage)cvClearMemStorage( storage );
	}

	return frame;
}

IplImage* Detection::detectEyes( IplImage* frame ) {

	CvRect* rect = (CvRect*)0;
	CvSeq* eyes = (CvSeq*)0;

	if(frame && cascade_eye) {
		if(storage)cvClearMemStorage( storage );

		eyes = cvHaarDetectObjects( frame, cascade_eye, storage, 1.2, 2, 
									 CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		for ( int i = 0; i < (eyes ? eyes->total : 0); i++) { //so lange ein videostrom besteht
			rect = (CvRect*)cvGetSeqElem( eyes, i );
			cvRectangle( frame, 
						 cvPoint(rect->x,rect->y),
						 cvPoint((rect->x+rect->width),
								 (rect->y+rect->height)),
						 CV_RGB(0,0,255), 3 ); // red, rectangle for eye
		}
		if(storage)cvClearMemStorage( storage );
	}

	return frame;
}



