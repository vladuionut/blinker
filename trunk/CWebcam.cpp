#include "stdafx.h"
#include "CWebcam.h"
#include <FL/fl_ask.H>


void CWebcam::captureFromCamera(int index) {
	capture = cvCaptureFromCAM(index );

	if(capture == NULL)
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
	IplImage* image = NULL;
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