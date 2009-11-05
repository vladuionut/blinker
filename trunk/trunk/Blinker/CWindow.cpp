#include "stdafx.h"
#include "CWindow.h"
#include <FL/fl_ask.H>
#include <iostream>
using namespace std; 

CWindow::CWindow(int x, int y, int w, int h) : Fl_Widget(x, y, w, h, NULL) {
	_x = x;
	_y = y;
	_w = w;
	_h = h;

	image = NULL;
	dImage = NULL;
}

void CWindow::setImage(IplImage* nImage) {
	if (image)
		cvReleaseImage(&image);

	image = nImage;

	draw();
}

void CWindow::draw() {
	//draw( Fl_Widget::x(), Fl_Widget::y(), getW(), getH() );

	if ( !image )
		return;

	float fx, fy, scale;
	int nWidth, nHeight;

	fl_push_clip( getX(), getY(), getW(), getH() );
	fx = (float) image->width / getW();
	fy = (float) image->height / getH();

	if ( fx > fy )
		scale = fx;
	else
		scale = fy;

	if ( scale > 1 ) {
		if ( dImage )
			cvReleaseImage( &dImage );

		nWidth	= cvFloor(image->width/scale);
		nHeight = cvFloor(image->height/scale);
		dImage  = cvCreateImage( cvSize(nWidth,nHeight), image->depth, image->nChannels );
		dImage->widthStep = dImage->width * dImage->nChannels;
		cvResize( image, dImage, CV_INTER_LINEAR );
		fl_draw_image( (uchar *)dImage->imageData, getX(), getY(), dImage->width, dImage->height, dImage->nChannels, 0 );
	}
	else
		fl_draw_image( (uchar *)image->imageData, getX(), getY(), image->width, image->height, image->nChannels , 0 );
	fl_pop_clip();
}
