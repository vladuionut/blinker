#include "stdafx.h"
#include "CFace.h"

#include <iostream>
using namespace std;

CFace::CFace() {
	cascade_face		= 0;
	cascade_eye			= 0;
	storage				= 0;
	cascade_face_name	= "F:\\Studium\\9.Semester WS 09\\Visual Analysis of Human Motion\\Übung\\Blinker\\Blinker\\haarcascade_face.xml";
	cascade_eye_name	= "F:\\Studium\\9.Semester WS 09\\Visual Analysis of Human Motion\\Übung\\Blinker\\Blinker\\haarcascade_eye.xml";
	rFace				= 0;
	rEyeRight			= 0;
	rEyeLeft			= 0;
	prevDetection		= new CVector;
	loadHaarClassifier();
}

CFace::~CFace() {
	delete rFace;
	delete prevDetection;
}

IplImage* CFace::performDetection( IplImage* frame ) {
	detectFace( frame );
	detectEyes( frame );


	return frame;
}

IplImage* CFace::detectFace( IplImage* frame ) {
	IplImage* temp		= cvCreateImage( cvSize(frame->width,frame->height), 8, 3 );
	storage				= cvCreateMemStorage(0);
	cvClearMemStorage( storage );
	CvSeq* faces		= cvHaarDetectObjects( frame, cascade_face, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

	rFace = prevDetection->getNearestFace( faces );
	
	if ( rFace ) {
		cvRectangle( frame, 
					 cvPoint(rFace->x,rFace->y),
					 cvPoint((rFace->x+rFace->width),(rFace->y+rFace->height)),
					 CV_RGB(255,0,0), 3 );
	}

	cvReleaseImage( &temp );
	cvClearMemStorage( storage );	

	return frame;
}

IplImage* CFace::detectEyes( IplImage* frame ) {
	if ( !rFace )
		return frame;

	vector<CvRect*> vEyes;

	CvRect* rect;
	IplImage* temp = cvCreateImage( cvSize(frame->width,frame->height), 8, 3 );
	storage = cvCreateMemStorage(0);
	cvClearMemStorage( storage );
	cvSetImageROI( frame, cvRect(rFace->x, rFace->y+rFace->height/5, rFace->width, rFace->height/3) );
	CvSeq* eyes = cvHaarDetectObjects( frame, cascade_eye, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

	for ( int i = 0; i < eyes->total; i++ ) {
		rect = (CvRect*)cvGetSeqElem( eyes, i );
		cvRectangle( frame, 
					 cvPoint(rect->x,rect->y),
					 cvPoint((rect->x+rect->width),(rect->y+rect->height)),
					 CV_RGB(0,255,0), 3 );
	}


	vEyes = prevDetection->getNearesEyes( eyes, rFace );
	rEyeRight = vEyes.at(0);
	rEyeLeft = vEyes.at(1);

	if ( rEyeRight ) {
		cvRectangle( frame, 
					 cvPoint(rEyeRight->x,rEyeRight->y),
					 cvPoint((rEyeRight->x+rEyeRight->width),(rEyeRight->y+rEyeRight->height)),
					 CV_RGB(255,0,0), 3 );
	}
	else 
		cout << "right" << endl;

	if ( rEyeLeft ) {
		cvRectangle( frame, 
					 cvPoint(rEyeLeft->x,rEyeLeft->y),
					 cvPoint((rEyeLeft->x+rEyeLeft->width),(rEyeLeft->y+rEyeLeft->height)),
					 CV_RGB(255,0,0), 3 );
	}
	else
		cout << "left" << endl;

	cvReleaseMemStorage( &storage );
	cvReleaseImage( &temp );
	cvResetImageROI( frame );

	return frame;
}

bool CFace::loadHaarClassifier() {
	if (cascade_face)
		cvReleaseHaarClassifierCascade( &cascade_face );
	if (cascade_eye)
		cvReleaseHaarClassifierCascade( &cascade_eye );

	cascade_face = (CvHaarClassifierCascade*) cvLoad( cascade_face_name, 0, 0, 0);
	cascade_eye  = (CvHaarClassifierCascade*) cvLoad( cascade_eye_name, 0, 0, 0);

	if ( !cascade_face  ) {
		fl_alert( "Error: Could not load cascade face classifier!" );
		return false;
	}

	if ( !cascade_eye ) {
		fl_alert( "Error: Could not load cascade eye classifier!" );
		return false;
	}

	return true;
}

