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

Software for performing object detection based on a trained object model.
Choice between video and image sequence input for detection purposes.

*****************************************************************************************************/

// Standard windows includes + input output file processing
#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <strsafe.h>

// OpenCV includes needed for correct functionality to work
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Including the used namespaces
using namespace std;
using namespace cv;

// Function headers
void detect_and_display( Mat frame, string window );
void detect_and_display( Mat frame, string window, ofstream& output );

// Global variables
CascadeClassifier cascade;
string window_name = "Cascade classifier object detector";

int main( int argc, const char** argv )
{
	// Check if arguments are given correct
	if( argc == 1 ){
		printf( "Usage of model detection software: \n"  
			"detect_objects.exe <object_model.xml> <input_type> <input_data> <detections_output> \n"
				"   --> input type = video OR images \n"
				"   --> input data = video location OR images folder \n"
				"   --> detections output = txt file that will contain all detections \n");

		return 0;
	}

	// Load the cascade model into the model container
	string model_name = argv[1];
	if( !cascade.load( model_name ) ){ 
		printf("Error loading the trained model from the provided model file!\n"); 
		return -1; 
	};
  
	// Check which image type is supplied
	string input_type = argv[2];

	// If input type is an image sequence, then load in all filenames for processing
	vector<string> filenames;
	if ( strcmp(input_type.c_str(), "images") == 0 ){
		// Location of the image data
		string location_data = argv[3];

		// list every filename from the annotated folder
		WIN32_FIND_DATA ffd;
		TCHAR szDir[MAX_PATH];
		HANDLE hFind = INVALID_HANDLE_VALUE;
		
		StringCchCopy(szDir, MAX_PATH, location_data.c_str());
		StringCchCat(szDir, MAX_PATH, TEXT("*"));

		hFind = FindFirstFile(szDir, &ffd);
		do
		{
			filenames.push_back (ffd.cFileName);	
		}
		while (FindNextFile(hFind, &ffd) != 0);
		// Remove the current and parent directory name (. and ..)
		filenames.erase(filenames.begin(), filenames.begin() + 2);

		FindClose( hFind );
	}

	// If output txt file with detections is required, then pass along the folder
	string output_folder = argv[4];
	string location_output = output_folder + "detections.txt";
	ofstream output_file;
	output_file.open(location_output);

	// If input is video, a basic frame capture is applied, else we use the image filenames
	Mat current_frame;
	if ( strcmp(input_type.c_str(), "video") == 0 ){
		string video_location = argv[3];
		VideoCapture cap (video_location);

		// Keep looping until frame is empty
		int frame_counter = 0;
		for(;;){
			cap >> current_frame;
			
			// If an empty frame turns up, then video is processed completely
			if ( current_frame.empty() ){
				break;
			}

			// Perform and visualize detection
			// Save if requested
			if ( !argv[4] ){
				detect_and_display( current_frame, window_name );
			}else{
				output_file << "frame_" << frame_counter;
				detect_and_display( current_frame, window_name, output_file );
			}

			frame_counter++;
		}
	}else{
		// Loop over each image in the sequence and perform detection
		for(int i = 0; i < filenames.size(); i++){
			// Retrieve file location
			stringstream ss;
			ss << argv[3] << filenames[i];
			current_frame = imread(ss.str());

			// Perform and visualize detection
			// Save if requested
			if ( !argv[4] ){
				detect_and_display( current_frame, window_name );
			}else{
				output_file << filenames[i];
				detect_and_display( current_frame, window_name, output_file );
			}
		}
	}

	output_file.close();

	return 0;
}

// FUNCTION : Detects and displays an object detection on the current frame
// WITHOUT SAVE FUNCTIONALITY
void detect_and_display( Mat frame, string mask_file )
{
	// Function specific parameters
	vector<Rect> objects;
	Mat frame_gray;

	// Convert the input frame to grayscale image and apply lightning normalization using histogram equilization
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// Detect object in a given image
	// Parameters should be checked at : http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html?highlight=detectmultiscale#void%20CascadeClassifier::detectMultiScale%28const%20Mat&%20image,%20vector%3CRect%3E&%20objects,%20double%20scaleFactor,%20int%20minNeighbors,%20int%20flags,%20Size%20minSize,%20Size%20maxSize%29
	// Below command would detect and visualize all single detections
	cascade.detectMultiScale( frame_gray, objects, 1.05, 0, 0);
	
	// Below command would detect and visualize all detections that have 5 or more matching overlaps
	//cascade.detectMultiScale( frame_gray, objects, 1.05, 5, 0);

	// Below command would detect and visualize all detections that have 5 or more matching overlaps and specify a maximum and minimal object size
	//cascade.detectMultiScale( interested_region, objects, 1.05, 5, 0, Size(10, 25), Size(100, 250));

	// Visualize detections on the input frame and show in the given window
	for( int i = 0; i < objects.size(); i++ )
	{
		// Use a rectangle representation on the frame
		// Frame width 3 pixels in color red (BGR format)
		rectangle(frame, Point(objects[i].x, objects[i].y), Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), Scalar(0, 0, 255), 3);
	}
   
	// Show the result
	imshow( window_name, frame );
}

// FUNCTION : Detects and displays an object detection on the current frame
// WITH SAVE FUNCTIONALITY
void detect_and_display( Mat frame, string mask_file, ofstream& output_file )
{
	// Function specific parameters
	vector<Rect> objects;
	Mat frame_gray;

	// Convert the input frame to grayscale image and apply lightning normalization using histogram equilization
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// Detect object in a given image
	// Parameters should be checked at : http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html?highlight=detectmultiscale#void%20CascadeClassifier::detectMultiScale%28const%20Mat&%20image,%20vector%3CRect%3E&%20objects,%20double%20scaleFactor,%20int%20minNeighbors,%20int%20flags,%20Size%20minSize,%20Size%20maxSize%29
	// Below command would detect and visualize all single detections
	cascade.detectMultiScale( frame_gray, objects, 1.05, 0, 0);
	
	// Below command would detect and visualize all detections that have 5 or more matching overlaps
	//cascade.detectMultiScale( frame_gray, objects, 1.05, 5, 0);

	// Below command would detect and visualize all detections that have 5 or more matching overlaps and specify a maximum and minimal object size
	//cascade.detectMultiScale( interested_region, objects, 1.05, 5, 0, Size(10, 25), Size(100, 250));

	// Visualize detections on the input frame and show in the given window
	for( int i = 0; i < objects.size(); i++ )
	{
		// Use a rectangle representation on the frame
		// Frame width 3 pixels in color red (BGR format)
		rectangle(frame, Point(objects[i].x, objects[i].y), Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), Scalar(0, 0, 255), 3);
	}
   
	// Show the result
	imshow( window_name, frame );

	// Save detections to file if requested
	output_file << " " << objects.size();
	for(int i = 0; i < objects.size(); i++){
		output_file << " " << objects[i].x << " " << objects[i].y << " " << objects[i].width << " " << objects[i].height;
	}
	output_file << endl;
}

