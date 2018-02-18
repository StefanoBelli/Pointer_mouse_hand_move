#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include<Windows.h>

using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );
void GetDesktopResolution(int& horizontal, int& vertical);
void HideConsole();
/** Global variables */
String cascade_name ="C:/.../data/haarcascades/haarcascade_hand.xml";
CascadeClassifier cascade;
//string window_name = "";
int x;
int y;
/** @function main */
int main()
{
      Mat frame;
      VideoCapture cap;
     //initializing functions
      HideConsole();
      GetDesktopResolution(x, y);
      //-- 1. Load the cascades
      if( !cascade.load( cascade_name ) )
      {
            printf("--(!)Error loading\n"); return -1;
      }
      // open the default camera, use something different from 0 otherwise;
      // Check VideoCapture documentation.
      if ( !cap.open(0) )
      {
            return 0;
      }
      while( cap.read(frame) )
      {
            //-- 3. Apply the classifier to the frame
            if( !frame.empty() )
            {
                  detectAndDisplay( frame );
            }
           else
            {
                  printf(" --(!) No captured frame -- Break!");
                  break;
            }
            int c = waitKey(10);
            if( (char)c == 'c' )
            {
                  break;
            }
       }
       return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
       std::vector<Rect> hands;
       Mat frame_gray;
       cvtColor( frame, frame_gray, CV_BGR2GRAY );
       equalizeHist( frame_gray, frame_gray );
       //-- Detect hands
       cascade.detectMultiScale( frame_gray, hands, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100), Size(160, 160));
       for( size_t i = 0; i < hands.size(); i++ )
       {
           //this function sets the cursor position
           SetCursorPos(x-(x/frame.cols+2)*hands[i].x,(y/(frame.rows-220))*hands[i].y);
       }
       //-- Show what you got
       //imshow( window_name, frame );
}

/** @function GetDesktopResolution */
void GetDesktopResolution(int& horizontal, int& vertical)
{
       RECT desktop;
       // Get a handle to the desktop window
       const HWND hDesktop = GetDesktopWindow();
       // Get the size of screen to the variable desktop
       GetWindowRect(hDesktop, &desktop);
       // The top left corner will have coordinates (0,0)
       // and the bottom right corner will have coordinates
       // (horizontal, vertical)
       horizontal = desktop.right;
       vertical = desktop.bottom;
}      
       
/** @function HideConsole */
void HideConsole()
{      
       ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}
