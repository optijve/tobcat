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

This software can be used for easily grabbing frames from a video and store them as images
in order to perform some processing on it

*****************************************************************************************************/

// Includes for openCV functionality, requires openCV 2.4.5 or higher --> uses new C++ functionality
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Includes for standard file input output functionality
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    // Check if arguments are given correct
	if( argc == 1 && argc != 3 ){
		cout << "Usage of video2images software;" << endl;
		cout <<	"video2images.exe <video_file> <output_path>" << endl;
		return 0;
	}

	// Open a video capture interface
	VideoCapture input_video ( argv[1] );
	if ( !input_video.isOpened() ){
	    cout << "Video cannot be opened correctly. Either format is not supported or location provided is wrong." << endl;
	    return -1;
	}

	// Configure parameters for writing images from video
	vector<int> parameters;
	parameters.push_back(CV_IMWRITE_PNG_COMPRESSION); parameters.push_back(0);

	// Grab a frame, check if it is still valid, store it in the output directory
    Mat current_frame;
    string base_path = argv[2];
    int counter = 0;
    cerr << "Starting conversion: ";
    for(;;){
        // Grab a frame
        input_video >> current_frame;

        // Perform check
        if (current_frame.empty()){
            cout << endl << "Last frame grabbed was empty, reached end of video file!" << endl;
            break;
        }

        // Store the image in the output location
        stringstream storage_location;
        storage_location << base_path << "video_frame_" << counter << ".png";
        imwrite(storage_location.str(), current_frame, parameters);

        // Add the counter and visualize a recording
        counter++;
        cerr << "@";
    }

    cout << endl;

    return 0;
}
