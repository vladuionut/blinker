#ifndef CWINDOW_H
#define CWINDOW_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H> 
#include <cv.h>
#include <highgui.h>

class CWindow : public Fl_Widget {

public:
	CWindow(int x, int y, int w, int h);
	void setImage(IplImage* nImage);
	int getX() {return _x;};
	int getY() {return _y;};
	int getW() {return _w;};
	int getH() {return _h;};
	~CWindow();

private:
	int _x, _y, _w, _h;
	IplImage* image;
	IplImage* dImage;

	void draw();
};

#endif