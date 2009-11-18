#include "stdafx.h"
#include "Detection.h"

#include <iostream>
using namespace std;

bool Detection::detectBlink(IplImage* frame ) { 
	if(!frame)
		return false;

	return detectBlink( frame, 
					    detectEyes(frame) );
}

bool Detection::detectBlink(IplImage* frame, vector<CvRect*> eyes) {
	if(!frame|| eyes == (vector<CvRect*>)0 )
		return false;

	return blinkDetector->detect( frame, eyes );
}

Detection::Detection() {
	temp				= (vector<IplImage*>)0;
	cascade_face		= (CvHaarClassifierCascade*)0;
	cascade_eye			= (CvHaarClassifierCascade*)0;
	storage				= (CvMemStorage*)0;
	CvRect* rFace		= (CvRect*)0;
	vector<CvRect*> rEyes = (vector<CvRect*>)0;

	prevDetection		=  new NearestDetection();
	blinkDetector		= new BlinkDetection();
	loadHaarClassifier();
}

Detection::~Detection() {
	delete prevDetection;
	delete blinkDetector;

	if(storage)cvClearMemStorage( storage );
	delete rFace;
	if(rEyes != (vector<CvRect*>)0)rEyes.empty();
	if(temp != (vector<IplImage*>)0)temp.empty();
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
	if(storage)cvClearMemStorage( storage );

	if( !frame )
		return frame;

	CvRect* face = detectFace( frame );
	if (face) {
		cvRectangle( frame, 
					 cvPoint(face->x,face->y),
					 cvPoint((face->x+face->width),(face->y+face->height)),
					 CV_RGB(255,0,0), 3 );  // blue, rectangle for face

		rFace = face;
	} else {
		rFace = (CvRect*)0;
	}
	
	if(storage)cvClearMemStorage( storage );

	vector<CvRect*> eyes = detectEyes( frame, face );
	if(eyes != (vector<CvRect*>)0) {
		if ( eyes.at(0) ) {
			cvRectangle( frame, 
						 cvPoint(eyes.at(0)->x,eyes.at(0)->y),
						 cvPoint((eyes.at(0)->x+eyes.at(0)->width),(eyes.at(0)->y+eyes.at(0)->height)),
						 CV_RGB(0,255,0), 3 );
		}

		if (  eyes.at(1) ) {
			cvRectangle( frame, 
						 cvPoint(eyes.at(1)->x,eyes.at(1)->y),
						 cvPoint((eyes.at(1)->x+eyes.at(1)->width),(eyes.at(1)->y+eyes.at(1)->height)),
						 CV_RGB(0,255,0), 3 );
		}

		rEyes = eyes;
	}else {
		rEyes = (vector<CvRect*>)0;
	}

	if(storage)cvClearMemStorage( storage );

	return frame;
}

CvRect* Detection::detectFace( IplImage* frame ) {
	CvRect* rFace = (CvRect*)0;
	CvRect* rect = (CvRect*)0;
	CvSeq* faces = (CvSeq*)0;

	if(frame && cascade_face) {
		if(storage)cvClearMemStorage( storage );

		faces = cvHaarDetectObjects( frame, cascade_face, storage, 1.2, 2, 
									 CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

		rFace = prevDetection->getNearestFace( faces );
	}

	return rFace;
}

vector<CvRect*> Detection::detectEyes( IplImage* frame, CvRect* rFace ) {

	vector<CvRect*> vEyes = (vector<CvRect*>)0;

	if ( !frame || !cascade_eye || !rFace)
		return vEyes;
		
	if(storage)cvClearMemStorage( storage );
	cvSetImageROI( frame, cvRect(rFace->x, rFace->y+rFace->height/5, 
								 rFace->width, rFace->height/3) );
	CvSeq* eyes = cvHaarDetectObjects( frame, cascade_eye, storage, 1.2, 2, 
									   CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

	// MATEJ dieser Teil gehoert ueberarbeitet
	if(eyes->total > 2) {
		vEyes = vector<CvRect*>();
		/*for ( int i = 0; i < eyes->total; i++ )
			vEyes.insert(vEyes.end(), (CvRect*)cvGetSeqElem( eyes, i ) );*/
		vEyes = prevDetection->getNearestEyes( eyes, rFace ); //????????????????
	}

	if(frame)cvResetImageROI( frame );

	return vEyes;
}

vector<CvRect*> Detection::detectEyes( IplImage* frame ) {
	if ( !frame )
		return (vector<CvRect*>)0;

	return detectEyes( frame, detectFace(frame) );
}
