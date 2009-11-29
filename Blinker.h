#include "stdafx.h"

#include <iostream>
using namespace std;
/************************************************************************
   Technische Univeristaet Wien - 
   188.469 Visual Analysis of Human Motion (LU 1,0) 2009W

   Project: Real Time Eye Tracking and Blink Detection with USB Cameras
   Team: Matej Nezveda
		 Agnieszka Wojdecka
************************************************************************/

#include <string>
#include <algorithm>
#include <sstream>
#include <stdexcept>

// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// include
#include "CWindow.h"
#include "CWebcam.h"
#include "CDetection.h"

// FLTK
#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

/************************************************************
* Diese Klasse ermöglicht das Extrahieren von Kopf- und 
* Augenbereich in einem Video zum Detektieren eines Augen-
* blinzen. Wird ein Blinzeln erkannt, wird ein Aktion - in
* diesem Fall das Schiessen eines Fotos - ausgelöst.
* Für die Anwendung ist der Anschluss an eine Webcam notwendig.
*
* version: 0.1
* autor: Agnieszka Wojdecka
* e-mail: a0501682@unet.univie.ac.at
**************************************************************/

class Blinker:public Fl_Double_Window{
	public:

		/*
			Constructor. Defaultwerte fuer Breite, 700, Hoehe, 400, Titel "Blinker".
		*/
		Blinker(int, int, const char*);

		/*
			Destructor.
		*/
		~Blinker();

		/*
			Diese Methode erzeugt die Fensterapplikation.  
		*/
		void creatWin();

	protected:

	private:				
		/*
			Uebersteigt der Inhalt die Dimensionen des Fensters,
			wird das ordnungsgerechte Betrachten des Inhalts ueber
			Scroll-Balken ermoeglich.
		*/
		Fl_Scroll* scroll;

		Fl_Scroll* scroll_snapshot;

		/*
			Button, dessen Druecken einen Verbindungsaufbau zu einer
			bestehenden Webcam ausloest.
		*/
		Fl_Button* cam_opener;

		/*
			Container fuer die Wiedergabe des Videomaterials. 
			Ist eingebettet in scroll.
		*/
		CWindow* win_frame;

		CWindow* win_snapshot;

		/*
			Speichert bei Wiedergabe des Videos Bild fuer Bild.
		*/
		IplImage* img_frame;

		IplImage* img_snapshot;


		/*
			Ermoeglicht den Verbindugnsaufbau und das Zugreifen auf Daten einer Webcam.
		*/
		CWebcam cap;

		/*
			Dieses Flag wird auf true gesetzt, wenn eine Verbindung 
			zu einer Webcam aufgebaut werden konnte, und ueber diese
			Frames ausgespielt werden koennen.
		*/
		bool flag_play;

		CDetection* detection;

		/*
			Bei bestehendem Verbindungsaufbau zur Webcam und Ausspielen von Videodaten wird ueber 
			die Callbackmethode openCam_CB Videomaterial im Container scroll ausgegeben.
		*/
		void openCam();

		/*
			Callbackmethode fuer den Verbindungsaufbau zur Webcam und Ausspielen der Videodaten 
			wird von openCam aufgerufen. Knopft cam_opener wird deaktiviert.
		*/
		void openCam_CB();

		/*
			Statische Methode, die den Aufruf fuer einen Timeout Callback 
			der Callback-Methode play_CB ermoeglicht.
		*/
		static void play_CB_stat(void*);

		/*
			Wird alle 0.01 Sekunden aufgerufen um ein neues Bild des Videostroms
			zu holen und auszugeben.
		*/
		void play_CB();

		typedef struct BlinkerUser{
				IplImage* img;
				CvHistogram* hist;
				BlinkerUser( IplImage* i = (IplImage*)0, CvHistogram* h = (CvHistogram*)0 ) {
					int hdims = 16; /* TODO: make that static in CDetection  */
					float hranges_arr[2] = {0,180};
				    float * hranges = hranges_arr;
					
					img = cvCreateImage( cvGetSize(i), i->depth, i->nChannels );
					cvCopyImage(i,img);
					hist = cvCreateHist( 1, &hdims, 
											CV_HIST_ARRAY, 
											&hranges, 1 );
					cvCopyHist(h,&hist);
				}
				/*~BlinkerUser() {
					if(img)cvReleaseImage(&img);
					if(hist)cvReleaseHist(&hist);
				}*/
		};
		vector<BlinkerUser> user;
		int isNewUser( IplImage* );
};