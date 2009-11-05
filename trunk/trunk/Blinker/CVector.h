#ifndef CVECTOR_H
#define CVECTOR_H

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// others
#include <vector>
#include <numeric>

using namespace std;

class CVector {

public:
	CVector();
	~CVector();
	
	CvRect* getNearest( vector<CvRect*>* candidates );
	CvRect* getNearestFace( CvSeq* seq );
	vector<CvRect*> getNearesEyes( CvSeq* seq, CvRect* face );

private:
	static const int REFERENCE_FRAMES = 20;
	vector<CvRect> *vFace;
	vector<CvRect> *vEyeRight;
	vector<CvRect> *vEyeLeft;

	void add( CvRect& rect, vector<CvRect>* v );

};

#endif