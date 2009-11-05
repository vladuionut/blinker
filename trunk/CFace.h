#ifndef CFACE_H
#define CFACE_H

#include "stdafx.h"

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// FLTK
#include <FL/fl_ask.H>

// others
#include "CVector.h"

class CFace {

public:
	CFace();
	~CFace();

	IplImage* performDetection( IplImage* frame );

private:
	CvMemStorage* storage;
	CvHaarClassifierCascade* cascade_face;		
	CvHaarClassifierCascade* cascade_eye;	
	const char* cascade_face_name;
	const char* cascade_eye_name;

	CvRect* rFace;
	CvRect* rEyeRight;
	CvRect* rEyeLeft;
	CVector* prevDetection;

	bool loadHaarClassifier();
	IplImage* detectFace( IplImage* frame );
	IplImage* detectEyes( IplImage* frame );
};

#endif

