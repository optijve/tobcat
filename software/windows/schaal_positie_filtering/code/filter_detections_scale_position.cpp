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

Software for cleaning person detections in 360° panoramic images based on known location of camera.
Leads to the possibility of creating a location-scale relationship for detections, which can
exclude a large set of false positive detections.

PREREQUISITES
 - Boundary selections of accepted data based on manually annotated dataset
 - File is supplied at the download section of http://www.eavise.be/tobcat/

*****************************************************************************************************/

// Standard Windows include header
#include "stdafx.h"

// Includes for openCV functionality, requires openCV 2.4.5 or higher!
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"

// Includes for standard functionality
#include <fstream>

// Includes for namespaces
using namespace std;
using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{
	// Standard interface for executing the program
	if( argc == 1 ){
		printf( "Application reads in the following files:\n"
				"1. manual annotion file of a set of samples [default = 0]\n"
				"2. output detection file of the person detector\n"
				"Then processes this data and outputs a cleaned detection file.\n"
				"--> filter_detections_scale_position.exe <manual_annotations.txt> <person_detections.txt> <cleaned_detections.txt> <image root folder>\n");
		return 0;
	}

	// General parameters
	double polygon_a, polygon_b, rico_m1, rico_m2;
	Point border1_start, border1_end, border2_start, border2_end;
	
	ifstream detections; detections.open(argv[2]);
	ofstream cleaned_detections; cleaned_detections.open(argv[3]);
	string input_line;

	// Needed for storing the correct image location
	// Make sure that the images processed are actually at that root folder
	string image_root_folder = argv[4];

	// Easiest way, there is no manual annotation file and you want to process the detection file on known constants
	// Fitted polygon to data = a * x + b
	string manual_annotation = argv[1];
	if (strcmp(manual_annotation.c_str(),"0") == 0){
		// Take parameter values from matlab processing of data
		// Central polygon fitted to the data
		polygon_a = 1.483;
		polygon_b = -1729;

		// Two more precise borders in order to limit data that should be pedestrians
		// Marker 1 : P1(1180, 0) P2(1320 520)
		// Marker 2 : P1(1225, 0) P2(1635, 520)
		// Based on these point, you need to calculate the direction coefficient: m = (y2 - y1)/(x2 - x1) and then apply it as y = m * x + 0
		rico_m1 = (520.0 - 0) / (1320.0 - 1180.0);
		rico_m2 = (520.0 - 0) / (1635.0 - 1225.0);
	}
	
	// Hard way, grab all points from the manual annotations file --> PROCESSING WILL BE PROVIDED LATER

	// Retrieve all detections from the input file and store them in a matrix structure
	// This file is the output from the detector and has structure image - x - y - width - height - score
	vector< vector<string> > all_detection_data;
	// First grab a line to remove the headings
	getline(detections, input_line);
	while( getline(detections, input_line) ){
		// Split the line in parameters
		string element;
		vector<string> line_elements;
		stringstream temp_stream(input_line);
		while( getline(temp_stream, element,'\t') ){
			line_elements.push_back(element);
		}

		// Put the line elements into a new vector containing all elements of each detection
		all_detection_data.push_back(line_elements);
	}

	// Loop over all elements, check whether the detection is inside the allowed boundaries.
	// If so add the detection to the cleaned file, if not, remove the line from the file.
	for(int i = 0; i < all_detection_data.size(); i ++){
		// Define how many detections this file has
		vector<string> current_elements = all_detection_data[i];
		
		// Check if the center point of the detection is inside the marker regions
		int y = atoi(current_elements[2].c_str());
		int height = atoi(current_elements[4].c_str());
		int middle_point = y + (height / 2);

		// Push into borders to define max and min allowed value
		double center_high = rico_m1 * (double)(middle_point - 1180);
		double center_low = rico_m2 * (double)(middle_point - 1225);
		
		// Make the check for correctness of point
		// Detection height has to be between the two border values for that corresponding center point
		double score = atof(current_elements[5].c_str());
		if ( (height >= center_low) && (height <= center_high) && (score > 0) ){
			// Process all data into a correct line and push it into a file
			stringstream ss;
			string filename = image_root_folder + "\\" + current_elements[0];
			cleaned_detections << filename << " " << current_elements[1] << " " << current_elements[2] << " " << current_elements[3] << " " << current_elements[4] << " " << current_elements[5] << endl;
		}
	}

	return 0;
}

