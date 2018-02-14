#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>

using Cv_VideoCapture = cv::VideoCapture;
using Cv_Mat = cv::Mat;
using Cv_Rect = cv::Rect;
using Cv_CascadeClassifier = cv::CascadeClassifier;
using Cv_Size = cv::Size;

constexpr const char* haarCascadeHand = "res/haarcascade_hand.xml";

void DetectAndDisplay(Display* xorgDisplay,
							 Cv_Mat const& frame,
							 Cv_CascadeClassifier& cascade,
							 int const& w,
							 int const& h) {
  std::vector<Cv_Rect> hands;
  Cv_Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  cascade.detectMultiScale( frame_gray, hands, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Cv_Size(100, 100), Cv_Size(160, 160));

  for( size_t i = 0; i < hands.size(); i++ ) {
	 XWarpPointer(xorgDisplay,None,None,0,0,0,0,
	 		 w-(w/frame.cols+2) * hands[0].x, 
	 		 (h/(frame.rows-220)) * hands[0].y);
	 XSync(xorgDisplay,False);
  }
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
  Display* disp = XOpenDisplay(NULL);
  if(disp == nullptr) {
	 std::cerr << "Unable to connect to Xorg server" << std::endl;
	 return 1;
  }
  
  int scrHeight;
  int scrWidth;
  GetScreenSize(disp, scrWidth, scrHeight);

  std::cout << "Screen:\n\tWidth: " << scrWidth << "\n\tHeight: " << scrHeight << std::endl;
  
  Cv_CascadeClassifier cascade;
  if(!cascade.load(haarCascadeHand)) {
	 std::cerr << "Unable to load HAAR cascade: " << haarCascadeHand << std::endl;
	 return 1;
  }

  Cv_VideoCapture source;
  if(!source.open(0)) {
	 std::cerr << "Unable to open source 0" << std::endl;
	 return 1;
  }

  std::cout << "Starting capture..." << std::endl;
  
  Cv_Mat frame;
  while(source.read(frame)) {
	 if(frame.empty()) {
		std::cerr << "No frame captured, exiting..." << std::endl;
		return 1;
	 }

	 DetectAndDisplay(disp, frame, cascade, scrWidth, scrHeight);
  }

  //unreachable code
  return 0;
}
