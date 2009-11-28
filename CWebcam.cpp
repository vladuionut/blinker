#include "stdafx.h"
#include "CWebcam.h"
#include <FL/fl_ask.H>


CWebcam::CWebcam(){
	capture = (CvCapture*)0;
	frame = (IplImage*)0;
}

void CWebcam::captureFromCamera(int index) {
	capture = cvCaptureFromCAM(index );

	if(capture == (CvCapture*)0)
        fl_alert("Webcam not opened");
}

void CWebcam::queryFrame() {
	if (frame)
		cvReleaseImage(&frame);

	frame = cvQueryFrame(capture);

	if (!frame)
		fl_alert("Frame not retrieved");
}

int CWebcam::grabFrame() {
	return cvGrabFrame(capture);
}

void CWebcam::retrieveFrame() {
	frame = cvRetrieveFrame(capture);
	if(!frame)
		fl_alert("Frame not Retrieved");

}

IplImage* CWebcam::getFrame() {
	IplImage* image = (IplImage*)0;
	grabFrame();
	retrieveFrame();

	// color image
	if(frame) {
		if (frame->nChannels == 3) {
			int origin = frame->origin;
			CvMat* mat, stub;
			mat = cvGetMat( frame, &stub );
			cvConvertImage( mat, frame, CV_CVTIMG_SWAP_RB );
			image = cvCreateImage( cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3 );
			image->widthStep = image->width * 3;

			if (origin == IPL_ORIGIN_TL)
				cvCopy( frame, image, 0);
			else
				cvFlip( frame, image, 0);
		}
	}

	return image;
}