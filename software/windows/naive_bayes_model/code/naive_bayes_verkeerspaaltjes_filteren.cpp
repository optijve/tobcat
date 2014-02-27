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

Software for cleaning detections based on CMY properties of detection window using 
a naive bayes classifier approach.

*****************************************************************************************************/

// Standard Windows include header
#include "stdafx.h"

// Includes for openCV functionality, requires openCV 2.4.5 or higher!
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"

// Includes for standard file input output functionality
#include <fstream>

// Includes for namespaces
using namespace std;
using namespace cv;

// FUNCTION: Convert an input matrix into seperate channels
vector<Mat> to_channels(Mat input_matrix){
	vector<Mat> channels;
	split(input_matrix, channels);
	return channels;
}

// FUNCTION: Combine three channels into a single matrix
Mat channels_to_matrix(Mat channel1, Mat channel2, Mat channel3){
	Mat output(channel1.rows, channel1.cols, CV_64FC3);
	Mat in[] = { channel1, channel2, channel3 };
	int from_to[] = { 0,0, 1,1, 2,2 };
	mixChannels( in, 3, &output, 1, from_to, 3 );
	return output;
}

// FUNCTION = Convert BGR image to CMY image since this is not standard in openCV
// Remember that OpenCV uses BGR color channels instead of RGB color channels
Mat bgr_to_cmy(Mat input_image){	
	vector<Mat> color_channels;
	color_channels = to_channels(input_image);

	int rows = input_image.rows;
	int cols = input_image.cols;

	// Convert the channels towards the CMYK space - only the CMY channels are needed
	// Convert the ranges from 0 - 255 to range 0 - 1 (which needs doubles first)
	color_channels[0].convertTo(color_channels[0], CV_64F); color_channels[0] = color_channels[0] / 255.0;
	color_channels[1].convertTo(color_channels[1], CV_64F); color_channels[1] = color_channels[1] / 255.0;
	color_channels[2].convertTo(color_channels[2], CV_64F); color_channels[2] = color_channels[2] / 255.0;
	
	// Calculate the C M Y channels based on de B G R channels
	Mat c_channel = Mat(rows, cols, CV_64F);
	Mat m_channel = Mat(rows, cols, CV_64F);
	Mat y_channel = Mat(rows, cols, CV_64F);

	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			c_channel.at<double>(i,j) = (1.0 - color_channels[2].at<double>(i,j));
			m_channel.at<double>(i,j) = (1.0 - color_channels[1].at<double>(i,j));
			y_channel.at<double>(i,j) = (1.0 - color_channels[0].at<double>(i,j));
		}
	}

	// Place the 3 channels together as an CMY image and return it
	return channels_to_matrix(c_channel, m_channel, y_channel);
}

// Create feature representation that is used by the naive bayes approach
Mat create_feature_cmy(Mat input){
	vector<Mat> channels = to_channels(input);
	Mat feature(1,3,CV_64F);

	// Create region of interest - region of which we want to extract the feature for the classifier
	// Verkeerspaaltje = Region (2/3 from bottom, 1/3 to 2/3 from left to right)
	int x = channels[0].cols / 3;
	int y = channels[0].rows / 3;
	int width = channels[0].cols / 3;
	int height = (channels[0].rows / 3) * 2;
	Rect roi = Rect(x, y, width, height);

	Mat c_roi = channels[0](roi);
	Mat m_roi = channels[1](roi);
	Mat y_roi = channels[2](roi);

	// Calculate average C M Y value in the feature region
	Scalar mean_values_c = mean(c_roi);
	Scalar mean_values_m = mean(m_roi);
	Scalar mean_values_y = mean(y_roi);

	double mean_c = mean_values_c[0];
	double mean_m = mean_values_m[0];
	double mean_y = mean_values_y[0];

	feature.at<double>(0,0) = mean_c;
	feature.at<double>(0,1) = mean_m;
	feature.at<double>(0,2) = mean_y;

	return feature;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Local variables needed for processing
	CvNormalBayesClassifier classifier;
	string read_string;
	string location_model = "C:/data/traffic_signs/naive_bayes_verkeerspaaltjes.xml";

	// Read in file, in order to be able to segment out the wrong ones
	if( argc == 1 ){
		printf( "Usage: \n"
				"Application reads in annotions, applies for each detection the naive bayes classifier and generates a cleaned output.\n"
				"--> naive_bayes_verkeerspaaltjes.exe <annotations.txt> <cleaned_annotations.txt>\n");
		return 0;
	}
	
	string input_annotations = argv[1];
	string output_annotations = argv[2];
		
	// Open input stream for reading annotations file
	ifstream input; 
	input.open(input_annotations);
	if (!input.is_open()){
		cout << "Input file could not be opened!";
    }

	// Open output stream for writing the cleaned annotations data
	ofstream output; 
	output.open(output_annotations);
	if (!output.is_open()){
		cout << "Output file could not be opened!";
    }
	
	// Open a stored Naive Bayes Classifier model - supplied at download section of http://www.eavise.be/tobcat/
	// The example model whas produced using 10 positive and 10 negative 'verkeerspaal' images
	classifier.load(location_model.c_str());
	
	// Perform classifier on data based on annotations file
	while(!input.eof()){
		// Read in the first line of annotations
		getline(input, read_string, '\n');
		if (read_string == ""){
			// If string is empty, then an error occured and processing should be stopped
			cout << "Reading of annotation file failed, please check the file content.";
			break; 
		}
		
		// Split the string into different annotation parameters
		// PARAMETERS: filename - number of detections - x1 - y1 - width1 - height1 - ... - xN - yN - widthN - heightN
		vector<string> data_input;
		string element;
		stringstream tempStream(read_string);
		// Data in annotations file is split by a space - could be any other delimiter based on file creation
		while(getline(tempStream, element, ' ')){
			data_input.push_back (element);
		}

		// Read in the original image, based on the annotation data
		Mat input_img = imread(data_input[0]);

		// Look how many detection regions there are for this specific input image
		int amount = atoi(data_input[1].c_str());
		int counter = 0;
		for(int i = 0; i < amount; i ++){
			int x = atoi(data_input[2 + counter].c_str());
			int y = atoi(data_input[3 + counter].c_str());
			int w = atoi(data_input[4 + counter].c_str());
			int h = atoi(data_input[5 + counter].c_str());

			Rect roi_detection(x, y, w, h);
			Mat image_detection = input_img( roi_detection );

			// Process the image and generate the feature vector for classification
			Mat image_detection_cmy = bgr_to_cmy(image_detection);
			Mat feature = create_feature_cmy(image_detection_cmy);
			
			// Perform the classification
			// The predict function requires the matrix to be of type CV_32F, so the conversion need to be done explictly
			Mat result;
			feature.convertTo(feature, CV_32FC1);
			classifier.predict(feature, &result);
			double prediction = result.at<float>(0,0);

			// Process the prediction result of the classifier
			// 0 = no traffic sign / 1 = traffic sign
			if (!prediction){
				// This means that the annotation is not selected as traffic sign
				// Add the correct output line to the output file
				if(i == 0){
					// If it is the first entry, then add the name of the file location
					output << data_input[0] << " " << x << " " << y << " " << w << " " << h;
				}else{
					output << " " << x << " " << y << " " << w << " " << h;
				}
			}
			// If it was predicted as a traffic sign, then the detection is removed from the annotation file

			// Push up counter with 4 steps, since each annotation contains 4 elements
			counter = counter + 4;

			// If we are at last detection, also add a newline command, so that new entry comes at correst place
			if (i == amount - 1){
				output << "\n";
			}
		}

	}
	
	return 0;
}

