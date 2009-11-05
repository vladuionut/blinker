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
	Statische Methode, die nach Druecken von cam_opener das Aufrufen der Methode openCam durchfuehrt.
*/
void Blinker::openCam_stat(Fl_Widget* wid, void* v) {
	((Blinker*)v)->openCam();
}
/*
	Bei bestehendem Verbindungsaufbau zur Webcam und Ausspielen von Videodaten wird ueber 
	die Callbackmethode openCam_CB Videomaterial im Container scroll ausgegeben.
*/
void Blinker::openCam(){
	load_classifiers = loadHaarClassifier();
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
	Wird alle 0.01 Sekunden aufgerufen um ein neues Bild des Videostroms
	zu holen und auszugeben.
*/
void Blinker::play_CB(){
	CvRect* rect = 0;

	if(flag_play == false) 
		return;
	frame = cap.getFrame();

	// detect face and eyes
	/*rect = detectFaces();
	detectEyes(rect);*/
	frame = face->performDetection( frame );

	pic->setImage(frame);
	scroll->redraw();
	Fl::wait(0);

	Fl::add_timeout(0.01, (Fl_Timeout_Handler)play_CB_stat, this);
}

/*
	Callbackmethode fuer den Verbindungsaufbau zur Webcam und Ausspielen der Videodaten 
	wird von openCam aufgerufen. Knopft cam_opener wird deaktiviert.
*/
void Blinker::openCam_CB(){
	try {
		cap.captureFromCamera(0);
		pic->setImage(cap.getFrame());
		scroll->redraw();
		flag_play = true;
		Fl::add_timeout(0.05, (Fl_Timeout_Handler)play_CB_stat, this);
		cam_opener->deactivate();
	} catch(exception &ex){
		cerr << ex.what() << endl;
	}
}

/*
	Constructor.
*/
Blinker::Blinker(int width = 700, int height =400, const char* title = "Blinker"):Fl_Double_Window(width,height,title){
	flag_play			= false;
	load_classifiers	= false;
	scroll				= (Fl_Scroll*)0;
	cam_opener			= (Fl_Button*)0;
	pic					= (CWindow*)0;
	frame				= (IplImage*)0;

	cascade_face		= 0;
	cascade_eye			= 0;
	storage				= 0;
	cascade_face_name	= "F:\\Studium\\9.Semester WS 09\\Visual Analysis of Human Motion\\Übung\\Blinker\\Blinker\\haarcascade_face.xml";
	cascade_eye_name	= "F:\\Studium\\9.Semester WS 09\\Visual Analysis of Human Motion\\Übung\\Blinker\\Blinker\\haarcascade_eye.xml";

	face = new CFace();
}

/*
	Destructor.
*/
Blinker::~Blinker(){
	if(frame)
		cvReleaseImage(&frame);

	delete face;
}

bool Blinker::loadHaarClassifier() {
	if (cascade_face)
		cvReleaseHaarClassifierCascade( &cascade_face );
	if (cascade_eye)
		cvReleaseHaarClassifierCascade( &cascade_eye );

	cascade_face = (CvHaarClassifierCascade*) cvLoad( cascade_face_name, 0, 0, 0);
	cascade_eye  = (CvHaarClassifierCascade*) cvLoad( cascade_eye_name, 0, 0, 0);

	if ( !cascade_face  ) {
		fl_alert( "Error: Could not load cascade face classifier!" );
		return false;
	}

	if ( !cascade_eye ) {
		fl_alert( "Error: Could not load cascade eye classifier!" );
		return false;
	}

	return true;
}

CvRect* Blinker::detectFaces() {
	CvRect* rect = 0;
	IplImage* temp = cvCreateImage( cvSize(frame->width,frame->height), 8, 3 );
	storage = cvCreateMemStorage(0);
	cvClearMemStorage( storage );
	CvSeq* faces = cvHaarDetectObjects( frame, cascade_face, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

	for ( int i = 0; i < (faces ? faces->total : 0); i++) {
		rect = (CvRect*)cvGetSeqElem( faces, i );
		cvRectangle( frame, cvPoint(rect->x,rect->y),
                     cvPoint((rect->x+rect->width),
                             (rect->y+rect->height)),
                     CV_RGB(255,0,0), 3 );

		cvRectangle( frame, 
			         cvPoint(rect->x,rect->y+rect->height/5),
                     cvPoint((rect->x+rect->width),
                             (rect->y+rect->height/2)),
                     CV_RGB(0,255,0), 3 );
	}

	cvReleaseImage( &temp );
	cvClearMemStorage( storage );

	return rect;
}

void Blinker::detectEyes(CvRect* rect) {
	if ( !rect )
		return;

	IplImage* temp = cvCreateImage( cvSize(frame->width,frame->height), 8, 3 );
	storage = cvCreateMemStorage(0);
	cvClearMemStorage( storage );
	cvSetImageROI( frame, cvRect(rect->x, rect->y+rect->height/5, rect->width, rect->height/3) );
	CvSeq* eyes = cvHaarDetectObjects( frame, cascade_eye, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );

	for ( int i = 0; i < (eyes ? eyes->total : 0); i++) {
		rect = (CvRect*)cvGetSeqElem( eyes, i );
		cvRectangle( frame, cvPoint(rect->x,rect->y),
                     cvPoint((rect->x+rect->width),(rect->y+rect->height)),
                     CV_RGB(255,0,0), 3 );

	}

	cvReleaseMemStorage( &storage );
	cvReleaseImage( &temp );
	cvResetImageROI( frame );
}

/*
	Diese Methode erzeugt die Fensterapplikation.  
*/
void Blinker::creatWin(){
	try {
		this->color((Fl_Color)31);
		this->begin();
		{
			scroll = new Fl_Scroll(5, 10, 330, 250);
			scroll->box(FL_EMBOSSED_FRAME);
			scroll->color((Fl_Color)23);
			pic = new CWindow(10,15,320,240);
			scroll->add(pic);
			scroll->end();
		}
		{
			cam_opener = new Fl_Button(376, 80, 215, 30, "Open Web-CAM");
			cam_opener->box(FL_PLASTIC_UP_BOX);
			cam_opener->color(FL_SELECTION_COLOR);
			cam_opener->labelfont(1);
			cam_opener->labelsize(15);
			cam_opener->callback((Fl_Callback*)openCam_stat,this);
		}

		this->end();
		this->show();

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
