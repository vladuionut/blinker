
// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// others
#include <vector>
#include <numeric>

using namespace std;

class NearestDetection {

public:
	NearestDetection();
	~NearestDetection();
	
	CvRect* getNearestFace( CvSeq* seq );
	vector<CvRect*> getNearestEyes( CvSeq* seq, CvRect* face );

private:
	static const int REFERENCE_FRAMES = 20;
	vector<CvRect*> vFace;
	vector<CvRect*> vEyeRight;
	vector<CvRect*> vEyeLeft;

	void add( CvRect* , vector<CvRect*>& );

};
