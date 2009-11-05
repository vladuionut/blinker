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
	loadHaarClassifier();
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
	if(flag_play==false) 
		return;
	frame=cap.GetFrame();
	// detect_and_draw();
	pic->SetImage(frame);
	scroll->redraw();
	Fl::wait(0);

	Fl::add_timeout(0.01, (Fl_Timeout_Handler)play_CB_stat, this);
}

void Blinker::extractFaceCB_stat(Fl_Widget* wid,void* v){
	((Blinker*)v)->extractFace();
}
void Blinker::extractFaceCB(){
	if(flag_eyeExtract)
		return;
	if(extractFace())
		fFace->redraw();
	else
		Fl::add_timeout(0.1, (Fl_Timeout_Handler)extractFaceCB_stat, this);
}

bool Blinker::extractFace(){
	flag_play = false;
	flag_faceExtract = true;
	IplImage * ex_img = NULL;
	
	cvClearMemStorage(storage);

	if(cascade_face){
		if(faces) 
			cvClearSeq(faces);
		// frame ..... last frame of play-mode
		faces = cvHaarDetectObjects(frame, cascade_face, storage, ...);
	}

	flag_play = true;
	flag_faceExtract = false;
	
	return true;
}

void Blinker::loadHaarClassifier(){
	if(cascade_face)
		cvReleaseHaarClassifierCascade(&cascade_face);
	if(cascade_eye)
		cvReleaseHaarClassifierCascade(&cascade_eye);

	cascade_face = (CvHaarClassifierCascade*)cvLoad( cascade_face_names[0], 0, 0, 0 );
	cascade_eye = (CvHaarClassifierCascade*)cvLoad( cascade_eye_names[0], 0, 0, 0 );
	if(!cascade_face || !cascade_eye) {
		fl_alert("Blinker::loadHaarClassifier: Error loading HaarClassifier.");
		exit(-1);
	}
	storage = cvCreateMemStorage(0);
}

/*
	Callbackmethode fuer den Verbindungsaufbau zur Webcam und Ausspielen der Videodaten 
	wird von openCam aufgerufen. Knopft cam_opener wird deaktiviert.
*/
void Blinker::openCam_CB(){
	try {
		if(!cap.CaptureFromCAM(0)) // Default -1
			return;
		pic->SetImage(cap.GetFrame());
		scroll->redraw();
		flag_play=true;
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
	flag_play = false;
	scroll = (Fl_Scroll*)0;
	cam_opener = (Fl_Button*)0;
	pic = (Fl_OpenCV*)0;
	frame = (IplImage*)0;
	cascade_face = (CvHaarClassifierCascade*)0;
	storage = (CvMemStorage*)0;
	faces = (CvSeq*)0;
}

/*
	Destructor.
*/
//Blinker::~Blinker(){
//}

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
			pic = new Fl_OpenCV(10,15,320,240);
			scroll->add(pic);
			pic->FitWindows();
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
	// Debug purpose !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{ 
			faceButton = new Fl_Button(372, 120, 218, 30, "Extract Face");
			faceButton->box(FL_PLASTIC_UP_BOX);
			faceButton->color(FL_SELECTION_COLOR);
			faceButton->labelfont(1);
			faceButton->labelsize(15);
			faceButton->callback((Fl_Callback*)extractFaceCB_stat);
		} // Fl_Button* ExtractBut
		{ 
		  Fl_Group* o = new Fl_Group(380, 5, 60, 60);
		  o->box(FL_EMBOSSED_FRAME);
		  o->color((Fl_Color)23);
		  o->align(129);
		  fFace=new Fl_OpenCV(385,10,50,50);
		  o->end();
		} // Fl_Group* o

		this->end();
		this->resizable(this);
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
