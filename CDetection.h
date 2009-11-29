#include "stdafx.h"

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// FLTK
#include <FL/fl_ask.H>

//own
#include"BlinkDetection.h"

// others
#include <vector>
#include <numeric>

#include <iostream>
using namespace std;


#define CASC_FACE "haarcascade_face.xml" // fuer release stat der Klasse Blinker

class CDetection {

public:
	//friend typedef struct Blinker::BlinkerUser;

	CDetection();
	~CDetection();

	IplImage* detectVideo(IplImage*);
	CvRect* rFace;
	vector<CvRect*> rEyes;
	bool detectBlink(IplImage*);
	bool detectBlink(IplImage*, vector<CvRect*>);
	CvHistogram* createHist( const IplImage* );

private:
	CvHaarClassifierCascade* cascade_face;
	BlinkDetection * blinkDetector;
	bool loadHaarClassifier();
	CvRect* detectFace( IplImage* );
	vector<CvRect*> detectEyes( IplImage* );
	vector<CvRect*> detectEyes( IplImage*, CvRect* );
};

