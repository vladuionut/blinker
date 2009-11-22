
#include "BlinkDetection.h"

BlinkDetection::BlinkDetection(float _treshval) {
	flag_init = true;
	flag_prev = true;
	flag_match = true;
	tmpL = (BlinkTemplate)0;
	tmpR = (BlinkTemplate)0;
	storage	= (CvMemStorage*)0;
	temp_prev = (IplImage*)0;
	treshval = _treshval;
	startTime = 0;
}

BlinkDetection::~BlinkDetection() {
	if(temp_prev)cvReleaseImage( &temp_prev );
	if(storage)cvClearMemStorage( storage );

}

bool BlinkDetection::match(IplImage* frame, vector<CvRect*> eyes) {
	flag_match = false;
	CvHistogram * hist1 = (CvHistogram *)0;
	CvHistogram * hist2 = (CvHistogram *)0;
	IplImage * gray_frame = (IplImage *)0;
	IplImage * gray_tmpl = (IplImage *)0;
	CvPoint* point = (CvPoint*)0;
	float comp1 = 0.f;
	float comp2 = 0.f;

	cvSetImageROI(frame,*eyes.at(0));
	hist1 = createHist( frame );
	cvResetImageROI(frame);
	cvSetImageROI(frame,*eyes.at(1));
	hist2 = createHist(frame);
	cvResetImageROI(frame);

	comp1 = 1 - cvCompareHist(hist1,tmpL.hist,CV_COMP_BHATTACHARYYA);
	comp2 = 1 - cvCompareHist(hist2,tmpR.hist,CV_COMP_BHATTACHARYYA);

	if(comp1 > treshval && comp2 > treshval) { // eyes for matching?
		// TODO: diff of pic -tmp
		// TODO: measurment of clock, return false so lange bis blink 2 sec
		
		// motion analyse
		gray_frame = cvCreateImage(cvGetSize(frame),8,1);
		cvConvertImage(frame,gray_frame,CV_BGR2GRAY);

		// left eye
		gray_tmpl = cvCreateImage(cvGetSize(tmpL.tmp),8,1);
		cvConvertImage(tmpL.tmp,gray_tmpl,CV_BGR2GRAY);

		point = &cvPoint( eyes.at(0)->x + eyes.at(0)->width/2,
						  eyes.at(0)->y + eyes.at(0)->height/2 ); /* centroids */
		
		cvSetImageROI( gray_frame, cvRect( point->x + tmpL.tmp->width/2, 
									       point->y + tmpL.tmp->height/2, 
									       tmpL.tmp->width,
									       tmpL.tmp->height ) );


		// cvSub(gray_frame, gray_tmpl, diff, NULL); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// right eye
		gray_tmpl = cvCreateImage(cvGetSize(tmpR.tmp),8,1);
		cvConvertImage(tmpR.tmp,gray_tmpl,CV_BGR2GRAY);


		point = &cvPoint( eyes.at(1)->x + eyes.at(1)->width/2,
						  eyes.at(1)->y + eyes.at(1)->height/2 );  /* centroids */

		cvResetImageROI(gray_frame);
		cvSetImageROI( gray_frame, cvRect( point->x + tmpR.tmp->width/2, 
									       point->y + tmpR.tmp->height/2, 
									       tmpR.tmp->width,
									       tmpR.tmp->height ) );

		cvResetImageROI(gray_frame);

		if(difftime(startTime,time(0)) > 2) {
			flag_match = true;
			startTime = 0;
			return true;
		}
	
	}else{
		flag_match = true;
		startTime = 0;
	}

	if(hist1)cvReleaseHist(&hist1);
	if(hist2)cvReleaseHist(&hist2);

	return false;
}

bool BlinkDetection::detect( IplImage* frame, vector<CvRect*> eyes ) {
	
	m_frame = frame; // loeschen???????????????????????????????????????????

	int width, height, x, y, help1, help2;
	IplImage *curTmpArea = (IplImage*)0;
	IplImage *prevTmpArea = (IplImage*)0;

	if(storage)cvClearMemStorage( storage );
	storage = cvCreateMemStorage(0);

	if( !frame )
		return false;
	
	if(  eyes == (vector<CvRect*>)0 || !eyes.at(0) || !eyes.at(1) )
		return false;

	x = min(eyes.at(0)->x,eyes.at(1)->x);
	y = min(eyes.at(0)->y,eyes.at(1)->y);
	help1 =  eyes.at(0)->x + eyes.at(0)->width;
	help2 =	 eyes.at(1)->x + eyes.at(1)->width;
	width =	 max(help1,help2);
	width -= x;
	help1 =  eyes.at(0)->y + eyes.at(0)->height;
	help2 =	 eyes.at(1)->y + eyes.at(1)->height;
	height = max(help1, help2);
	height -=  y;
		
	curTmpArea = cvCreateImage( cvSize( width, height ), 
									 frame->depth, frame->nChannels ); 

	// loeschen !!!!!!!!!!!!!!!!!
	cvRectangle( frame, 
				 cvPoint(x,y),
				 cvPoint((x+width),(y+height)),
				 CV_RGB(0,0,255), 3 );

	prevTmpArea = cvCreateImage( cvSize( width, height ), 
								 frame->depth, frame->nChannels ); 

	if(flag_init) { // templates has to be initial
		// area of eyes from current frame
		cvSetImageROI(frame, cvRect( x, y, width, height ));
		cvCopy(frame, curTmpArea);
		cvResetImageROI(frame);

		if(flag_prev) {
			temp_prev = (IplImage*)cvClone(frame);
			flag_prev = false;
		} else {
			// area of eyes from previous frame
			cvSetImageROI(temp_prev, cvRect( x, y, width, height ));
			cvCopy(temp_prev, prevTmpArea);
			cvResetImageROI(temp_prev);

			if( createTemplate( curTmpArea, prevTmpArea ) )
				flag_init = false;
			else
				flag_prev = true;
		}
	}else{ 
		if(flag_match) // template matching
			startTime = time(0);
		
		return match(frame,eyes);
	}

	if(curTmpArea)cvReleaseImage(&curTmpArea);
	if(prevTmpArea)cvReleaseImage(&prevTmpArea);

	if(storage)cvClearMemStorage( storage );

	return false;
}

bool BlinkDetection::createTemplate( IplImage * curTmpArea, IplImage * prevTmpArea ) {

	if( !curTmpArea || !prevTmpArea )
		return false;

	IplConvKernel* kernel = (IplConvKernel*)0;
	CvSeq* comp = (CvSeq*)0;
	int numComp = 0;
	IplImage *eyeComp1 = (IplImage *)0; 
	IplImage *eyeComp2 = (IplImage *)0; 

	IplImage *diff = cvCreateImage( cvGetSize(curTmpArea), 8, 1);
	IplImage *curGray = cvCreateImage( cvGetSize(curTmpArea), 8, 1);
	IplImage *prevGray = cvCreateImage( cvGetSize(curTmpArea), 8, 1 );

	cvConvertImage(curTmpArea, curGray, CV_BGR2GRAY);
	cvConvertImage(prevTmpArea, prevGray, CV_BGR2GRAY);

	// motion analysis
	cvSub(curGray, prevGray, diff, NULL);
	cvThreshold(diff, diff, 5, 255, CV_THRESH_BINARY);

	// noise reduction
	kernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
	cvMorphologyEx(diff, diff, NULL, kernel, CV_MOP_OPEN, 1);

	numComp = cvFindContours(   diff,                   /* the difference image */
								storage,                /* created with cvCreateMemStorage() */
								&comp,                  /* output: connected components */
								sizeof(CvContour),
								CV_RETR_CCOMP,
								CV_CHAIN_APPROX_SIMPLE,
								cvPoint(0,0)
								);

	// find eye pair
	if( comp != 0 && numComp == 2) 
	{
		CvRect* r1 = &cvBoundingRect(comp, 1);
		comp = comp->h_next;
		if(comp == 0)
			return false;
		CvRect* r2 = &cvBoundingRect(comp, 1);

		// width of eyes are about the same
		if( abs(r1->width - r2->width) < (r1->width + r2->width)/(2*5) )
			return false;

		// height of eyes are about the same
		if( abs(r1->height - r2->height) < (r1->height + r2->height)/(2*5) )
			return false;

		// vertical distance
		if( abs(r1->y - r2->y) < (r1->height + r2->height)/4 )
			return false;

		// horizontal distance
		if( abs(r1->x - r2->x) < (r1->width + r2->width) )
			return false;

		// loeschen !!!!!!!!!!!!!!!!!
		cvRectangle( m_frame, 
				 cvPoint(r1->x,r1->y),
				 cvPoint((r1->x+r1->width),(r1->y+r1->height)),
				 CV_RGB(0,100,155), 3 );
		cvRectangle( m_frame, 
				 cvPoint(r2->x,r2->y),
				 cvPoint((r2->x+r2->width),(r2->y+r2->height)),
				 CV_RGB(0,100,155), 3 );

		// RGB eye area for histogramm detection 
		eyeComp1 = cvCreateImage( cvSize(r1->width, r1->height), curTmpArea->depth, curTmpArea->nChannels);
	    eyeComp2 = cvCreateImage( cvSize(r2->width, r2->height),  curTmpArea->depth, curTmpArea->nChannels);

		cvSetImageROI(curTmpArea, *r1);
		cvCopy(curTmpArea, eyeComp1);
		cvResetImageROI(curTmpArea);
		cvSetImageROI(curTmpArea, *r2);
		cvCopy(curTmpArea, eyeComp2);
		cvResetImageROI(curTmpArea);


		if( is_eye_open( eyeComp1, eyeComp2 ) ) 
		{
			tmpL.tmp = (IplImage*)cvClone(eyeComp1);
			tmpL.hist = (CvHistogram*)cvClone(createHist(eyeComp1));
			tmpL.r = (CvRect*)cvClone(r1);
			tmpR.tmp = (IplImage*)cvClone(eyeComp2);
			tmpR.hist = (CvHistogram*)cvClone(createHist(eyeComp2));
			tmpR.r = (CvRect*)cvClone(r2);
	
			return true;
		}

	}

	if(storage)cvClearMemStorage( storage );

	if(eyeComp1)cvReleaseImage(&eyeComp1);
	if(eyeComp2)cvReleaseImage(&eyeComp2);
	
	return false; // open eye pair was not found
}

bool BlinkDetection::is_eye_open( IplImage *eyeComp1, IplImage *eyeComp2 ){

	float max_val1 = 0.f;
	float max_val2 = 0.f;
	CvHistogram* hist1 = (CvHistogram*)0;
	CvHistogram* hist2 = (CvHistogram*)0;
	IplImage* pup1 = (IplImage*)0;
	IplImage* pup2 = (IplImage*)0;

	// centroid
	float x = eyeComp1->width /2;
	float y = eyeComp1->height /2;
	// dimension
	float a = (eyeComp1->width*0.5 + eyeComp1->height*0.5) /2;
	
	// area of pupils1 ???????????????????????????????????????????????????????????
	pup1 = cvCreateImage(cvSize(a,a), eyeComp1->depth,eyeComp1->nChannels);
	cvSetImageROI(eyeComp1, cvRect( x - a/2, y - a/2, a, a ));
	cvCopyImage(eyeComp1, pup1);
	cvResetImageROI(eyeComp1);

	// centroid
	x = eyeComp2->width /2;
	y = eyeComp2->height /2;
	// dimension
	a = (eyeComp2->width*0.5 + eyeComp2->height*0.5) /2;

	// area of pupils ???????????????????????????????????????????????????????????
	pup2 = cvCreateImage(cvSize(a,a), eyeComp2->depth,eyeComp2->nChannels);
	cvSetImageROI(eyeComp2, cvRect( x - a/2, y - a/2, a, a ));
	cvCopyImage(eyeComp2, pup1);
	cvResetImageROI(eyeComp2);

	hist1 = createHist( pup1 );
	hist2 = createHist( pup2 );

	if( hist1 )
		cvGetMinMaxHistValue( hist1, 0, &max_val1, 0, 0 );
	if( hist2 )
		cvGetMinMaxHistValue( hist2, 0, &max_val2, 0, 0 );

	cout<< max_val1 << "  " << max_val2 << endl;
	
	// TODO: RGB, red, green, brown, black

	if(hist1)cvReleaseHist(&hist1);
	if(hist2)cvReleaseHist(&hist2);
	if(pup1)cvReleaseImage(&pup1);
	if(pup2)cvReleaseImage(&pup2);

	return false;
}

CvHistogram* BlinkDetection::createHist( IplImage* img ) {

	int hdims = 16;
	float hranges_arr[] = {0,180};
	float * hranges = hranges_arr;

	CvHistogram* hist = (CvHistogram*)0;
	IplImage *hue = (IplImage *)0;
	IplImage *sat = (IplImage *)0;
	IplImage *val = (IplImage *)0;
	//IplImage *backproject = (IplImage *)0;
	

	if(img) {
		// convert from BGR to HSV
		cvCvtColor(img, img, CV_BGR2HSV);

		hue = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		sat = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		val = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

		cvCvtPixToPlane( img, hue, sat, val, 0 );
		cvCalcHist( &hue, hist );
		cvNormalizeHist( hist, 1000 );
		
		// release
		if(hist)cvReleaseHist(&hist);
		if(hue)cvReleaseImage(&hue);
		if(sat)cvReleaseImage(&sat);
		if(val)cvReleaseImage(&val);
	}

	return hist;
}

CvScalar hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}
