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

This software allows to quickly sum up all files in a specific folder and outputs it to a text file

*****************************************************************************************************/

// Includes for standard output functionality
#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace std;

int main( int argc, const char** argv )
{
    // Check if arguments are given correct
	if( argc == 1 && argc != 3 ){
		cout << "Usage of folder listing software:" << endl;
		cout <<	"folder_listing.exe <folder> <output.txt>" << endl;
		return 0;
	}

	// Read in the given arguments
	string folder_location = argv[1];
	string output_location = argv[2];

	// Create the batch command
	stringstream buffer;
	buffer << "find " << folder_location << " -type f > " << output_location;

    // Execute the batch command
    system( buffer.str().c_str() );

    return 0;
}
