#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#include <signal.h>

static int receivedSignal = 0;
static volatile sig_atomic_t run = true;
static std::vector<cv::Rect> handsFist;

constexpr char haarCascadeFist[] = "res/haarcascade_fist.xml";

static void set_runflag_zero(int sig) {
  	run = false;
  	receivedSignal = sig;
}

//
// main()
//
int main()
{
  	std::cout << "setting up..." << std::endl;

	handsFist.reserve(1);

  	Display* disp = XOpenDisplay(NULL);
  	if(disp == nullptr) {
	 	std::cerr << "Unable to connect to Xorg server" << std::endl;
	 	return 1;
  	}

  	Screen* scr = XScreenOfDisplay(disp, 0);
  	int scrWidth = XWidthOfScreen(scr);
  	int scrHeight = XHeightOfScreen(scr);

  	std::cout << "screen:\n\twidth: " << scrWidth << "\n\theight: " << scrHeight << std::endl;

  	cv::CascadeClassifier casFist;
  	if(!casFist.load(haarCascadeFist)) {
	 	std::cerr << "Unable to load HAAR cascade: " << haarCascadeFist << std::endl;
	 	return 1;
  	}

  	cv::VideoCapture source;
  	if(!source.open(0, cv::CAP_V4L)) {
	 	std::cerr << "Unable to open source 0" << std::endl;
	 	return 1;
  	}

  	signal(SIGINT, set_runflag_zero);
  	signal(SIGTERM, set_runflag_zero);

  	Window rootWin = XRootWindow(disp,0);
  	cv::Mat frame;

#ifdef COORDS
  	std::cout << "\033[7mcapturing...\033[0m\ncoords:\n"
		<< "\tx: \033[1m0\033[0m, y: \033[1m0\033[0m\r" << std::flush;
#endif

	cv::Size sq100(100, 100);
	cv::Size sq160(160, 160);

  	while(source.read(frame) && run) {
	 	if(frame.empty()) {
			std::cerr << "No frame captured, exiting..." << std::endl;
			return 1;
	 	}

		cv::Mat gray_frame;

		cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(gray_frame, gray_frame);

  		casFist.detectMultiScale( 
  				gray_frame, 
  				handsFist, 
  				1.1, 
  				2, 
  				0|cv::CASCADE_SCALE_IMAGE, 
  				sq100, sq160);

  		int xw;
  		int yh;

  		xw = scrWidth - ((scrHeight / frame.cols + 2) * handsFist[0].x);
  		yh = (scrHeight / (frame.rows-220)) * handsFist[0].y;
  		
 		XWarpPointer(disp,None,
	 	    	rootWin,
				0,0,0,0,
				xw, yh);
		
		XFlush(disp);
#ifdef COORDS
		std::cout << "\tx: \033[1m" << xw << "\033[0m, y: \033[1m" << yh << "\033[0m\r" << std::flush;
#endif
  	}

  	std::cout << std::endl;

  	std::cout << "received signal: " << receivedSignal << "\nclosing X server connection..." << std::endl;

  	XSync(disp,True);
  	XCloseDisplay(disp);

  	std::cout << "bye" << std::endl;
  	return 0;
}
