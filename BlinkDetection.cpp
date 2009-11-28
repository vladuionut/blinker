
#include "BlinkDetection.h"


BlinkDetection::BlinkDetection( float _treshval ) {
	startTime = 0;
	curTime = 0;
	treshval = _treshval;
	flag_match = true;
	templ_img = vector<IplImage*>();
	templ_r = vector<CvRect*>();
	ncc = vector<float>();
	m_frame = 0;
}

BlinkDetection::~BlinkDetection() {
	if(!templ_img.empty())templ_img.clear();
	if(!templ_r.empty())templ_r.clear();
	if(!ncc.empty())ncc.clear();
	if(m_frame)cvReleaseImage(&m_frame);
}

bool BlinkDetection::detect( IplImage* frame, vector<CvRect*> eyes ) {
	
	vector<IplImage*> cur_img = vector<IplImage*>();
	vector<CvRect*> cur_r = vector<CvRect*>();
	/*CvMat* mat = 0;
	CvMat stub;*/

	if( !frame )
		return false;
	
	if(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) )
		return false;

	/*if(m_frame)cvReleaseImage(&m_frame);
	m_frame = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	mat = cvGetMat( frame, &stub); 
	cvConvertImage( mat, m_frame, CV_CVTIMG_SWAP_RB );*/

	

	if(templ_img.empty() || templ_r.empty()) { // init template
		createTemplate(frame,eyes.at(0),eyes.at(1));
	}else{
		if(flag_match) { // start prev/cur matching, start timer
			startTime = time(0);
			flag_match = false;
			if(ncc.empty())ncc.clear();
		}
		cur_r.insert(cur_r.end(), eyes.at(0)); 
		cur_r.insert(cur_r.end(),  eyes.at(1));
			 
		cvSetImageROI(frame, *cur_r.at(0));
		cur_img.insert( cur_img.end(), cvCloneImage(frame) );
		cvResetImageROI(frame);
		cvSetImageROI(frame, *cur_r.at(1));
		cur_img.insert( cur_img.end(), cvCloneImage(frame) );
		cvResetImageROI(frame);

		if( match( cur_img, cur_r ) ) {
			if( difftime(startTime, curTime) > 2 )
				flag_match = true; // loop frame by frame for 2 sec
			return true;
		}
		else
			flag_match = true;
	}

	// release
	if(!cur_img.empty())cur_img.clear();
	if(!cur_r.empty())cur_r.clear();

	return false;
}

void BlinkDetection::createTemplate(IplImage* frame,CvRect* leftEye,CvRect* rightEye) {
	if( frame && leftEye && rightEye ) {
		if(!templ_img.empty())templ_img.clear();
		if(!templ_r.empty())templ_r.clear();

		IplImage* tmp = 0;

		templ_r.insert(templ_r.end(), leftEye); 
		templ_r.insert(templ_r.end(), rightEye);
			 
		tmp = cvCreateImage( cvSize( leftEye->width, leftEye->height ),
									 frame->depth, frame->nChannels );
		cvSetImageROI(frame, *templ_r.at(0));
		templ_img.insert( templ_img.end(), cvCloneImage(frame) );
		cvResetImageROI(frame);
		cvSetImageROI(frame, *templ_r.at(1));
		templ_img.insert( templ_img.end(), cvCloneImage(frame) );
		cvResetImageROI(frame);
	}
}

bool BlinkDetection::match( vector<IplImage*> cur_img, vector<CvRect*> cur_r ) {

	IplImage* tm = 0;
	curTime = time(0);

	if( cur_img.empty() && cur_r.empty() ) {

		// eye matching
		//cvMatchTemplate(cur_img, tpl, tm, CV_);

		/* TODO: 
			- template matching
			- min/max in ncc vector schreiben
			- blinken nach 2 sec auswerten?
		*/


	}
	return false;
}


CvHistogram* BlinkDetection::createHist( const IplImage* img ) {

	int hdims = 16;
	float hranges_arr[] = {0,180};
	float * hranges = hranges_arr;

	CvHistogram* hist = 0;
	IplImage *hue = 0;
	IplImage *sat = 0;
	IplImage *val = 0;
	IplImage *_img = 0;

	if(img) {
		_img = cvCloneImage(img);

		// convert from BGR to HSV
		cvCvtColor(_img, _img, CV_BGR2HSV);

		hue = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		sat = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		val = cvCreateImage(cvGetSize(_img), IPL_DEPTH_8U, 1);
		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

		cvCvtPixToPlane( _img, hue, sat, val, 0 );
		cvCalcHist( &hue, hist );
		cvNormalizeHist( hist, 1000 );
		
		// release
		if(hue)cvReleaseImage(&hue);
		if(sat)cvReleaseImage(&sat);
		if(val)cvReleaseImage(&val);
		if(_img)cvReleaseImage(&_img);
	}

	return hist;
}	
