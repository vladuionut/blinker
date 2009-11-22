

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
			tmp = t;
			hist = h;
			r = _r;
		}
	};

	bool flag_init;
	bool flag_prev;
	bool flag_match;
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

