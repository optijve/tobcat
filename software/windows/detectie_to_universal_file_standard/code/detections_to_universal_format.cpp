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

Software for switching between data format --> detections to annotations

*****************************************************************************************************/

// Standard Windows include header
#include "stdafx.h"

// Includes for standard functionality
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// Includes for namespaces
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	// Standard interface for executing the program
	if( argc == 1 ){
		printf( "Application for converting the detector output (cleaned or original) into a general annotation file.\n"
				"It helps to combine multiple detection on 1 input in a single line.\n"
				"--> detections_to_universal_format.exe <detections_output.txt> <annotations.txt> <OPTIONAL image root folder>\n");
		return 0;
	}

	// Parameters
	string input_file = argv[1];
	string output_file = argv[2];
	string root_folder;
	// Check if the third parameter is supplied
	if ( argv[3] ){
		root_folder = argv[3];
	}
	
	ifstream in (input_file);
	ofstream out (output_file);
	
	string current_line;
	string filename = "";

	stringstream temp_output;

	int counter_detections = 0;
	bool start = false;

	// Read in the original file rule by rule
	// Process all data for a single image
	// Write the result into the new file

	// Remove the first line, which contains headers
	getline(in, current_line);
	while ( getline(in, current_line) ){
		// Split the line into parameters
		string element;
		vector<string> line_elements;
		stringstream temp_stream( current_line );
		while( getline(temp_stream, element,'\t') ){
			line_elements.push_back(element);
		}

		// Check if the filename is the existing filename
		// If yes, then continue
		// If no, then add the filename to the output stringstream
		if( !(strcmp(filename.c_str(), line_elements[0].c_str()) == 0) ){
			if (start){
				out << counter_detections << " " << temp_output.str() << endl;
				temp_output.str("");
				counter_detections = 0;
			}
			filename = line_elements[0];
			
			if ( argv[3] ){
				out << root_folder << filename << " ";
			}else{
				out << filename << " ";
			}
			
			temp_output << line_elements[1] << " " << line_elements[2] << " " << line_elements[3] << " " << line_elements[4] << " ";
			counter_detections++;
			start = true;
		}else{
			temp_output << line_elements[1] << " " << line_elements[2] << " " << line_elements[3] << " " << line_elements[4] << " ";
			counter_detections++;
		}
	}

	// Make sure that last buffer is outputted also
	out << counter_detections << " " << temp_output.str() << endl;

	return 0;
}

