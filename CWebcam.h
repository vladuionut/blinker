#ifndef CWEBCAM_H
#define CWEBCAM_H

#include <highgui.h>

class CWebcam {

public:
	void captureFromCamera(int index);
	IplImage* getFrame();
	CWebcam();

private:
	CvCapture* capture;
	IplImage* frame;

	void queryFrame();
	int grabFrame();
	void retrieveFrame();
};


#endif