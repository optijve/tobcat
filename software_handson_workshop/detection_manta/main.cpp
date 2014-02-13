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

This software allows people to perform cascade classification on an AVT MANTA GigE Camera
Some preparations for this to work
1) Add the PvAPI header file to the project source code folder
2) Add the correct libraries in the linker options, in the following order [does matter!]
    - /home/steven/Documents/AVT_sdk/lib-pc/x64/4.5/libPvAPI.a
    - /usr/lib/x86_64-linux-gnu/librt.a
    - /usr/lib/x86_64-linux-gnu/libpthread.a
3) Make sure the openCV libs are included with an extra linker command
   `pkg-config opencv --libs`
4) Make sure your ethernet interface in linux has jumbo packaging enabled using the following command
    - sudo ifconfig [eth0] mtu 9000
    - check the correct adapter using ifconfig command

*****************************************************************************************************/
// Link to the AVT technologies camera library for AVT Manta
#include "PvApi.h"

// Link to the necessary OpenCV libraries
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

// Functionality to provide extra output
#include <string.h>
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;

// camera's data type definition
typedef struct
{
    unsigned long   UID;
    tPvHandle       Handle;
    tPvFrame        Frame;

} tCamera;

#define CH_MONO 1	// Single channel for mono images

int main(int argc, char* argv[])
{
	tCamera			myCamera;
	tPvCameraInfo	cameraInfo;
	unsigned long	frameSize;
	tPvErr			Errcode;

	int counter = 0;

	if( argc == 1 ){
		cout << "This script will stream data from an AVT MANTA GigE camera using OpenCV C++ style interface." << endl;
        cout << "capture_manta.exe <resolution width> <resolution heigth> <detection_model.xml>" << endl;
		return 0;
	}

	// Be sure to move the windows to correct location
	namedWindow("Detection result",1); moveWindow("View window", 50, 50);

	// Recover the detection details
	string detection_model ( argv[3] );
	CascadeClassifier detector ( detection_model );

	// Initialize the API
	if(!PvInitialize())
	{
		// Wait for the response from a camera after the initialization of the driver
		// This is done by checking if camera's are found yet
		////////////////////////////////////////////////////////////
		while(PvCameraCount() == 0)
		{
			waitKey(15);
		}

		//debug
		cout << "Camera count successfully finished." << endl;

		/////////////////////////////////////////////////////////////
		if ( PvCameraList(&cameraInfo, 1, NULL) == 1)
		{
			// Get the camera ID
			myCamera.UID = cameraInfo.UniqueId;

			// Open the camera
			if( !PvCameraOpen(myCamera.UID, ePvAccessMaster, &(myCamera.Handle)) )
			{
				//debug
				cout << "Camera opened succesfully." << endl;

				// Get the image size of every capture
				PvAttrUint32Get(myCamera.Handle, "TotalBytesPerFrame", &frameSize);

				// Allocate a buffer to store the image
				memset(&myCamera.Frame, 0, sizeof(tPvFrame));
				myCamera.Frame.ImageBufferSize = frameSize;
				myCamera.Frame.ImageBuffer = new char[frameSize];

				// Set maximum camera parameters - camera specific
				int max_width = 1624;
				int max_heigth = 1234;

				int center_x = max_width / 2;
				int center_y = max_heigth / 2;

				int frame_width = atoi(argv[1]);
				int frame_heigth = atoi(argv[2]);

				// Set the manta camera parameters to get wanted frame size retrieved
				PvAttrUint32Set(myCamera.Handle, "RegionX", center_x - (frame_width / 2) );
				PvAttrUint32Set(myCamera.Handle, "RegionY", center_y - (frame_heigth / 2));
				PvAttrUint32Set(myCamera.Handle, "Width", frame_width);
				PvAttrUint32Set(myCamera.Handle, "Height", frame_heigth);

				// Start the camera
				PvCaptureStart(myCamera.Handle);

				// Set the camera to capture continuously
				PvAttrEnumSet(myCamera.Handle, "AcquisitionMode", "Continuous");
				PvCommandRun(myCamera.Handle, "AcquisitionStart");
				PvAttrEnumSet(myCamera.Handle, "FrameStartTriggerMode", "Freerun");

				// Create infinite loop - break out when condition is met
				for(;;)
				{
					if(!PvCaptureQueueFrame(myCamera.Handle, &(myCamera.Frame), NULL))
					{
						while(PvCaptureWaitForFrameDone(myCamera.Handle, &(myCamera.Frame), 100) == ePvErrTimeout)
						{
						}

						////////////////////////////////////////////////////////
						// Here comes the OpenCV functionality for each frame //
						////////////////////////////////////////////////////////

						// Create an image header (mono image)
						// Push ImageBuffer data into the image matrix
						Mat image = Mat(frame_heigth, frame_width, CV_8UC1);
						image.data = (uchar *)myCamera.Frame.ImageBuffer;

                        // Create color image for visualization
                        Mat image_color( image.rows, image.cols, CV_8UC3 );
                        Mat in[] = { image, image, image };
                        int from_to[] = { 0,0, 1,1, 2,2 };
                        mixChannels( in, 3, &image_color, 1, from_to, 3 );

                        // Perform detection
                        vector<Rect> objects;
                        equalizeHist( image, image );
                        detector.detectMultiScale(image, objects, 1.05, 3, 0, Size(93,43), Size(140,64));

                        // Visualize detections on the input frame and show in the given window
                        for( size_t i = 0; i < objects.size(); i++ )
                        {
                            // Use a rectangle representation on the frame
                            // Frame width 3 pixels in color red (BGR format)
                            rectangle(image_color, Point(objects[i].x, objects[i].y), Point(objects[i].x + objects[i].width, objects[i].y + objects[i].height), Scalar(0, 0, 255), 2);
                        }

						// Show the actual frame
						imshow("Detection result", image_color);

						// Now wait for a keystroke and respond to it
                        int key = waitKey(5);
                        if (key == 27){
                            cout << "Closed down the application by pressing ESC." << endl;

                            // Release image
                            image.release();

                            break;
                        }

                        // Release images
                        image.release();

					}
				}

				// Stop the acquisition & free the camera
				Errcode = PvCommandRun(myCamera.Handle, "AcquisitionStop");
				if (Errcode != ePvErrSuccess)
					throw Errcode;

				PvCaptureEnd(myCamera.Handle);
				PvCameraClose(myCamera.Handle);

				cout << endl << "finished" << endl;
			}
			else
				cout << "open camera error" << endl;
		}
		else
			cout << "camera not found" << endl;
	}
	else
		cout << "failed to initialise the API" << endl;

    return 0;
}
