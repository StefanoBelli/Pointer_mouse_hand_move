#include <iostream>
#include <thread>
#include <chrono>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#include <signal.h>

using Cv_VideoCapture = cv::VideoCapture;
using Cv_Mat = cv::Mat;
using Cv_Rect = cv::Rect;
using Cv_CascadeClassifier = cv::CascadeClassifier;
using Cv_Size = cv::Size;

static int receivedSignal = 0;
static volatile sig_atomic_t run = true;
static std::vector<Cv_Rect> handsFist;
static std::vector<Cv_Rect> handsPalm;

constexpr char haarCascadeFist[] = "res/haarcascade_fist.xml";
constexpr char haarCascadePalm[] = "res/haarcascade_palm.xml";

static void set_runflag_zero(int sig) {
  	run = 0;
  	receivedSignal = sig;
}

static void DetectAndDisplay(Display* xorgDisplay,
		Window const& rootWindow,
		Cv_Mat const& frame,
		Cv_CascadeClassifier& fistCascade,
		Cv_CascadeClassifier& palmCascade,
		int const& w,
		int const& h) {

	handsPalm.clear();
	handsFist.clear();
	
 	Cv_Mat frame_gray;

  	cvtColor( frame, frame_gray, CV_BGR2GRAY );
  	equalizeHist( frame_gray, frame_gray );

  	fistCascade.detectMultiScale( 
  			frame_gray, 
  			handsFist, 
  			1.1, 
  			2, 
  			0|CV_HAAR_SCALE_IMAGE, 
  			Cv_Size(100, 100), 
  			Cv_Size(160, 160));

  	palmCascade.detectMultiScale( 
  			frame_gray, 
  			handsPalm, 
  			1.1, 
  			2, 
  			0|CV_HAAR_SCALE_IMAGE, 
  			Cv_Size(100, 100), 
  			Cv_Size(160, 160));


  	int xw;
  	int yh;
  	
  	if(handsFist.size()) {
	    xw = w - ((w / frame.cols + 2) * handsFist[0].x);
 	    yh = (h / (frame.rows-220)) * handsFist[0].y;
 	    XWarpPointer(xorgDisplay,None,
	 	        rootWindow,
				0,0,0,0,
				xw,yh);

	 	std::cout << "\tx: \033[1m" << xw << "\033[0m, y: \033[1m" << yh << "\033[0m\r" << std::flush;
  	} else if(handsPalm.size() == 1) {
		XTestFakeButtonEvent(xorgDisplay, 1, True, CurrentTime);
		XTestFakeButtonEvent(xorgDisplay, 1, False, CurrentTime);
		std::cout << "\tpalm: \033[1mdetected\033[0m!\r" << std::flush;
	}

  	XFlush(xorgDisplay);
    cv::imshow("HandMovement", frame_gray);
    cv::waitKey(10);
}

void GetScreenSize(Display* xorgDisplay,
		int& width,
		int& height) {
  	Screen* scr = XScreenOfDisplay(xorgDisplay, 0);
  	width = XWidthOfScreen(scr);
  	height = XHeightOfScreen(scr);
}

//
// main()
//
int main()
{
  	std::cout << "setting up..." << std::endl;

  	handsFist.reserve(3);
  	handsPalm.reserve(3);

  	Display* disp = XOpenDisplay(NULL);
  	if(disp == nullptr) {
	 	std::cerr << "Unable to connect to Xorg server" << std::endl;
	 	return 1;
  	}

  	int scrHeight;
  	int scrWidth;
  	GetScreenSize(disp, scrWidth, scrHeight);

  	std::cout << "screen:\n\twidth: " << scrWidth << "\n\theight: " << scrHeight << std::endl;

  	Cv_CascadeClassifier casFist;
  	if(!casFist.load(haarCascadeFist)) {
	 	std::cerr << "Unable to load HAAR cascade: " << haarCascadeFist << std::endl;
	 	return 1;
  	}

  	Cv_CascadeClassifier casPalm;
  	if(!casPalm.load(haarCascadePalm)) {
	 	std::cerr << "Unable to load HAAR cascade: " << haarCascadePalm << std::endl;
	 	return 1;
  	}

  	Cv_VideoCapture source;
  	if(!source.open(0)) {
	 	std::cerr << "Unable to open source 0" << std::endl;
	 	return 1;
  	}

  	signal(SIGINT, set_runflag_zero);
  	signal(SIGTERM, set_runflag_zero);

  	Window rootWin = XRootWindow(disp,0);
  	Cv_Mat frame;

    cv::namedWindow("HandMovement");

  	std::cout << "\033[7mcapturing...\033[0m\ncoords:\n"
		<< "\tx: \033[1m0\033[0m, y: \033[1m0\033[0m\r" << std::flush;

  	while(source.read(frame) && run) {
	 	if(frame.empty()) {
			std::cerr << "No frame captured, exiting..." << std::endl;
			return 1;
	 	}

	 	DetectAndDisplay(disp, rootWin, frame, casFist, casPalm, scrWidth, scrHeight);
  	}

  	std::cout << std::endl;

  	std::cout << "received signal: " << receivedSignal << "\nclosing X server connection..." << std::endl;

  	XSync(disp,True);
  	XCloseDisplay(disp);

  	std::cout << "bye" << std::endl;
  	return 0;
}
