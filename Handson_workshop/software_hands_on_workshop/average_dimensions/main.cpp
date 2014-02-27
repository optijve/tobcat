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

This software allows to define average w and h parameters from your training data

*****************************************************************************************************/

// Includes for standard file input output functionality
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <stdlib.h>

using namespace std;

int main( int argc, const char** argv )
{
    // Check if arguments are given correct
	if( argc == 1 && argc != 2 ){
		cout << "Usage of average dimension retrieval software;" << endl;
		cout <<	"average_dimensions.exe <annotion_file.txt>" << endl;
		return 0;
	}

	// Read in the file using a ifstream
	ifstream input ( argv[1] );
	string current_line;
	vector<int> w_dimensions;
	vector<int> h_dimensions;

	while ( getline(input, current_line) ){
	    // A complete line is read, now check how many detections happened
	    // First split the string into elements
        vector<string> line_elements;
        stringstream temp (current_line);
        string item;
        while ( getline(temp, item, ' ') ) {
            line_elements.push_back(item);
        }

        // Now use the elements to grab all w and h values
        int number_rectangles = atoi( line_elements[1].c_str() );
        for (int i = 0; i < number_rectangles; i++){
            w_dimensions.push_back( atoi( line_elements[(i*4)+4].c_str() ) );
            h_dimensions.push_back( atoi( line_elements[(i*4)+5].c_str() ) );
        }
    }
    input.close();

	// Calculate the average value of the vectors
    int sum_w = 0, sum_h = 0;
    for (int i = 0; i < w_dimensions.size(); i++){
        sum_w += w_dimensions[i];
        sum_h += h_dimensions[i];
    }
    int average_w = sum_w / w_dimensions.size();
    int average_h = sum_h / h_dimensions.size();

    // Output the result
    cout << "The average dimensions are: w = " << average_w << " and h = " << average_h << "." << endl;
    system("wait");

    return 0;
}
