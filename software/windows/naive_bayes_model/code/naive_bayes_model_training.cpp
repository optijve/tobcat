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

Software for training of a naive bayes model based on the openCV source code.

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
#include <windows.h>
#include <strsafe.h>

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

// FUNCTION : Create a complete feature matrix as input for classifier based on a vector of features
Mat convert_input_vector(vector<Mat> features){
	// Retrieve dimensions for the output matrix
	int rows = (int)features.size();
	Mat first_element = features[0];
	int cols = first_element.cols;

	// Create output matrix
	// Each feature represented as a row in the final matrix
	Mat result(rows,cols, first_element.type());

	// Go throuhg input and copy it to the result matrix
	for(int i = 0; i < rows; i++){
		Mat feature_loaded = features[i];
		feature_loaded.convertTo(feature_loaded,CV_64F);
		for(int j = 0; j < cols; j ++){
			result.at<double>(i,j) = feature_loaded.at<double>(0,j);
		}
	}

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// General training of a naive bayes model - trafic sign model training in this specific case
	// A model is trained by supplying positive and negative examples and letting the classifier decide how to seperate them
	// For this training, a feature vector of each sample and an output needs to be supplied to the algoritm

	// Read in file, in order to be able to segment out the wrong ones
	if( argc == 1 ){
		printf( "Usage: Application trains a naive bayes classifier on positive and negative samples.\n"
			    "For the moment parameters and feature construction are specified in the code.\n"
				"--> naive_bayes_model_training.exe\n");
		return 0;
	}

	// Parameters - comment out
	string main_folder = "C:/data/traffic_signs/";
	string positive_folder = main_folder + "positives/";
	string negative_folder = main_folder + "negatives/";
	vector<Mat> input_elements, input_elements_cmy, features;
	CvNormalBayesClassifier classifier;

	// Read in the positive and negative training samples and put them into a specific vector
	// C/C++ doesn't provide a standard file reading system,  this is WINDOWS based and will not work under Linux/MacOsx/Android/... !
	WIN32_FIND_DATA ffd;
	TCHAR szDir_positives[MAX_PATH], szDir_negatives[MAX_PATH];
	vector<string> filenames_positives, filenames_negatives;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	StringCchCopy(szDir_positives, MAX_PATH, positive_folder.c_str());
	StringCchCat(szDir_positives, MAX_PATH, TEXT("*"));

	StringCchCopy(szDir_negatives, MAX_PATH, negative_folder.c_str());
	StringCchCat(szDir_negatives, MAX_PATH, TEXT("*"));

	// Process data from the positives folder, at the end remove the current and parent folder references in the listing
	hFind = FindFirstFile(szDir_positives, &ffd);
	do
	{
		filenames_positives.push_back (ffd.cFileName);	
	}
	while (FindNextFile(hFind, &ffd) != 0);
	filenames_positives.erase(filenames_positives.begin(),filenames_positives.begin()+2);
	FindClose( hFind );

	// Process data from the negatives folder, at the end remove the current and parent folder references in the listing
	hFind = FindFirstFile(szDir_negatives, &ffd);
	do
	{
		filenames_negatives.push_back (ffd.cFileName);	
	}
	while (FindNextFile(hFind, &ffd) != 0);
	filenames_negatives.erase(filenames_negatives.begin(),filenames_negatives.begin()+2);
	FindClose( hFind );

	// Based on the filenames, read in the data and put into the matrix vector
	for(int i = 0; i < filenames_positives.size(); i++){
		Mat temp = imread(filenames_positives[i]);
		input_elements.push_back(temp);
	}
	for(int i = 0; i < filenames_negatives.size(); i++){
		Mat temp = imread(filenames_negatives[i]);
		input_elements.push_back(temp);
	}

	// Switch inputs from bgr to cmy color space
	for(int i = 0; i < 10; i++){
		Mat temp = bgr_to_cmy(input_elements[i]);
		input_elements_cmy.push_back(temp);
	}
	
	// Pick the yellow channel of each sample, create a feature representation of it and return to the naïve bayes classifier
	for(int i = 0; i < input_elements.size(); i++){
		Mat temp_feature = create_feature_cmy(input_elements_cmy[i]);
		features.push_back(temp_feature);
	}

	// Create feature matrix based on retrieved vector with the single features
	Mat input_features = convert_input_vector(features);
	// Create according feature label matrix
	Mat label_features(10,1,CV_8UC1);
	for(int i = 0; i < filenames_positives.size(); i++){
		label_features.push_back(1);
	}
	for(int i = 0; i < filenames_negatives.size(); i++){
		label_features.push_back(0);
	}

	// Train the normal bayes classifier model and store in into an xml file so that it can be reused
	// Training data and labels need to be of the CV_32FC1 format!
	input_features.convertTo(input_features, CV_32FC1);
	label_features.convertTo(label_features, CV_32FC1);
	classifier.train(input_features, label_features);

	// Save the normal bayes classifer model
	string output_location = main_folder + "naive_bayes_traffic_signs.xml";
	classifier.save(main_folder.c_str());

	return 0;
}

