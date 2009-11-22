

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//other
#include <vector>
#include <time.h>


#include <iostream>
using namespace std;

class BlinkDetection {

public:
	BlinkDetection(float _treshval = 0.6);
	~BlinkDetection();
	
	bool detect( IplImage*, vector<CvRect*> );

protected:
	typedef struct BlinkTemplate {
		IplImage* tmp;
		CvHistogram* hist;
		CvRect* r;
		BlinkTemplate( IplImage* t = (IplImage*)0, CvHistogram* h = (CvHistogram*)0, CvRect* _r = (CvRect*)0) {
			if( t )tmp = (IplImage* )cvClone(t); 
			else tmp = (IplImage*)0;
			if( h )hist = (CvHistogram* )cvClone(h);
			else hist = (CvHistogram*)0;
			if( _r )r = (CvRect* )cvClone(_r);
			else r = (CvRect*)0;
		}
	};

	bool flage_init;
	bool flage_prev;
	float treshval;
	IplImage* temp_prev;
	BlinkTemplate tmpL;
	BlinkTemplate tmpR;

	IplImage* m_frame;

	CvMemStorage* storage;

private:
	time_t startTime;
	bool createTemplate(IplImage*,IplImage*);
	bool is_eye_open(IplImage*,IplImage*);
	CvHistogram* createHist(IplImage*);
	bool match(IplImage*,vector<CvRect*>);

};

