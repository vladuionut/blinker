

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//other
#include <vector>
#include <ctime>


#include <iostream>
using namespace std;

class BlinkDetection {

public:
	BlinkDetection( float _treshval = 0.6 );
	~BlinkDetection();
	
	bool detect( IplImage*, vector<CvRect*> );

protected:
	IplImage* m_frame;
	vector<IplImage*> templ_img;
	vector<CvRect*> templ_r;
	float treshval;
	int blinksec;
	int count_lost;
		
	CvHistogram* createHist( const IplImage* );
	void createTemplate(IplImage*,CvRect*,CvRect*);
	bool match(vector<IplImage*>,vector<CvRect*>);
	

private:
	bool flag_match;
	bool flag_ncc[3];
	time_t startTime;
	time_t curTime;
};

