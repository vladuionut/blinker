
#include "BlinkDetection.h"


BlinkDetection::BlinkDetection( float _treshval ) {
	startTime = 0;
	curTime = 0;
	treshval = _treshval;
	flag_match = true;
	flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
	templ_img = vector<IplImage*>();
	templ_r = vector<CvRect*>();
	m_frame = 0;
	numb_blink = 0;
}

BlinkDetection::~BlinkDetection() {
	if(!templ_img.empty())templ_img.clear();
	if(!templ_r.empty())templ_r.clear();
	if(m_frame)cvReleaseImage(&m_frame);
}

bool BlinkDetection::detect( IplImage* frame, vector<CvRect*> eyes ) {
	
	vector<IplImage*> cur_img = vector<IplImage*>();
	vector<CvRect*> cur_r = vector<CvRect*>();
	IplImage* leftTmp = 0;
	IplImage* rightTmp = 0;
	CvRect* size = 0;
	int x, y, width, height;
	/*CvMat* mat = 0;
	CvMat stub;*/

	curTime = time(0);

	if( !frame ||
		(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) ) ) {
		startTime = time(0);
		curTime = time(0);
		flag_match = false;
		flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
		numb_blink = 0;
		return false;
	}

	/*if(m_frame)cvReleaseImage(&m_frame);
	m_frame = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);
	mat = cvGetMat( frame, &stub); 
	cvConvertImage( mat, m_frame, CV_CVTIMG_SWAP_RB );*/

	if(templ_img.empty() || templ_r.empty()) { // init template
		createTemplate(frame,eyes.at(0),eyes.at(1));
	}else{
		if(flag_match) { // start prev/cur matching, start timer
			startTime = time(0);
			curTime = time(0);
			flag_match = false;
			flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
			numb_blink = 0;
		}
		size = new CvRect();
		cur_r.insert(cur_r.end(), eyes.at(0)); 
		cur_r.insert(cur_r.end(),  eyes.at(1));
		cur_r.insert(cur_r.begin(), size );
			 
		leftTmp = cvCreateImage( cvSize(cur_r.at(1)->width,cur_r.at(1)->height), frame->depth, frame->nChannels );
		rightTmp = cvCreateImage( cvSize(cur_r.at(2)->width,cur_r.at(2)->height), frame->depth, frame->nChannels );

		cvSetImageROI( frame, *cur_r.at(1) );
		cvCopy(frame, leftTmp);
		cvResetImageROI(frame);
		cur_img.insert( cur_img.end(), leftTmp );
		
		cvSetImageROI(  frame, *cur_r.at(2) );
		cvCopyImage(frame, rightTmp);
		cvResetImageROI(frame);
		cur_img.insert( cur_img.end(),rightTmp  );

		x = min(cur_r.at(1)->x, cur_r.at(2)->x);
		y = min(cur_r.at(1)->y, cur_r.at(2)->y);
		width = (cur_r.at(1)->width + cur_r.at(2)->width)/2;
		height = (cur_r.at(1)->height + cur_r.at(2)->height)/2;
		// estimator of eye
		x += width/4;
		y += height/4;	
		width = width*0.7;
		height = height*0.7;
		cur_r.at(0)->x = x;
		cur_r.at(0)->y = y;
		cur_r.at(0)->width = width;
		cur_r.at(0)->height = height;

		if( match( cur_img, cur_r ) ) {
			if( difftime(curTime, startTime) > 2 ) {
				if( numb_blink > 0 ) {
					flag_match = true;
					return true;
				}
				flag_match = true; // loop frame by frame for 2 sec
			}
			return false;
		}
		else 
			flag_match = true;
	}

	// release
	if(size) delete size;
	if(!cur_img.empty())cur_img.clear();
	if(!cur_r.empty())cur_r.clear();
	if(leftTmp)cvReleaseImage(&leftTmp);
	if(rightTmp)cvReleaseImage(&rightTmp);

	return false;
}

void BlinkDetection::createTemplate(IplImage* frame,CvRect* leftEye,CvRect* rightEye) {
	if( frame && leftEye && rightEye ) {
		if(!templ_img.empty())templ_img.clear();
		if(!templ_r.empty())templ_r.clear();

		int x, y, width, height;
		CvRect* size  = 0;
		IplImage* leftTmp = 0;
		IplImage* rightTmp = 0;


		x = min(leftEye->x, rightEye->x);
		y = min(leftEye->y, rightEye->y);
		width = (leftEye->width + rightEye->width)/2;
		height = (leftEye->height + rightEye->height)/2;
		// fit template window to eye
		x += width/4;
		y += height/4;			/* maybe there is a better way to do this, morph, opening, comp, hist??*/
		width = width*0.7;
		height = height*0.7;
		size = new CvRect();
		size->x = x;
		size->y = y;
		size->width = width;
		size->height = height;

		templ_r.insert(templ_r.end(), leftEye); 
		templ_r.insert(templ_r.end(), rightEye);
		templ_r.insert(templ_r.begin(), size);

		leftTmp = cvCreateImage( cvSize(width,height), frame->depth, frame->nChannels );
		rightTmp = cvCreateImage( cvSize(width,height), frame->depth, frame->nChannels );

		cvSetImageROI( frame, *templ_r.at(0) );
		cvCopyImage(frame, leftTmp, NULL );
		cvResetImageROI(frame);
		templ_img.insert( templ_img.end(), cvCloneImage(leftTmp) );

		cvSetImageROI(  frame, *templ_r.at(0) );
		cvCopyImage(frame, rightTmp, NULL);
		cvResetImageROI(frame);
		templ_img.insert( templ_img.end(), cvCloneImage(rightTmp) );

		if(leftTmp)cvReleaseImage(&leftTmp);
		if(rightTmp)cvReleaseImage(&rightTmp);
	}
}

bool BlinkDetection::match( vector<IplImage*> cur_img, vector<CvRect*> cur_r ) {

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	IplImage* tm_img = 0;
	double maxval;
	CvPoint maxloc;
	CvMat* mat = 0;
	CvMat stub;

	if( cur_img.empty() && cur_r.empty() )
		return false;

	// size of matching window
	width = cur_img.at(0)->width - templ_img.at(0)->width + 1;
	height = cur_img.at(0)->height - templ_img.at(0)->height + 1;
	tm_img = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);

	// template matching
	cvMatchTemplate(cur_img.at(0), templ_img.at(0), tm_img, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(tm_img, 0, &maxval, 0, &maxloc, 0);
	
	if( maxval < 0.3 ) {			   // lost eye position
		return false;
		cout << difftime(curTime, startTime) << " lost eye " << maxval << endl;
	}
	if( maxval > 0.3 && maxval < 0.6 ) {// closed eye
		flag_ncc[1] = true;
				cout << difftime(curTime, startTime) << " closed eye " << maxval << endl;
	}
	if( maxval > 0.8 )	{		   // open eye
		if(flag_ncc[1]) {
			flag_ncc[2] = true; cout << "2 ";
		}
		else{
			flag_ncc[0] = true; cout << "1 ";
		}
						cout << difftime(curTime, startTime) << " open eye " << maxval << endl;
	}


	if( flag_ncc[0] && flag_ncc[1] && flag_ncc[2] ) {
		++numb_blink;
		flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
		cout << difftime(curTime, startTime) << " blinked " << numb_blink << endl;
	}

	return true;
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
