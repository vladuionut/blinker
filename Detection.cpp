#include "stdafx.h"
#include "Detection.h"

#include <iostream>
using namespace std;


Detection::Detection() {
	cascade_face		= (CvHaarClassifierCascade*)0;
	cascade_eye			= (CvHaarClassifierCascade*)0;
	storage				= (CvMemStorage*)0;
	prevDetection		=  new NearestDetection();
	rFace				=  new CvRect();
	rEyeRight			=  new CvRect();
	rEyeLeft			=  new CvRect();
	loadHaarClassifier();
}

Detection::~Detection() {
	delete prevDetection;
	if(storage)cvClearMemStorage( storage );
	delete rFace;
	delete rEyeLeft;
	delete rEyeRight;
	/*delete cascade_face;
	delete cascade_eye;*/
 
}

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

		rFace = prevDetection->getNearestFace( faces );

		if (rFace) {
			cvRectangle( frame, 
						 cvPoint(rFace->x,rFace->y),
						 cvPoint((rFace->x+rFace->width),(rFace->y+rFace->height)),
						 CV_RGB(255,0,0), 3 );  // blue, rectangle for face
		}

		if(storage)cvClearMemStorage( storage );
	}

	return frame;
}

IplImage* Detection::detectEyes( IplImage* frame ) {

	if (!rFace || !frame || !cascade_eye)
		return frame;

		vector<CvRect*> vEyes;
		CvRect* rect;
		if(storage)cvClearMemStorage( storage );
		cvSetImageROI( frame, cvRect(rFace->x, rFace->y+rFace->height/5, 
									 rFace->width, rFace->height/3) );
		CvSeq* eyes = cvHaarDetectObjects( frame, cascade_eye, storage, 1.2, 2, 
										   CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		
		for ( int i = 0; i < eyes->total; i++ ) {
			rect = (CvRect*)cvGetSeqElem( eyes, i );
			cvRectangle( frame, 
						 cvPoint(rect->x,rect->y),
						 cvPoint((rect->x+rect->width),(rect->y+rect->height)),
						 CV_RGB(0,255,0), 3 );
		}


		vEyes = prevDetection->getNearestEyes( eyes, rFace );

		
		rEyeRight = vEyes.at(0);
		rEyeLeft = vEyes.at(1);

		if ( rEyeRight ) {
			cvRectangle( frame, 
						 cvPoint(rEyeRight->x,rEyeRight->y),
						 cvPoint((rEyeRight->x+rEyeRight->width),(rEyeRight->y+rEyeRight->height)),
						 CV_RGB(255,0,0), 3 );
		}

		if ( rEyeLeft ) {
			cvRectangle( frame, 
						 cvPoint(rEyeLeft->x,rEyeLeft->y),
						 cvPoint((rEyeLeft->x+rEyeLeft->width),(rEyeLeft->y+rEyeLeft->height)),
						 CV_RGB(255,0,0), 3 );
		}


		if(storage)cvClearMemStorage( storage );
		if(frame)cvResetImageROI( frame );

		return frame;
}
