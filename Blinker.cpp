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
	IplImage* img_help = 0;

	if(flag_play == false) 
		return;


	img_frame = cap.getFrame();
	img_frame = detection->detectVideo( img_frame );

	win_frame->setImage( img_frame );
	scroll->redraw();

	if( detection->detectBlink( img_frame, detection->rEyes ) ) {
		img_help = cvCreateImage( cvSize( scroll_snapshot->w(),
								          scroll_snapshot->h() ), 
								  IPL_DEPTH_8U, 
								  3 );
		cvResize( img_frame, img_help, CV_INTER_LINEAR );
		cvCopyImage(img_help,img_snapshot,NULL);
		cout << "BlinkBlink!" << endl;
		/*win_snapshot->setImage( img_snapshot );
		scroll_snapshot->redraw();*/
	}

	if(img_help)cvReleaseImage(&img_help);

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
	scroll_snapshot     = (Fl_Scroll*)0;
	win_frame			= (CWindow*)0;
	win_snapshot		= (CWindow*)0;
	img_frame			= (IplImage*)0;
	img_snapshot		= (IplImage*)0;

	/*face = new CFace();*/
	detection = new CDetection();
}

/*
	Destructor.
*/
Blinker::~Blinker(){
	if( img_frame )
		cvReleaseImage( &img_frame );
	if( img_snapshot )
		cvReleaseImage( &img_snapshot );
	/*delete face;*/
}

/*
	Diese Methode erzeugt die Fensterapplikation.  
*/
void Blinker::creatWin(){
	int width, height;
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
			width = scroll->w()/3;
			height = scroll->h()/3;
			scroll_snapshot = new Fl_Scroll(500, 150, width, height);
			scroll_snapshot->box(FL_EMBOSSED_FRAME);
			scroll_snapshot->color((Fl_Color)23);
			win_snapshot = new CWindow(505, 155, width-10, height-10);
			scroll_snapshot->add(win_snapshot);
			scroll_snapshot->end();
			img_snapshot = cvCreateImage( cvSize( scroll_snapshot->w(),
								          scroll_snapshot->h() ), 
										  IPL_DEPTH_8U, 
										  3 );
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
