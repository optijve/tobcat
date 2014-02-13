/***************************************************************************************************
Copyright (c) 2013 EAVISE, KU Leuven, Campus De Nayer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*****************************************************************************************************

Software for performing real time face detection

*****************************************************************************************************/

// Includes for openCV functionality
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Includes for outputting information
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
{
	// Standard interface for executing the program
	if( argc == 1 ){
		cout << "Simple real time face detection demo." << endl;
		cout << "real_time_face_detection.exe <webcam number - default = 0> <face_model.xml> <# of overlaps - default = 15>" << endl;
		return 0;
	}

	// Retrieve the correct capturing device
	int device_number = atoi(argv[1]);
	VideoCapture capture( device_number );

	if(!capture.isOpened()){
		cout << "Could not open webcam input, make sure a webcam is connected to the system." << endl;
		return -1;
	}

	Mat frame, grayscale;
	string window_name = "Face detection demo, please smile";

	CascadeClassifier cascade;
	cascade.load(argv[2]);

	int number_overlaps = atoi(argv[3]);

	while( true ){
		capture >> frame;
		if( !frame.empty() ){

		}else{
			cout << "Bad frame received, closing down application!";
			break;
		}

		// Process frame and perform detection
		cvtColor(frame, grayscale, COLOR_BGR2GRAY);
		equalizeHist(grayscale, grayscale);

		vector<Rect> faces;

		cascade.detectMultiScale(grayscale, faces, 1.05, number_overlaps);

		// Add rectangle around each result and display
		for( int i = 0; i < faces.size(); i++ ){
			rectangle(frame, faces[i], Scalar(0,0,255), 2);
			stringstream face;
			face << "Face " << (i + 1);
			putText(frame, face.str(), Point(faces[i].x, faces[i].y - 15), 1, 2, Scalar(0, 0, 255), 1 );
		}

		imshow(window_name, frame);

		int key = waitKey(25);
		if(key == 27 ) { break;}
	}

	return 0;
}

