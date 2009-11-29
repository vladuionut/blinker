
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
	blinksec = 0;
	count_lost = 0;
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

	curTime = time(0);

	if( !frame ||
		(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) ) ) {
		startTime = 0;
		flag_match = false;
		flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
		blinksec = 0;
		return false;
	}

	if(count_lost > 15) {
		count_lost = 0;
		templ_img.clear();
		templ_r.clear();
	}
	
	if(templ_img.empty() || templ_r.empty()) { // init template
		createTemplate(frame,eyes.at(0),eyes.at(1));
	}else{
		if(flag_match) { // start prev/cur matching, start timer
			startTime = 0;
			flag_match = false;
			flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
			blinksec = 0;
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
			if( startTime != 0 && blinksec > 2 ) {
				flag_match = true;
				return true;
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
	double maxval, leftMaxval,rightMaxval;
	CvPoint leftMaxloc,rightMaxloc;
	CvMat* mat = 0;
	CvMat stub;

	if( cur_img.empty() && cur_r.empty() )
		return false;

	/* left eye */
	// size of matching window
	width = cur_img.at(0)->width - templ_img.at(0)->width + 1;
	height = cur_img.at(0)->height - templ_img.at(0)->height + 1;
	tm_img = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);

	// template matching
	cvMatchTemplate(cur_img.at(0), templ_img.at(0), tm_img, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(tm_img, 0, &leftMaxval, 0, &leftMaxloc, 0);
	
	/* right eye */
	// size of matching window
	if(tm_img)cvReleaseImage(&tm_img);
	width = cur_img.at(1)->width - templ_img.at(1)->width + 1;
	height = cur_img.at(1)->height - templ_img.at(1)->height + 1;
	tm_img = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);

	// template matching
	cvMatchTemplate(cur_img.at(1), templ_img.at(1), tm_img, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(tm_img, 0, &rightMaxval, 0, &rightMaxloc, 0);

	maxval = (leftMaxval + rightMaxval) / 2;

	if( maxval < 0.45 ) {			   // lost eye position
		return false;
	}
	else if( maxval >= 0.45 && maxval < 0.6 ) {// closed eye
	  --count_lost;
		if(startTime == 0)
			startTime = time(0);
		flag_ncc[1] = true;
	}
	else if( maxval > 0.8 )	{		   // open eye
	  --count_lost;
		if(flag_ncc[1]) {
			flag_ncc[2] = true;
			this->blinksec = difftime(curTime,startTime);
			startTime = 0;
		}
		else{
	      --count_lost;
			flag_ncc[0] = true;
			startTime = 0;
		}
	} else {
		--count_lost; // something in between
		startTime = 0;
	}

	if( flag_ncc[0] && flag_ncc[1] && flag_ncc[2] ) {
		flag_ncc[0] = (flag_ncc[1]= (flag_ncc[2]= false ));
	}

	return true;
}

