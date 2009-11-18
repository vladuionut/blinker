

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//other
#include <vector>

#include <iostream>
using namespace std;

class BlinkDetection {

public:
	BlinkDetection();
	//~BlinkDetection();
	
	bool detect( IplImage*, vector<CvRect*> );

protected:
	bool init;
	vector<IplImage*> tmpL;
	vector<IplImage*> tmpR;
	CvMemStorage* storage;

private:
	bool findTemplate(IplImage*, IplImage*);
	int findEyeColor();


};

