
#include "stdafx.h"

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// FLTK
#include <FL/fl_ask.H>

// others
#include "NearestDetection.h"
#include "BlinkDetection.h"

#define CASC_FACE "haarcascade_face.xml" // fuer release stat der Klasse Blinker
#define CASC_EYE "haarcascade_eye.xml"

class Detection {

public:
	CvRect* rFace;
	vector<CvRect*> rEyes;

	Detection();
	~Detection();

	/*
		Detektiert zunaecht den Gesichtsbereich, aufbauend auf diesem
		den Augenbereich in den übergebenden Videodaten.
		IplImage*: Zeiger auf Frames eines Videos.
	*/
	IplImage* detectVideo(IplImage*); // statt performDetection
	bool detectBlink(IplImage*);
	bool detectBlink(IplImage*, vector<CvRect*>);

protected:
	const char* cascade_face_name;
	const char* cascade_eye_name;

	NearestDetection* prevDetection;
	BlinkDetection* blinkDetector;

private:
	CvMemStorage* storage;
	CvHaarClassifierCascade* cascade_face;		
	CvHaarClassifierCascade* cascade_eye;	

	bool loadHaarClassifier();
	CvRect* detectFace( IplImage* );
	vector<CvRect*> detectEyes( IplImage* );
	vector<CvRect*> detectEyes( IplImage*, CvRect* );
};

