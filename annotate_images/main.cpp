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

This is a unix version of the GUI interface for annotation.
This software allows people to import images and manually annotate regions of interest.

*****************************************************************************************************/

// Includes for openCV functionality, requires openCV 2.4.5 or higher --> uses new C++ functionality
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Includes for standard file input output functionality
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

// Public parameters that are needed to share data between functions correctly
// Some of them are required to enforce a continuous redrawing on mouse actions
Mat image, image_part;

int roi_x0 = 0, roi_y0 = 0, roi_x1 = 0, roi_y1 = 0, num_of_rec = 0;
bool start_draw = false, segment = false;

const string window_name="<NUMPAD DEC> add ROI <NUMPAD ENTER> save/next";

string root_folder = "";

// FUNCTION : Mouse response for selecting objects in images
// If left button is clicked, start drawing a rectangle as long as mouse moves
// Stop drawing once a new click is registered
void on_mouse(int event, int x, int y, int flag, void* param)
{
    // Action when left button is clicked
    if(event == CV_EVENT_LBUTTONDOWN)
    {
        if(!start_draw)
        {
            roi_x0 = x;
            roi_y0 = y;
            start_draw = true;
        } else {
            roi_x1 = x;
            roi_y1 = y;
            start_draw = false;
        }
    }
    // Action when mouse is moving
    if((event == CV_EVENT_MOUSEMOVE) && start_draw)
    {
        // If an image is to large, it is segmented but this should be considered in the drawing
        if (!segment){
            //redraw ROI selection
            Mat image2;
            image.copyTo(image2);
            rectangle(image2, Point(roi_x0,roi_y0), Point(x,y), Scalar(0,0,255));
            imshow(window_name, image2);
            moveWindow(window_name, 50, 50); //used to make sure that window is in a reachable area
        }
        if (segment){
            //redraw ROI selection
            Mat image_part2;
            image_part.copyTo(image_part2);
            rectangle(image_part2, Point(roi_x0,roi_y0), Point(x,y), Scalar(0,0,255));
            imshow(window_name, image_part2);
            moveWindow(window_name, 50, 50);
        }
    }
}

// FUNCTION : Small snippet to convert an int to a string with a clean function, instead of creating a stringstream each time
string int_to_string(int num)
{
    stringstream myStream; //creates an ostringstream object
    myStream << num;
    return myStream.str(); //returns the string form of the stringstream object
}

// FUNCTION : Based on a set of positive images, create the output
void get_annotations(Mat input_image, stringstream* output_stream)
{
    // Make it possible to jump out of the looping over image
    bool stop = false;

    int pressed_key = 0;
    num_of_rec = 0;
    image = input_image;

    //	init highgui - make sure that mouse actions are coupled to the correct window
    namedWindow(window_name, WINDOW_AUTOSIZE);
    setMouseCallback(window_name, on_mouse);

    // Create a stringstream to make sure all detections on this image are stored
    stringstream temp_result;

    // functionality that is called when segmentation is required
    // this is usefull when input data is larger than the actual computer screen size - and thus cannot be visualised in whole
    if (segment){
        // push the original image into a copy named img_large to perform processing
        Mat img_large(image);

        // Retrieve parameters for segmentation --> fixed for now, will be adaptable through configuration file
        // If size of width or heigth is larger than 750 px, take half windows, if it is larger than 1500 px take a third
        Size dimensions = img_large.size();
        int v_part = dimensions.height, h_part = dimensions.width, overlap = 150;
        if (dimensions.width > 1000|| dimensions.height > 1000){
            v_part = dimensions.height / 2;
            h_part = dimensions.width / 2;
            overlap = 100;
        }
        if (dimensions.width > 1500 || dimensions.height > 1500){
            v_part = dimensions.height / 3;
            h_part = dimensions.width / 3;
            overlap = 150;
        }

        // Variables and calculations needed for processing - to make sure that subwindows have correct regions
        Mat dst_img;
        int steps_horizontal = (dimensions.width)/(h_part - overlap);
        int steps_vertical = (dimensions.height)/(v_part - overlap);
        int rest_horizontal = dimensions.width - (steps_horizontal * (h_part-overlap)) - overlap;
        int rest_vertical = dimensions.height - (steps_vertical * (v_part-overlap)) - overlap;

        // Loop over the image window by window with a certain overlap that was chosen
        // This is in order to mark elements at border regions!
        for (int i = 0; i < steps_vertical; i++){
            for(int j = 0; j < steps_horizontal; j++){
                // warp window over the image
                int original_location_horizontal = 0;
                int original_location_vertical = 0;
                // for the first frame, no overlap is needed
                if (j == 0 && i == 0){
                    dst_img = img_large( Rect(j*h_part,i*v_part,h_part,v_part) );
                    original_location_horizontal = j*h_part;
                    original_location_vertical = i*v_part;
                // FIRST ROW case where we are at the last column - add extra space (overlap region)
                }else if(j == (steps_horizontal - 1) && i == 0 ){
                    dst_img = img_large( Rect(j*(h_part-overlap),i*v_part,h_part+rest_horizontal,v_part) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*v_part;
                // FIRST COLUMN case where we are at the last row - add extra space (overlap region)
                }else if(i == (steps_vertical - 1) && j == 0){
                    dst_img = img_large( Rect(j*h_part,i*(v_part-overlap),h_part,v_part+rest_vertical) );
                    original_location_horizontal = j*h_part;
                    original_location_vertical = i*(v_part-overlap);
                // LAST ROW LAST COLUMN
                }else if(j == (steps_horizontal - 1) && i == (steps_vertical - 1)){
                    dst_img = img_large( Rect(j*(h_part-overlap),i*(v_part-overlap),h_part+rest_horizontal,v_part+rest_vertical) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*(v_part-overlap);
                // case where we are at the last column - add extra space (overlap region)
                }else if(j == (steps_horizontal - 1) ){
                    dst_img = img_large( Rect(j*(h_part-overlap),i*(v_part-overlap),h_part+rest_horizontal,v_part) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*(v_part-overlap);
                // case where we are at the last row - add extra space (overlap region)
                }else if(i == (steps_vertical - 1) ){
                    dst_img = img_large( Rect(j*(h_part-overlap),i*(v_part-overlap),h_part,v_part+rest_vertical) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*(v_part-overlap);
                // case of the first row
                }else if(j == 0 && i != 0){
                    dst_img = img_large( Rect(j*h_part,i*(v_part-overlap),h_part,v_part) );
                    original_location_horizontal = j*h_part;
                    original_location_vertical = i*(v_part-overlap);
                // case of the first column
                }else if(j != 0 && i == 0){
                    dst_img = img_large( Rect(j*(h_part-overlap),i*v_part,h_part,v_part) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*v_part;
                // all the other cases
                }else{
                    dst_img = img_large( Rect(j*(h_part-overlap),i*(v_part-overlap),h_part,v_part) );
                    original_location_horizontal = j*(h_part-overlap);
                    original_location_vertical = i*(v_part-overlap);
                }

                // Add counter info to the image
                stringstream label;
                label << "Vertical: "<< (i + 1) << " Horizontal: " << (j + 1);
                putText(dst_img, label.str(), Point(25,25), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0), 2);

                // Copy the partial Mat image to a central container
                // This way the mouse refresh tool can use this knowledge for refreshing purposes
                image_part = dst_img;
                imshow(window_name,dst_img);
                moveWindow(window_name, 50, 50);

                // Wait 15 ms to actually process the visualization - only works when a window is open!
                waitKey(15);

                // For each segment of the image, process the frame based on keystroke instructions
                do
                {
                    // used waitKey returns:
                    //  <Numpad .> = 46			save added rectangles and show next image
                    //	<Numpad Enter> = 13		go to next window region
                    //	<ESC> = 27				exit program
                    pressed_key = waitKey(0);

                    switch(pressed_key)
                    {
                    // ESC was pressed --> exit the program
                    case 27:
                            destroyWindow(window_name);
                            stop = true;
                            break;
                    // Numpad dot was pressed --> add the rectangle to the current image
                    case 46:
                            num_of_rec++;
                            // --------------------------------------------------------------------------------------
                            // SEGMENTED INFORMATION - important difference with no segment
                            // - use knowledge of current overlap position to define the actual location in the large image
                            // - for this usage of parameters of loops can be used
                            // --------------------------------------------------------------------------------------
                            // Create new locations based on information that comes from segment, use this for textfile, not for visualization
                            int roi_x0_new = roi_x0 + original_location_horizontal;
                            int roi_y0_new = roi_y0 + original_location_vertical;
                            int roi_x1_new = roi_x1 + original_location_horizontal;
                            int roi_y1_new = roi_y1 + original_location_vertical;

                            // Draw initiated from top left corner
                            if(roi_x0<roi_x1 && roi_y0<roi_y1)
                            {
                                temp_result << " " << int_to_string(roi_x0_new) << " " << int_to_string(roi_y0_new) << " " << int_to_string(roi_x1_new-roi_x0_new) << " " << int_to_string(roi_y1_new-roi_y0_new);
                            }
                            // Draw initiated from bottom right corner
                            if(roi_x0>roi_x1 && roi_y0>roi_y1)
                            {
                                temp_result << " " << int_to_string(roi_x1_new) << " " << int_to_string(roi_y1_new) << " " << int_to_string(roi_x0_new-roi_x1_new) << " " << int_to_string(roi_y0_new-roi_y1_new);
                            }
                            // Draw initiated from top right corner
                            if(roi_x0>roi_x1 && roi_y0<roi_y1)
                            {
                                temp_result << " " << int_to_string(roi_x1_new) << " " << int_to_string(roi_y0_new) << " " << int_to_string(roi_x0_new-roi_x1_new) << " " << int_to_string(roi_y1_new-roi_y0_new);
                            }
                            // Draw initiated from bottom left corner
                            if(roi_x0<roi_x1 && roi_y0>roi_y1)
                            {
                                temp_result << " " << int_to_string(roi_x0_new) << " " << int_to_string(roi_y1_new) << " " << int_to_string(roi_x1_new-roi_x0_new) << " " << int_to_string(roi_y0_new-roi_y1_new);
                            }

                            rectangle(image_part, Point(roi_x0,roi_y0), Point(roi_x1,roi_y1), Scalar(0,255,0));

                            break;
                    }

                    // go out of while loop
                    if(stop){ break;}

                // Numpad ENTER was pressed --> add the rectangle to the current image
                }while(pressed_key!=13);
                // 13 goes for ENTER at numpad
                // when ESC was pressed
                if(stop){ break;}
            }
            // when ESC was pressed - go out double loop
            if(stop){ break;}
        }
    }

    // Functionality that is called when segmentation is not required
    // This is likewise but more simple then the above steps - no segments need to be created
    if (!segment){
        imshow(window_name, image);
        moveWindow(window_name, 50, 50);

        do
        {
            // used waitKey returns:
            //  <Numpad .> = 46			add rectangle to current image
            //	<Numpad Enter> = 13		save added rectangles and show next image
            //	<ESC> = 27				exit program
            pressed_key=waitKey(0);

            switch(pressed_key)
            {
            case 27:
                    destroyWindow(window_name);
                    stop = true;
            case 46:
                    num_of_rec++;
                    // Draw initiated from top left corner
                    if(roi_x0<roi_x1 && roi_y0<roi_y1)
                    {
                        temp_result << " " << int_to_string(roi_x0) << " " << int_to_string(roi_y0) << " " << int_to_string(roi_x1-roi_x0) << " " << int_to_string(roi_y1-roi_y0);
                    }
                    // Draw initiated from bottom right corner
                    if(roi_x0>roi_x1 && roi_y0>roi_y1)
                    {
                        temp_result << " " << int_to_string(roi_x1) << " " << int_to_string(roi_y1) << " " << int_to_string(roi_x0-roi_x1) << " " << int_to_string(roi_y0-roi_y1);
                    }
                    // Draw initiated from top right corner
                    if(roi_x0>roi_x1 && roi_y0<roi_y1)
                    {
                        temp_result << " " << int_to_string(roi_x1) << " " << int_to_string(roi_y0) << " " << int_to_string(roi_x0-roi_x1) << " " << int_to_string(roi_y1-roi_y0);
                    }
                    // Draw initiated from bottom left corner
                    if(roi_x0<roi_x1 && roi_y0>roi_y1)
                    {
                        temp_result << " " << int_to_string(roi_x0) << " " << int_to_string(roi_y1) << " " << int_to_string(roi_x1-roi_x0) << " " << int_to_string(roi_y0-roi_y1);
                    }

                    rectangle(input_image, Point(roi_x0,roi_y0), Point(roi_x1,roi_y1), Scalar(0,255,0), 1);

                    break;
            }

            // Check if escape has been pressed
            if(stop)
            {
                break;
            }
        }
        while(pressed_key!=13);
    }

    if(num_of_rec>0 && pressed_key==13)
    {

        *output_stream << " " << num_of_rec << temp_result.str() << endl;
    }

    destroyWindow(window_name);
}

int main( int argc, const char** argv )
{
    // Check if arguments are given correct
	if( argc == 1 && argc != 4 ){
		cout << "Usage of object annotation software;" << endl;
		cout <<	"annotate_images.exe <root_folder> <object_images.txt> <detection_result.txt>" << endl;
		return 0;
	}

	// Retrieve the passed parameters
    root_folder = argv[1];
	string object_images = argv[2];
	string detection_result = argv[3];

    // Use the positive sample file to generate a filenames element
	stringstream in;
	in << root_folder << object_images;
	ifstream input (in.str().c_str());
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

    // Create output file stream
    stringstream out;
	out << root_folder << detection_result;
    ofstream output (out.str().c_str());

    // Loop through each image, storing the annotations once completed
    for (int i = 0; i < filenames.size(); i++){
        // Read in an image
        Mat current_image = imread(filenames[i]);

        // Check if original image needs to be splitted
        // For moment if width or height is larger than 750 pixels, then splitting is necessary
        if ( (current_image.cols > 750) || (current_image.rows > 750)){
            // Put the global segmentation variable on true
            segment = true;
        }

        // Perform annotations & generate corresponding output
        stringstream output_stream;
        get_annotations(current_image, &output_stream);

        // Store the annotations, write to the output file
        if (output_stream.str() != ""){
            output << filenames[i] << output_stream.str();
        }

    }

    return 0;
}
