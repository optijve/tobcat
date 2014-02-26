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

int main( int argc, const char** argv )
{
	// Global variables
    CascadeClassifier cascade;
    string window_name = "Cascade classifier object detector";

	// Check if arguments are given correct
	if( argc == 1 ){
		cout << "Usage of model detection software: " << endl;
		cout <<	"detect_objects.exe <object_model.xml> <test_images.txt> <detection_result.txt> <overlap_threshold>" << endl;
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
    input.close();

    // Retrieve the amount of overlap
    int threshold = atoi(argv[4]);

    // Create an output file for storing detections
	string location_output = argv[3];
	ofstream output_file (location_output.c_str());

    // Loop over each image in the test image sequence and perform detection
    for(int i = 0; i < (int)filenames.size(); i++){
        // Read in a single frame
        // TODO
        //    - loop over filenames
        //    - use iterator to load first image --> current_frame
        // FUNCTIONS imread

        // ------------------------------------------------------------------------
        // PERFORM THE ACTUAL DETECTION
        // ------------------------------------------------------------------------

        // Specific variables for the actual detection phase
        // TODO
        //    - create a container objects for detections stored as rectangles
        //    - create an empty image container (frame_gray/frame_equalize) for working on the data
        // FUNCTIONS vector<type> / Mat

        // Convert the input frame to grayscale image and apply lightning normalization using histogram equilization
        // TODO
        //    - convert image input to grayscale
        //    - account for lighting variance --> histogram equilization
        // FUNCTIONS cvtColor / equalizeHist

        // Detect object in a given image
        // Parameters should be checked at : http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html?highlight=detectmultiscale#void%20CascadeClassifier::detectMultiScale%28const%20Mat&%20image,%20vector%3CRect%3E&%20objects,%20double%20scaleFactor,%20int%20minNeighbors,%20int%20flags,%20Size%20minSize,%20Size%20maxSize%29
        // TODO
        //    - use the global created cascade to perform a multiscale detection
        //    - keep in mind we want a scale step of 5% and all possible detections
        //    - try making the result more usable after the first vizualisation [nmbr overlaps / min scale / max scale / ...]
        // FUNCTIONS detectMultiScale
        // (const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())

        // ------------------------------------------------------------------------
        // VISUALIZE THE ACTUAL DETECTION
        // ------------------------------------------------------------------------

        // Visualize detections on the input frame and show in the given window
        for( int j = 0; j < (int)objects.size(); j++ )
        {
            // TODO
            //   - Visualize each detection by drawing a rectangle
            //   - Line thickness = 3 pixels / color = red (BGR format) - Scalar(B,G,R)
            // FUNCTIONS rectangle
            // (Mat& img, Rect bounding, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
        }

        // TODO
        //   - Show the resulting rectangles on the image
        // FUNCTIONS imshow, waitKey

        // ------------------------------------------------------------------------
        // SAVE THE DETECTION RESULTS
        // Universal format used in TOBCAT project
        // filename #detections x1 y1 w1 h1 x2 y2 w2 h2 ... xN yN wN hN
        // ------------------------------------------------------------------------

        output_file << filenames[i];
        output_file << " " << objects.size();
        for(int i = 0; i < (int)objects.size(); i++){
            output_file << " " << objects[i].x << " " << objects[i].y << " " << objects[i].width << " " << objects[i].height;
        }
        output_file << endl;
    }

	output_file.close();

	return 0;
}
