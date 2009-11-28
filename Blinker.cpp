/************************************************************************
   Technische Univeristaet Wien - 
   188.469 Visual Analysis of Human Motion (LU 1,0) 2009W

   Project: Real Time Eye Tracking and Blink Detection with USB Cameras
   Team: Matej Nezveda
		 Agnieszka Wojdecka
************************************************************************/

#include "stdafx.h"
#include "Blinker.h"

/*
	Bei bestehendem Verbindungsaufbau zur Webcam und Ausspielen von Videodaten wird ueber 
	die Callbackmethode openCam_CB Videomaterial im Container scroll ausgegeben.
*/
void Blinker::openCam(){
	openCam_CB();
	scroll->redraw();
}

/*
	Statische Methode, die den Aufruf fuer einen Timeout Callback 
	der Callback-Methode play_CB ermoeglicht.
*/
void Blinker::play_CB_stat(void* v){
	((Blinker*)v)->play_CB();
}

/*
	Wird alle 0.04 Sekunden aufgerufen um ein neues Bild des Videostroms
	zu holen und auszugeben.
*/
void Blinker::play_CB(){
	CvRect* rect = 0;

	if(flag_play == false) 
		return;

	img_frame = cap.getFrame();
	img_frame = detection->detectVideo( img_frame );
	if( detection->detectBlink( img_frame, detection->rEyes ) )
		cout << "Blink!";

	win_frame->setImage( img_frame );
	scroll->redraw();
	Fl::wait(0);

	Fl::add_timeout(0.04, (Fl_Timeout_Handler)play_CB_stat, this);
}

/*
	Callbackmethode fuer den Verbindungsaufbau zur Webcam und Ausspielen der Videodaten 
	wird von openCam aufgerufen. Knopft cam_opener wird deaktiviert.
*/
void Blinker::openCam_CB(){
	try {
		cap.captureFromCamera(0);
		win_frame->setImage(cap.getFrame());		
		scroll_eye->redraw();
		scroll->redraw();
		flag_play = true;
		Fl::add_timeout(0.04, (Fl_Timeout_Handler)play_CB_stat, this);
	} catch(exception &ex){
		cerr << ex.what() << endl;
	}
}

/*
	Constructor.
*/
Blinker::Blinker(int width = 700, int height =400, const char* title = "Blinker"):Fl_Double_Window(width,height,title){
	flag_play			= false;
	scroll				= (Fl_Scroll*)0;
	scroll_eye			= (Fl_Scroll*)0;
	scroll_eye_template = (Fl_Scroll*)0;
	win_frame			= (CWindow*)0;
	win_eye				= (CWindow*)0;
	win_eye_template	= (CWindow*)0;
	img_frame			= (IplImage*)0;
	img_eye				= (IplImage*)0;
	img_eye_template	= (IplImage*)0;

	/*face = new CFace();*/
	detection = new CDetection();
}

/*
	Destructor.
*/
Blinker::~Blinker(){
	if( img_frame )
		cvReleaseImage( &img_frame );

	/*delete face;*/
}

/*
	Diese Methode erzeugt die Fensterapplikation.  
*/
void Blinker::creatWin(){
	try {
		this->color((Fl_Color)31);
		this->begin();
		{
			scroll = new Fl_Scroll(5, 10, 430, 325); // 330, 250
			scroll->box(FL_EMBOSSED_FRAME);
			scroll->color((Fl_Color)23);
			win_frame = new CWindow(10,15,420,315);
			scroll->add(win_frame);
			scroll->end();
		}		
		{
			scroll_eye_template = new Fl_Scroll(500, 10, 100, 100);
			scroll_eye_template->box(FL_EMBOSSED_FRAME);
			scroll_eye_template->color((Fl_Color)23);
			win_eye_template = new CWindow(505, 15, 90, 90);
			scroll_eye_template->end();
		}
		{
			scroll_eye = new Fl_Scroll(500, 150, 100, 100);
			scroll_eye->box(FL_EMBOSSED_FRAME);
			scroll_eye->color((Fl_Color)23);
			win_eye = new CWindow(505, 155, 90, 90);
			scroll_eye->add(win_eye);
			scroll_eye->end();
		}

		this->end();
		this->show();

		openCam();

	} catch(exception &ex){
		cerr << ex.what() << endl;
	}
}

/*
	Main mehtod fuer Validierungszwecke.
*/
int _tmain(int argc, _TCHAR* argv[])
{
	 Blinker* prog = new Blinker(700, 400,"Blinker");
	 prog->creatWin();
	 Fl::run();
     delete prog;

	 return 0;
}
