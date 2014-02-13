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

This software allows people to generate negative background images based on a single set of
training data.

*****************************************************************************************************/

// Includes for openCV functionality, requires openCV 2.4.5 or higher --> uses new C++ functionality
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
	// Check if arguments are given correct
	if( argc == 1 || argc != 4 ){
		printf( "Usage of generate negatives software: \n"
				"--> generate_negatives.exe <annotation_file> <folder> <type[jpg, png, ppm]>\n");
		return 0;
	}

	// Retrieve configuration parameters
	string annotation_file = argv[1];
	string folder_location = argv[2];
	string image_type = argv[3];

	// Read through the txt file and process the given information
	// Each annotation needs to be replaced by a blackend region
	ifstream my_annotations ( annotation_file.c_str() );
    string current_line;
    int counter = 0;
	while(!my_annotations.eof()){
		// Read the first line and check if it has a value, else break out of loop
		getline(my_annotations, current_line, '\n');
		if (current_line == ""){
		    break;
        }

		// Split the string into parameters
		vector<string> data;
		string element = "";
		stringstream temp_stream( current_line.c_str() );
		while(getline(temp_stream, element,' ')){
			data.push_back(element);
		}

		// Read in the original image
		Mat img = imread( data[0] );

		// Retrieve rectangle bounding box parameters
		// Parameterposition ignores the location and amount of detections and is then used for looping over bounding boxes
		int amount_detections = atoi(data[1].c_str());
		int x = 0, y = 0, width = 0, height = 0;
		for(int i = 0; i < amount_detections; i ++){
			// Grab each bounding box from annotations
			x = atoi(data[(i*4)+2].c_str());
			y = atoi(data[(i*4)+3].c_str());
			width = atoi(data[(i*4)+4].c_str());
			height = atoi(data[(i*4)+5].c_str());

			// Add a black filled rectangle to the image based on the information
			rectangle(img, Point(x,y), Point(x+width,y+height), Scalar(0,0,0), CV_FILLED);
		}

		// After all rectangles have been added, save the result
		stringstream out;
		stringstream date;

		// Create a unique blueprint for the name!
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		date << (now->tm_year + 1900) << (now->tm_mon + 1) << now->tm_mday << now->tm_hour << now->tm_min << now->tm_sec;

		out << folder_location << "negative_" << date.str() << "_" << counter << "." << image_type;

		imwrite(out.str(), img);

		// Add image index
		counter++;
	}

	return 0;
}
