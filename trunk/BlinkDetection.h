

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
	BlinkDetection( float _treshval = 0.6, 
					float _stateval1 = 0.85, 
					float _stateval2 = 0.55, 
					float _stateval3 = 0.45);
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
	bool flag_state[3];
	float stateval;
	float treshval;
	float stateval[3];
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

