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

Software for performing the training of a cascade classifier based on AdaBoost.
Most code is from OpenCV official tool, but made some adaptations for output and bug fixes.

*****************************************************************************************************/

// ----------------------------------------------------------------------------------------------------------------------------------------------
// NEEDED DATA BEFORE THIS SOFTWARE CAN BE RAN
//	- vector file containing the series of positive training samples
//  - background samples file - negatives.txt - can be created with command dir /b /s > ../negatives.txt
//
// PARAMETERS NEEDED TO WORK CORRECTLY
// Usage of the cascade training algorithm: train_cascade_model.exe
//      -data		<location of where the classifier needs to be stored>
//      -vec		<vector file positive samples>
//      -bg			<file negative samples>
//      -numPos		<number of positive samples>
//      -numNeg		<number of negative samples>
//      -numStages	<number_of_stages - default 20>
//      	
//		-featureType	<HAAR - LBP - HOG>
//		-w				<same width value as during sample creation>
//		-h				<same height value as during sample creation>
//
//		-minHitRate			<suggested value = 0.995>
//		-maxFalseAlarmRate	<suggested value = 0.5>
//      
//      -maxDepth		<maximum depth of weak tree - 1 in case of stumps - too complex trees slow down>
//		-maxWeakCount	<amount of weak trees for each stage
//
// ONLY IN CASE OF HAAR LIKE WAVELET TRANSFORM
//		-baseFormatSave	<only add if HAAR-like old format is required>
//      -mode			<only in case of HAAR - BASIC is upright - ALL is upright and 45° rotated>
//
// ----------------------------------------------------------------------------------------------------------------------------------------------


#include "opencv2/core/core.hpp"
#include "opencv2/core/internal.hpp"

#include "cv.h"
#include "cascadeclassifier.h"

// --------------------------------------------------------------------------------------------------------------------------------
// STILL NEEDS TO BE CHECKED IF TRAINING SHOULD BE DONE AT SOME TIME!
// Check if thread building blocks are activated
#ifdef HAVE_TBB
	printf("TBB is used\n");
#endif
// --------------------------------------------------------------------------------------------------------------------------------

using namespace std;

int main( int argc, char* argv[] )
{
    CvCascadeClassifier classifier;
    String cascadeDirName, vecName, bgName;
    int numPos    = 0;
    int numNeg    = 0;
    int numStages = 20;
	// The more memory that is assigned here, the faster the training process will actually be. This is in MB
    int precalcValBufSize = 3072,
        precalcIdxBufSize = 3072;
    bool baseFormatSave = false;

    CvCascadeParams cascadeParams;
    CvCascadeBoostParams stageParams;
    Ptr<CvFeatureParams> featureParams[] = { Ptr<CvFeatureParams>(new CvHaarFeatureParams),
                                             Ptr<CvFeatureParams>(new CvLBPFeatureParams),
                                             Ptr<CvFeatureParams>(new CvHOGFeatureParams)
                                           };
    int fc = sizeof(featureParams)/sizeof(featureParams[0]);
    
	// Show how to adress the functionality needed
	if( argc == 1 )
    {
        cout << "Usage of the cascade training algorithm: " << endl;
        cout << "  -data <location of where the classifier needs to be stored>" << endl;
        cout << "  -vec <vector file positive samples>" << endl;
        cout << "  -bg <file negative samples>" << endl;
        cout << "  -numPos <number of positive samples>" << endl;
        cout << "  -numNeg <number of negative samples>" << endl;
        cout << "  -numStages <number_of_stages - default 20>" << endl;
        cout << "  [-baseFormatSave <only add if HAAR-like old format is required>]" << endl;
		cout << endl;
		
		cout << "  -featureType <HAAR - LBP - HOG>" << endl;
		cout << "  -w <same width value as during sample creation>" << endl;
		cout << "  -h <same height value as during sample creation>" << endl;
		cout << endl;

		cout << "  -minHitRate <suggested value = 0.995>" << endl;
		cout << "  -maxFalseAlarmRate <suggested value = 0.5>" << endl;
		cout << "  -maxDepth <1 creates stumps in binary trees>" << endl;
		cout << "  -maxWeakCount <suggested amount of weak classifiers per stage>" << endl;
		cout << endl;

        cout << "  [-mode <only in case of HAAR - BASIC is upright - ALL is upright and 45 degrees rotated>]" << endl;
		cout << endl;

		// Make sure that code doesn't crash if someone asks for information
		return 0;
    }

    for( int i = 1; i < argc; i++ )
    {
        bool set = false;
        if( !strcmp( argv[i], "-data" ) )
        {
            cascadeDirName = argv[++i];
        }
        else if( !strcmp( argv[i], "-vec" ) )
        {
            vecName = argv[++i];
        }
        else if( !strcmp( argv[i], "-bg" ) )
        {
            bgName = argv[++i];
        }
        else if( !strcmp( argv[i], "-numPos" ) )
        {
            numPos = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-numNeg" ) )
        {
            numNeg = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-numStages" ) )
        {
            numStages = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-precalcValBufSize" ) )
        {
            precalcValBufSize = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-precalcIdxBufSize" ) )
        {
            precalcIdxBufSize = atoi( argv[++i] );
        }
        else if( !strcmp( argv[i], "-baseFormatSave" ) )
        {
            baseFormatSave = true;
        }
        else if ( cascadeParams.scanAttr( argv[i], argv[i+1] ) ) { i++; }
        else if ( stageParams.scanAttr( argv[i], argv[i+1] ) ) { i++; }
        else if ( !set )
        {
            for( int fi = 0; fi < fc; fi++ )
            {
                set = featureParams[fi]->scanAttr(argv[i], argv[i+1]);
                if ( !set )
                {
                    i++;
                    break;
                }
            }
        }
    }

    classifier.train( cascadeDirName,
                      vecName,
                      bgName,
                      numPos, numNeg,
                      precalcValBufSize, precalcIdxBufSize,
                      numStages,
                      cascadeParams,
                      *featureParams[cascadeParams.featureType],
                      stageParams,
                      baseFormatSave );
    return 0;
}
