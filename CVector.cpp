#include "stdafx.h"

#include "CVector.h"

#include <iostream>
using namespace std;

CVector::CVector() {
	vFace		= new vector<CvRect>;
	vEyeRight	= new vector<CvRect>;
	vEyeLeft	= new vector<CvRect>;
}

CVector::~CVector() {
	delete vFace;
	delete vEyeRight;
	delete vEyeLeft;
}

void CVector::add( CvRect& rect, vector<CvRect>* v ) {
	v->insert( v->begin(), 1, rect );
	if ( v->size() > REFERENCE_FRAMES )
		v->pop_back();	
}

CvRect* CVector::getNearestFace( CvSeq* seq ) {
	CvRect* rect = 0;
	CvRect* rHelp = 0;
	vector<CvRect>::iterator iter;
	int mx = 0, my = 0;
	int min, pos, iHelp;

	// no object detected
	if ( seq->total == 0 ) {
		return rect;
	}
	
	// calculate mean
	iter = vFace->begin();
	while ( iter != vFace->end() ) {
		mx += iter->x / vFace->size();
		my += iter->y / vFace->size();
		++iter;
	}

	// get best match
	rHelp = (CvRect*)cvGetSeqElem( seq, 0 );
	min = (int) sqrt( pow((double)rHelp->x - mx,2) - pow((double)rHelp->y - my,2) );
	pos = 0;
	for ( int i = 1; i < seq->total; i++) {
		rHelp = (CvRect*)cvGetSeqElem( seq, i );
		iHelp = (int) sqrt( pow((double)rHelp->x - mx,2) - pow((double)rHelp->y - my,2) );
		if ( iHelp < min ) {
			min = iHelp;
			pos = i;
		}
	}

	rHelp = (CvRect*)cvGetSeqElem( seq, pos );
	if ( rHelp ) {
		rect = new CvRect( *rHelp );
		add( *rect, vFace );
	}

	return rect;
}

vector<CvRect*> CVector::getNearesEyes( CvSeq* seq, CvRect* face ) {
	CvRect* rEyeRight = 0;
	CvRect* rEyeLeft = 0;
	CvRect* rHelp = 0;
	vector<CvRect*> vOutput;
	vector<CvRect>::iterator iter;
	int mxRight = 0, myRight = 0, mxLeft = 0, myLeft = 0;
	int iHelp;
	int minRight, minLeft, posRight, posLeft;
	int middle;

	// no object detected
	if ( seq->total == 0 ) {
		vOutput.push_back( rEyeRight );
		vOutput.push_back( rEyeLeft );
		return vOutput;
	}

	// calculate mean
	iter = vEyeRight->begin();
	while ( iter != vEyeRight->end() ) {
		mxRight += mxRight / vEyeRight->size();
		myRight += myRight / vEyeRight->size();
		++iter;
	}
	iter = vEyeLeft->begin();
	while ( iter != vEyeLeft->end() ) {
		mxLeft += mxLeft / vEyeLeft->size();
		myLeft += myLeft / vEyeLeft->size();
		++iter;
	}

	// get best match
	middle = (int)(face->width / 2);
	minRight = face->width;
	minLeft = face->width;
	posLeft = 0;
	posRight = 0;
	for ( int i = 1; i < seq->total; i++) {
		rHelp = (CvRect*)cvGetSeqElem( seq, i );
		if ( rHelp->x >= middle ) {	// right eye
			iHelp = (int) sqrt( pow((double)rHelp->x - mxRight,2) - pow((double)rHelp->y - myRight,2) );
			if ( iHelp < minRight ) {
				minRight = iHelp;
				posRight = i;
			}
		}
		else {						// left eye
			iHelp = (int) sqrt( pow((double)rHelp->x - mxLeft,2) - pow((double)rHelp->y - myLeft,2) );
			if ( iHelp < minLeft ) {
				minLeft = iHelp;
				posLeft = i;
			}
		}
	}

	rHelp = (CvRect*)cvGetSeqElem( seq, posRight );
	if ( rHelp ) {
		rEyeRight = new CvRect( *rHelp );
		add( *rEyeRight, vEyeRight );
	}

	rHelp = (CvRect*)cvGetSeqElem( seq, posLeft );
	if ( rHelp ) {
		rEyeLeft = new CvRect( *rHelp );
		add( *rEyeLeft, vEyeLeft );		
	}

	cout << middle << " " << rEyeRight->x << " " << rEyeLeft->x << endl;

	vOutput.push_back( rEyeRight );
	vOutput.push_back( rEyeLeft );

	return vOutput;
}

CvRect* CVector::getNearest( vector<CvRect*>* candidates ) {
	CvRect* rect = 0;
	//int mx = 0, my = 0, mw = 0, mh = 0;
	//int help, min, pos;
	//vector<int> distance;
	//
	//// no objects in frame detected
	//if ( candidates->empty() ) {
	//	return rect;
	//}


	//// no reference detections
	///*if ( v->empty() ) {
	//	for ( unsigned int i = 0; i < candidates->size(); i++ ) {
	//		add( candidates->at(i) );
	//	}
	//}*/

	//// only one object detected
	//if ( candidates->size() == 1 ) {
	//	rect = candidates->at( 0 );
	//	add( candidates->at(0) );
	//	return rect;
	//}

	//// calculate mean position
	//for ( unsigned int i = 0; i < v->size(); i++ ) {
	//	mx += v->at(i)->x;
	//	my += v->at(i)->y;
	//	mw += v->at(i)->width;
	//	mh += v->at(i)->height;
	//}

	//mx /= v->size();
	//my /= v->size();
	//mw /= v->size();
	//mh /= v->size();

	//// get nearest neighbour
	//for ( unsigned int i = 0; i < candidates->size(); i++ ) {
	//	help = (int) sqrt( pow((double)candidates->at(i)->x - mx,2) - pow((double)candidates->at(i)->y - my,2) );
	//	distance.push_back( help );
	//}

	//min = distance.at(0);
	//pos = 0;
	//for ( unsigned int i = 0; i < distance.size(); i++) {
	//	if ( distance.at(i) < min ) {
	//		min = distance.at(i);
	//		pos = i;
	//	}
	//}

	//rect = candidates->at(pos);
	//add( rect );

	return rect;
}