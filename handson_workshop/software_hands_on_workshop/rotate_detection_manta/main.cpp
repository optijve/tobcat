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

This software allows people to perform full rational cascade classification on an AVT MANTA GigE Camera
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

// Return the rotation matrices for each rotation
void rotate(Mat& src, double angle, Mat& dst)
{
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}

int main(int argc, char* argv[])
{
	tCamera			myCamera;
	tPvCameraInfo	cameraInfo;
	unsigned long	frameSize;
	tPvErr			Errcode;

	if( argc == 1 ){
		cout << "This script will stream data from an AVT MANTA GigE camera using OpenCV C++ style interface." << endl;
        cout << "capture_manta.exe <resolution width> <resolution heigth> <detection_model.xml> <angle_range> <angle_step>" << endl;
		return 0;
	}

	// Be sure to move the windows to correct location
	namedWindow("Detection result",1); moveWindow("View window", 50, 50);

	// Recover the detection details
	string detection_model ( argv[3] );
	CascadeClassifier detector ( detection_model );
	int angle_range = atoi( argv[4] );
	int angle_step = atoi( argv[5] );

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

                        // Before detection, rotate the image of the angle step and append it to an image list.
                        int number_of_steps = angle_range / angle_step;
                        vector<Mat> rotated_matrices;
                        vector<Mat> all_rotation_matrices;
                        for ( int i = 0; i < number_of_steps; i ++ ){
                            // Rotate the image
                            Mat rotated = image.clone();
                            rotate(image, (i+1)*angle_step, rotated);
                            // Preprocess the images already, since we will need it
                            equalizeHist( rotated, rotated );
                            // Add to the collection of rotated images
                            rotated_matrices.push_back(rotated);
                        }

                        // Create color image for visualization
                        Mat image_color( image.rows, image.cols, CV_8UC3 );
                        Mat in[] = { image, image, image };
                        int from_to[] = { 0,0, 1,1, 2,2 };
                        mixChannels( in, 3, &image_color, 1, from_to, 3 );

                        // Perform detection on each image seperately
                        // Store all detection in a vector of vector detections
                        vector< vector<Rect> > objects;
                        for (int i = 0; i < (int)rotated_matrices.size(); i++){
                            vector<Rect> current_objects;
                            detector.detectMultiScale(rotated_matrices[i], current_objects, 1.05, 20, 0, Size(93,43), Size(140,64));
                            objects.push_back(current_objects);
                        }

                        // Visualize detections on the input frame and show in the given window
                        // Select a color for each orientation
                        RNG rng(12345);
                        for( int i = 0; i < (int)objects.size(); i++ ){
                            // Visualize detections in the first image
                            //int i = 8;
                            vector<Rect> temp = objects[i];
                            Scalar color(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
                            for( int j = 0; j < (int)temp.size(); j++ )
                            {
                                    // Use a rectangle representation on the frame but warp back the coordinates
                                    // Retrieve the 4 corners detected in the rotation image
                                    Point p1 ( temp[j].x, temp[j].y ); // Top left
                                    Point p2 ( (temp[j].x + temp[j].width), temp[j].y ); // Top right
                                    Point p3 ( (temp[j].x + temp[j].width), (temp[j].y + temp[j].height) ); // Down right
                                    Point p4 ( temp[j].x, (temp[j].y + temp[j].height) ); // Down left

                                    // Add the 4 points to a matrix structure
                                    Mat coordinates = (Mat_<double>(3,4) << p1.x, p2.x, p3.x, p4.x,\
                                                                            p1.y, p2.y, p3.y, p4.y,\
                                                                            1   , 1  ,  1   , 1    );

                                    // Apply a new inverse tranformation matrix
                                    Point2f pt(image.cols/2., image.rows/2.);
                                    Mat r = getRotationMatrix2D(pt, -(angle_step*(i+1)), 1.0);
                                    Mat result = r * coordinates;

                                    // Retrieve the ew coordinates from the tranformed matrix
                                    Point p1_back, p2_back, p3_back, p4_back;
                                    p1_back.x=(int)result.at<double>(0,0);
                                    p1_back.y=(int)result.at<double>(1,0);

                                    p2_back.x=(int)result.at<double>(0,1);
                                    p2_back.y=(int)result.at<double>(1,1);

                                    p3_back.x=(int)result.at<double>(0,2);
                                    p3_back.y=(int)result.at<double>(1,2);

                                    p4_back.x=(int)result.at<double>(0,3);
                                    p4_back.y=(int)result.at<double>(1,3);

                                    // Draw a rotated rectangle by lines, using the reverse warped points
                                    line(image_color, p1_back, p2_back, color, 2);
                                    line(image_color, p2_back, p3_back, color, 2);
                                    line(image_color, p3_back, p4_back, color, 2);
                                    line(image_color, p4_back, p1_back, color, 2);
                                }
                        }

						// Show the actual frame
						imshow("Detection result", image_color);

						// Now wait for a keystroke and respond to it
                        int key = waitKey(15);
                        if (key == 27){
                            cout << "Closed down the application by pressing ESC." << endl;

                            // Release image
                            image.release();

                            break;
                        }
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
