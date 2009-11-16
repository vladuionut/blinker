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
	Constructor.
*/
Blinker::Blinker(int width = 700, int height =400, const char* title = "Blinker"):Fl_Double_Window(width,height,title){
	flag_play = false;
	scroll = (Fl_Scroll*)0;
	cam_opener = (Fl_Button*)0;
	pic = (Fl_OpenCV*)0;
	frame = (IplImage*)0;
	detector = new Detection();
}

/*
	Destructor.
*/
Blinker::~Blinker(){
	delete detector;
	delete scroll;
	delete cam_opener;
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

		this->end();
		this->resizable(this);
		this->show();

	} catch(exception &ex){
		cerr << ex.what() << endl;
	}
}

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
		openCam_CB();
		scroll->redraw();
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
	
	
	detect(); // detect face and eyes

	pic->SetImage(frame);
	scroll->redraw();
	Fl::wait(0);

	Fl::add_timeout(0.01, (Fl_Timeout_Handler)play_CB_stat, this);
}

void Blinker::detect(){
	frame = detector->detectVideo( frame );
	//detector->detectBlink(frame);
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
