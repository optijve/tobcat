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

Adapted OpenCV software for creating an image sample vector for processing. Stitches together the 
high variating input images (different lighting, angles, sizes, ...) into a single vector file 
for processing.

We do not support the possibility to rotate, squish and flip the images randomly, since we are of
the meaning that this generates artificial cases, which do not occur in real world processes.

The outputted vector file can be used to feed to traincascade algorithm in order to create an 
desired object model based on the training data.

USE OF THE SOFTWARE THROUGH CMD LINE WITH THE FOLLOWING ARGUMENTS
This use assumes that creator of vector file includes all possible variance inside the training set himself
create_sample_vector.exe
	-info	<collection_file.txt>		gives the information of the positive input data as follows: location number x1 y1 width height ...
	-vec	<output_vector.vec>			name and path of the location of where to store output vector
	-w		<width>						width in pixels of the output samples in the vector
	-h		<height>					height in pixels of the output samples in the vector
	-show	<debug option>				when added, each sample will be inspected to see if it is processed correctly
	-num	<num positive samples>		as declared in the collection file

SUGGESTION: the OpenCV provided tool is not ideal. W & H are the parameters that the input gets
resized to for model creation. You should always keep it beneath 100 px for a dimension, in order
not to get out of memory during performance.

I reduced the original tool to only take into account the parameters preferred for our applications.

*****************************************************************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>

using namespace std;

#include "cvhaartraining.h"

int main( int argc, char* argv[] )
{
    int i = 0;
    char* nullname   = (char*)"(NULL)";
    char* vecname    = NULL; /* .vec file name */
    char* infoname   = NULL; /* file name with marked up image descriptions */
    char* imagename  = NULL; /* single sample image */
    char* bgfilename = NULL; /* background */
    int num = 1000;
    int bgcolor = 0;
    int bgthreshold = 80;
    int invert = 0;
    int maxintensitydev = 40;
    double maxxangle = 1.1;
    double maxyangle = 1.1;
    double maxzangle = 0.5;
    int showsamples = 0;
    
	// the samples are adjusted to this scale in the sample preview window */
    double scale = 1.0;
    int width  = 24;
    int height = 24;

    srand((unsigned int)time(0));

    if( argc == 1 )
    {
        printf( "Usage of vector creation after manual labeling has been provided: \n"  
				"  -info <collection_file_name with relative location - place in folder of createsamples executable!>\n"
				"  -vec	<output_vector name with complete location>\n"
				"  -show <add if you want to inspect each vector element>\n"
                "  -w <sample_width for output samples>\n"
				"  -h <sample_height for output samples>\n"
				"  -num <number of samples to train - amount detections in the vector>");

        return 0;
    }

    for( i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-info" ) )
        {
            infoname = argv[++i];
        }
        else if( !strcmp( argv[i], "-vec" ) )
        {
            vecname = argv[++i];
        }
        else if( !strcmp( argv[i], "-num" ) )
        {
            num = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-show" ) )
        {
            showsamples = 1;
            if( i+1 < argc && strlen( argv[i+1] ) > 0 && argv[i+1][0] != '-' )
            {
                double d;
                d = strtod( argv[i+1], 0 );
                if( d != -HUGE_VAL && d != HUGE_VAL && d > 0 ) scale = d;
                ++i;
            }
        }
        else if( !strcmp( argv[i], "-w" ) )
        {
            width = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-h" ) )
        {
            height = atoi( argv[++i] );
        }
    }

    printf( "Info file name: %s\n", ((infoname == NULL) ?   nullname : infoname ) );
    printf( "Vec file name: %s\n",  ((vecname == NULL) ?    nullname : vecname ) );
    printf( "Number of positive training samples: %d\n", num );
    printf( "Show samples: %s\n", (showsamples) ? "TRUE" : "FALSE" );
    printf( "Width: %d\n", width );
    printf( "Height: %d\n", height );

   if( infoname && vecname )
    {
        int total;

        printf( "Create training samples from images collection...\n" );

        total = cvCreateTrainingSamplesFromInfo( infoname, vecname, num, showsamples,
                                                 width, height );

        printf( "Done. Created %d samples\n", total );
    }

    return 0;
}


