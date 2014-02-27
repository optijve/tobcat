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

*****************************************************************************************************/

// includes needed for reading in files correctly and creating output
#include <fstream>
#include <iostream>

// OpenCV includes needed for correct functionality to work
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Including the used namespaces
using namespace std;
using namespace cv;

// Global variables
CascadeClassifier cascade;
string window_name = "Cascade classifier object detector";

int main( int argc, const char** argv )
{
	// Check if arguments are given correct
	if( argc == 1 ){
		cout << "Usage of model detection software: " << endl;
		cout <<	"detect_objects.exe <object_model.xml> <test_images.txt> <detection_result.txt>" << endl;
		return 0;
	}

	// Load the cascade model into the model container
	string model_name = argv[1];
	if( !cascade.load( model_name ) ){
		cout << "Error loading the trained model from the provided model file! " << endl;
		return -1;
	};

	// Retrieve the filenames of all the test images
	string test_images = argv[2];
	ifstream input (test_images.c_str());
    string current_line;
    vector<string> filenames;
    while ( getline(input, current_line) ){
        vector<string> line_elements;
        stringstream temp (current_line);
        string first_element;
        getline(temp, first_element, ' ');
        filenames.push_back(first_element);
    }
    int number_input_samples = filenames.size();
    input.close();

    // Create an output file for storing detections
	string location_output = argv[3];
	ofstream output_file (location_output.c_str());

    // Loop over each image in the test image sequence and perform detection
    for(int i = 0; i < filenames.size(); i++){
        // Read in the first image
        Mat current_frame = imread(filenames[i]);

        // ------------------------------------------------------------------------
        // PERFORM THE ACTUAL DETECTION
        // ------------------------------------------------------------------------

        // Specific variables for the actual detection phase
        vector<Rect> objects;
        Mat frame_gray;

        // Convert the input frame to grayscale image and apply lightning normalization using histogram equilization
        cvtColor( current_frame, frame_gray, CV_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );

        // Detect object in a given image
        // Parameters should be checked at : http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html?highlight=detectmultiscale#void%20CascadeClassifier::detectMultiScale%28const%20Mat&%20image,%20vector%3CRect%3E&%20objects,%20double%20scaleFactor,%20int%20minNeighbors,%20int%20flags,%20Size%20minSize,%20Size%20maxSize%29
        // Below command would detect and visualize all single detections
        //cascade.detectMultiScale( frame_gray, objects, 1.05, 0, 0);

        // Below command would detect and visualize all detections that have 5 or more matching overlaps
        cascade.detectMultiScale( frame_gray, objects, 1.05, 5, 0);

        // Below command would detect and visualize all detections that have 5 or more matching overlaps and specify a maximum and minimal object size
        //cascade.detectMultiScale( interested_region, objects, 1.05, 5, 0, Size(10, 25), Size(100, 250));

        // ------------------------------------------------------------------------
        // VISUALIZE THE ACTUAL DETECTION
        // ------------------------------------------------------------------------

        // Visualize detections on the input frame and show in the given window
        for( int j = 0; j < objects.size(); j++ )
        {
            // Use a rectangle representation on the frame
            // Frame width 3 pixels in color red (BGR format)
            rectangle(current_frame, Point(objects[j].x, objects[j].y), Point(objects[j].x + objects[j].width, objects[j].y + objects[j].height), Scalar(0, 0, 255), 1);
        }

        // Show the result
        imshow( window_name, current_frame ); waitKey(0);

        // ------------------------------------------------------------------------
        // SAVE THE DETECTION RESULTS
        // Universal format
        // filename #detections x1 y1 w1 h1 x2 y2 w2 h2 ... xN yN wN hN
        // ------------------------------------------------------------------------

        output_file << filenames[i];
        output_file << " " << objects.size();
        for(int i = 0; i < objects.size(); i++){
            output_file << " " << objects[i].x << " " << objects[i].y << " " << objects[i].width << " " << objects[i].height;
        }
        output_file << endl;
    }

	output_file.close();

	return 0;
}
