

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
	bool creatTemplate();

protected:
private:

};

