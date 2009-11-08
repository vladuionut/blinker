
#include "stdafx.h"

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// FLTK
#include <FL/fl_ask.H>

// others
#include "NearestDetection.h"

#define CASC_FACE "haarcascade_face.xml" // fuer release stat der Klasse Blinker
#define CASC_EYE "haarcascade_eye.xml"

class Detection {

public:
	Detection();
	~Detection();

	/*
		Detektiert zunaecht den Gesichtsbereich, aufbauend auf diesem
		den Augenbereich in den übergebenden Videodaten.
		IplImage*: Zeiger auf Frames eines Videos.
	*/
	IplImage* detectVideo(IplImage*); // statt performDetection

protected:
	const char* cascade_face_name;
	const char* cascade_eye_name;

	CvRect* rFace;
	CvRect* rEyeRight;
	CvRect* rEyeLeft;
	NearestDetection* prevDetection;

private:
	CvMemStorage* storage;
	CvHaarClassifierCascade* cascade_face;		
	CvHaarClassifierCascade* cascade_eye;	

	bool loadHaarClassifier();
	IplImage* detectFace( IplImage* frame );
	IplImage* detectEyes( IplImage* frame );
};

