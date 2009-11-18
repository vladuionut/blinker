#include "stdafx.h"

#include "NearestDetection.h"
#include <sstream>
#include <stdexcept>


#include <iostream>
using namespace std;

NearestDetection::NearestDetection() {
	vFace		= vector<CvRect*>();
	vEyeRight	=  vector<CvRect*>();
	vEyeLeft	= vector<CvRect*>();
}

NearestDetection::~NearestDetection() {
	vFace.clear();
	vEyeRight.clear();
	vEyeLeft.clear();
}

void NearestDetection::add( CvRect* rect, vector<CvRect*>& v ) {
	v.insert( v.begin(), 1, rect ); // First-in, first-out
	if ( v.size() > REFERENCE_FRAMES )
		v.pop_back();	              // entfernt das letzte/aelteste rectangle
}

CvRect* NearestDetection::getNearestFace( CvSeq* seq ) {
	CvRect* rect = (CvRect*)0;
	CvRect* rHelp = (CvRect*)0;
	vector<CvRect*>::iterator iter = (vector<CvRect*>::iterator)vFace.begin();

	int mx = 0, my = 0;
	int min, pos, iHelp;

	// no object detected
	if ( seq->total == 0 ) {
		return rect;
	}
	
	// calculate mean
	while ( iter != vFace.end() ) {
		mx += (**iter).x / vFace.size();
		my += (**iter).y / vFace.size();
		++iter;
	}

	// get best match
	rHelp = (CvRect*)cvGetSeqElem( seq, 0 );
	min = (int) sqrt( pow((double)rHelp->x - mx,2) -  // minimale Differenz zum Durchschnittswert
					  pow((double)rHelp->y - my,2) ); // x-y, da immer querformat im video
	pos = 0;
	for ( int i = 1; i < seq->total; i++) {
		rHelp = (CvRect*)cvGetSeqElem( seq, i );
		iHelp = (int) sqrt( pow((double)rHelp->x - mx,2) - 
							pow((double)rHelp->y - my,2) );
		if ( iHelp < min ) {
			min = iHelp;
			pos = i;   // Position des Gesichtsbereichs mit dem aktuelles Min gewonnen
		}
	}

	rHelp = (CvRect*)cvGetSeqElem( seq, pos );
	if ( rHelp ) {
		rect = new CvRect( *rHelp );
		add( rect, vFace );
	}

	return rect;
}

vector<CvRect*> NearestDetection::getNearestEyes( CvSeq* seq, CvRect* face ) {
	CvRect* rEyeRight = 0;
	CvRect* rEyeLeft = 0;
	CvRect* rHelp = 0;
	vector<CvRect*> vOutput = (vector<CvRect*>)0;
	vector<CvRect*>::const_iterator iter = vEyeRight.begin();
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
	while ( iter != vEyeRight.end() ) {
		mxRight += mxRight / vEyeRight.size();
		myRight += myRight / vEyeRight.size();
		++iter;
	}
	iter = vEyeLeft.begin();
	while ( iter != vEyeLeft.end() ) {
		mxLeft += mxLeft / vEyeLeft.size();
		myLeft += myLeft / vEyeLeft.size();
		++iter;
	}

	// get best match
	middle = (int)(face->width / 2); // halbe weite
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
		add( rEyeRight, vEyeRight );
	}

	rHelp = (CvRect*)cvGetSeqElem( seq, posLeft );
	if ( rHelp ) {
		rEyeLeft = new CvRect( *rHelp );
		add( rEyeLeft, vEyeLeft );		
	}

	vOutput.push_back( rEyeRight );
	vOutput.push_back( rEyeLeft );

	return vOutput;
}
