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

This is a winforms GUI interface for annotation. Due to using the windows structure, most
functionality is provided in the headerfile Form1.h.

This software allows people to import images and manually annotate regions of interest. 
Comes with multiple options for large images, color conversion, ...

*****************************************************************************************************/
#pragma once

// Includes for openCV functionality, requires openCV 2.4.5 or higher to compile correctly!
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

// Include needed to define target architecture
#include <windows.h>

// Includes for standard file input output functionality
#include <fstream>
#include <iostream>
#include <strsafe.h>

// Includes for namespaces
// Not using the OpenCV namespace --> too much conflicts with std namespace and winforms functionality
// Just use cv:: in front of OpenCV functionality
using namespace std;

// Namespace needed for marshalling - making sure that string pointers can be retrieved from form
using namespace System::Runtime::InteropServices;

namespace clean_annotation_tool {
	// Namespaces needed to make winform functionality available inside the project
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	// Public parameters that are needed to interop between functions correctly
	cv::Mat image;
	cv::Mat image_part;

	// Parameters needed for the continuous redrawing when using mouse actions
	int roi_x0=0;
	int roi_y0=0;
	int roi_x1=0;
	int roi_y1=0;
	int num_of_rec=0;
	int startDraw = 0;
	const string window_name="<NUMPAD DEC> add ROI <NUMPAD ENTER> save/next";
		
	bool segment = false;

	string root_folder = "";
	
	// FUNCTION : Adding mouse actions in new window to process mouse action
	// If left button is clicked, start drawing a rectangle as long as mouse moves
	// Stop drawing once a new click is registered
	void on_mouse(int event,int x,int y,int flag, void* param)
	{
		if(event==CV_EVENT_LBUTTONDOWN)
		{
			if(!startDraw)
			{
				roi_x0=x;
				roi_y0=y;
				startDraw = 1;
			} else {
				roi_x1=x;
				roi_y1=y;
				startDraw = 0;
			}
		}
		if(event==CV_EVENT_MOUSEMOVE && startDraw)
		{
			if (!segment){ 
				//redraw ROI selection
				cv::Mat image2;
				image.copyTo(image2);
				cv::rectangle(image2, cvPoint(roi_x0,roi_y0), cvPoint(x,y), CV_RGB(255,0,0));
				cv::imshow(window_name, image2);
				cv::moveWindow(window_name, 50, 50);
			}
			if (segment){
				//redraw ROI selection
				cv::Mat image_part2;
				image_part.copyTo(image_part2);
				cv::rectangle(image_part2, cvPoint(roi_x0,roi_y0), cvPoint(x,y), CV_RGB(255,0,0));
				cv::imshow(window_name, image_part2);
				cv::moveWindow(window_name, 50, 50);
			}
		}
	};

	// FUNCTION : Small snippet to convert an int to a string with a clean function, instead of creating a stringstream each time
	string int_to_string(int num)
	{
		stringstream myStream; //creates an ostringstream object
		myStream << num;
		return myStream.str(); //returns the string form of the stringstream object
	};

	// THIS IS THE AUTOMATIC CODE GENERATED FROM FORM CONSTRUCTION, CHANGING IT SHOULD PROBABLY NOT BE NECCESARY
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();

		}
		
	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	
	private: System::Windows::Forms::SplitContainer^  splitContainer1;

	private: System::Windows::Forms::CheckBox^  checkbox_boundingbox;
	private: System::Windows::Forms::CheckBox^  checkbox_amount_annotations;
	private: System::Windows::Forms::CheckBox^  checkbox_image_name;
	private: System::Windows::Forms::CheckBox^  checkbox_input_video;
	private: System::Windows::Forms::CheckBox^  checkbox_input_images;
	private: System::Windows::Forms::CheckBox^  checkbox_subwindows;
	private: System::Windows::Forms::CheckBox^  checkbox_create_negatives;

	private: System::Windows::Forms::Label^  label_process_parameters;
	private: System::Windows::Forms::Label^  label_positive_folder;
	private: System::Windows::Forms::Label^  label_negative_folder;
	private: System::Windows::Forms::Label^  label_configuration;
	private: System::Windows::Forms::Label^  label_seperator_1;
	private: System::Windows::Forms::Label^  label_output_parameters;
	private: System::Windows::Forms::Label^  label_step_y;
	private: System::Windows::Forms::Label^  label_step_x;
	private: System::Windows::Forms::Label^  label_overlap;
	private: System::Windows::Forms::Label^  label_seperator_2;
	private: System::Windows::Forms::Label^  label_select_inputvideo;
	private: System::Windows::Forms::Label^  label_subwindow;
	private: System::Windows::Forms::Label^  label_seperator_3;
	private: System::Windows::Forms::Label^  label_seperator_4;
	private: System::Windows::Forms::Label^  label_seperator_5;

	private: System::Windows::Forms::Button^  button_annotate_positives;
	private: System::Windows::Forms::Button^  button_create_negative_file;
	private: System::Windows::Forms::Button^  button_select_negatives;
	private: System::Windows::Forms::Button^  button_select_positives;
	private: System::Windows::Forms::Button^  button_reset_root;
	private: System::Windows::Forms::Button^  button_reset_form;
	private: System::Windows::Forms::Button^  button_open_positives;
	private: System::Windows::Forms::Button^  button_open_negatives;
	private: System::Windows::Forms::Button^  button_select_video;
	private: System::Windows::Forms::Button^  button_open_root;

	private: FolderBrowserDialog^ browser_folder;
	private: OpenFileDialog^ browser_file;

	private: System::Windows::Forms::TextBox^  textbox_negatives;
	private: System::Windows::Forms::TextBox^  textbox_positives;
	private: System::Windows::Forms::TextBox^  textbox_overlap;
	private: System::Windows::Forms::TextBox^  textbox_y_direction;
	private: System::Windows::Forms::TextBox^  textbox_x_direction;
	private: System::Windows::Forms::TextBox^  textbox_input_video;

	private: System::Windows::Forms::PictureBox^  picturebox_negatives;
	private: System::Windows::Forms::PictureBox^  picturebox_positives;

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		// THIS CODE IS BETTER CHANGED USING THE PROPERTIES WINDOWS OF THE WINFORM ELEMENTS TO GUARANTEE GOOD CHANGEMENTS
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->checkbox_create_negatives = (gcnew System::Windows::Forms::CheckBox());
			this->label_seperator_3 = (gcnew System::Windows::Forms::Label());
			this->button_select_video = (gcnew System::Windows::Forms::Button());
			this->textbox_input_video = (gcnew System::Windows::Forms::TextBox());
			this->label_select_inputvideo = (gcnew System::Windows::Forms::Label());
			this->label_seperator_2 = (gcnew System::Windows::Forms::Label());
			this->label_seperator_1 = (gcnew System::Windows::Forms::Label());
			this->button_select_negatives = (gcnew System::Windows::Forms::Button());
			this->label_negative_folder = (gcnew System::Windows::Forms::Label());
			this->button_select_positives = (gcnew System::Windows::Forms::Button());
			this->label_positive_folder = (gcnew System::Windows::Forms::Label());
			this->textbox_negatives = (gcnew System::Windows::Forms::TextBox());
			this->textbox_positives = (gcnew System::Windows::Forms::TextBox());
			this->checkbox_input_images = (gcnew System::Windows::Forms::CheckBox());
			this->label_configuration = (gcnew System::Windows::Forms::Label());
			this->checkbox_input_video = (gcnew System::Windows::Forms::CheckBox());
			this->label_seperator_5 = (gcnew System::Windows::Forms::Label());
			this->button_open_root = (gcnew System::Windows::Forms::Button());
			this->label_seperator_4 = (gcnew System::Windows::Forms::Label());
			this->label_subwindow = (gcnew System::Windows::Forms::Label());
			this->label_overlap = (gcnew System::Windows::Forms::Label());
			this->button_open_negatives = (gcnew System::Windows::Forms::Button());
			this->button_reset_root = (gcnew System::Windows::Forms::Button());
			this->textbox_overlap = (gcnew System::Windows::Forms::TextBox());
			this->button_reset_form = (gcnew System::Windows::Forms::Button());
			this->button_open_positives = (gcnew System::Windows::Forms::Button());
			this->label_step_y = (gcnew System::Windows::Forms::Label());
			this->label_step_x = (gcnew System::Windows::Forms::Label());
			this->textbox_y_direction = (gcnew System::Windows::Forms::TextBox());
			this->textbox_x_direction = (gcnew System::Windows::Forms::TextBox());
			this->checkbox_subwindows = (gcnew System::Windows::Forms::CheckBox());
			this->label_output_parameters = (gcnew System::Windows::Forms::Label());
			this->picturebox_negatives = (gcnew System::Windows::Forms::PictureBox());
			this->picturebox_positives = (gcnew System::Windows::Forms::PictureBox());
			this->button_create_negative_file = (gcnew System::Windows::Forms::Button());
			this->button_annotate_positives = (gcnew System::Windows::Forms::Button());
			this->label_process_parameters = (gcnew System::Windows::Forms::Label());
			this->checkbox_image_name = (gcnew System::Windows::Forms::CheckBox());
			this->checkbox_amount_annotations = (gcnew System::Windows::Forms::CheckBox());
			this->checkbox_boundingbox = (gcnew System::Windows::Forms::CheckBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picturebox_negatives))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picturebox_positives))->BeginInit();
			this->SuspendLayout();
			// 
			// splitContainer1
			// 
			this->splitContainer1->BackColor = System::Drawing::SystemColors::Control;
			this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->IsSplitterFixed = true;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->BackColor = System::Drawing::SystemColors::Control;
			this->splitContainer1->Panel1->Controls->Add(this->checkbox_create_negatives);
			this->splitContainer1->Panel1->Controls->Add(this->label_seperator_3);
			this->splitContainer1->Panel1->Controls->Add(this->button_select_video);
			this->splitContainer1->Panel1->Controls->Add(this->textbox_input_video);
			this->splitContainer1->Panel1->Controls->Add(this->label_select_inputvideo);
			this->splitContainer1->Panel1->Controls->Add(this->label_seperator_2);
			this->splitContainer1->Panel1->Controls->Add(this->label_seperator_1);
			this->splitContainer1->Panel1->Controls->Add(this->button_select_negatives);
			this->splitContainer1->Panel1->Controls->Add(this->label_negative_folder);
			this->splitContainer1->Panel1->Controls->Add(this->button_select_positives);
			this->splitContainer1->Panel1->Controls->Add(this->label_positive_folder);
			this->splitContainer1->Panel1->Controls->Add(this->textbox_negatives);
			this->splitContainer1->Panel1->Controls->Add(this->textbox_positives);
			this->splitContainer1->Panel1->Controls->Add(this->checkbox_input_images);
			this->splitContainer1->Panel1->Controls->Add(this->label_configuration);
			this->splitContainer1->Panel1->Controls->Add(this->checkbox_input_video);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->BackColor = System::Drawing::SystemColors::Control;
			this->splitContainer1->Panel2->Controls->Add(this->label_seperator_5);
			this->splitContainer1->Panel2->Controls->Add(this->button_open_root);
			this->splitContainer1->Panel2->Controls->Add(this->label_seperator_4);
			this->splitContainer1->Panel2->Controls->Add(this->label_subwindow);
			this->splitContainer1->Panel2->Controls->Add(this->label_overlap);
			this->splitContainer1->Panel2->Controls->Add(this->button_open_negatives);
			this->splitContainer1->Panel2->Controls->Add(this->button_reset_root);
			this->splitContainer1->Panel2->Controls->Add(this->textbox_overlap);
			this->splitContainer1->Panel2->Controls->Add(this->button_reset_form);
			this->splitContainer1->Panel2->Controls->Add(this->button_open_positives);
			this->splitContainer1->Panel2->Controls->Add(this->label_step_y);
			this->splitContainer1->Panel2->Controls->Add(this->label_step_x);
			this->splitContainer1->Panel2->Controls->Add(this->textbox_y_direction);
			this->splitContainer1->Panel2->Controls->Add(this->textbox_x_direction);
			this->splitContainer1->Panel2->Controls->Add(this->checkbox_subwindows);
			this->splitContainer1->Panel2->Controls->Add(this->label_output_parameters);
			this->splitContainer1->Panel2->Controls->Add(this->picturebox_negatives);
			this->splitContainer1->Panel2->Controls->Add(this->picturebox_positives);
			this->splitContainer1->Panel2->Controls->Add(this->button_create_negative_file);
			this->splitContainer1->Panel2->Controls->Add(this->button_annotate_positives);
			this->splitContainer1->Panel2->Controls->Add(this->label_process_parameters);
			this->splitContainer1->Panel2->Controls->Add(this->checkbox_image_name);
			this->splitContainer1->Panel2->Controls->Add(this->checkbox_amount_annotations);
			this->splitContainer1->Panel2->Controls->Add(this->checkbox_boundingbox);
			this->splitContainer1->Size = System::Drawing::Size(1024, 300);
			this->splitContainer1->SplitterDistance = 457;
			this->splitContainer1->TabIndex = 0;
			// 
			// checkbox_create_negatives
			// 
			this->checkbox_create_negatives->AutoSize = true;
			this->checkbox_create_negatives->Location = System::Drawing::Point(270, 36);
			this->checkbox_create_negatives->Name = L"checkbox_create_negatives";
			this->checkbox_create_negatives->Size = System::Drawing::Size(121, 17);
			this->checkbox_create_negatives->TabIndex = 21;
			this->checkbox_create_negatives->Text = L"create negatives file";
			this->checkbox_create_negatives->UseVisualStyleBackColor = true;
			this->checkbox_create_negatives->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkbox_create_negatives_CheckedChanged);
			// 
			// label_seperator_3
			// 
			this->label_seperator_3->AutoSize = true;
			this->label_seperator_3->Location = System::Drawing::Point(7, 202);
			this->label_seperator_3->Name = L"label_seperator_3";
			this->label_seperator_3->Size = System::Drawing::Size(445, 13);
			this->label_seperator_3->TabIndex = 20;
			this->label_seperator_3->Text = L"---------------------------------------------------------------------------------" 
				L"-----------------------------------------------------------------";
			// 
			// button_select_video
			// 
			this->button_select_video->Location = System::Drawing::Point(308, 97);
			this->button_select_video->Name = L"button_select_video";
			this->button_select_video->Size = System::Drawing::Size(134, 23);
			this->button_select_video->TabIndex = 19;
			this->button_select_video->Text = L"select video";
			this->button_select_video->UseVisualStyleBackColor = true;
			this->button_select_video->Click += gcnew System::EventHandler(this, &Form1::button_select_video_Click);
			// 
			// textbox_input_video
			// 
			this->textbox_input_video->Location = System::Drawing::Point(10, 100);
			this->textbox_input_video->Name = L"textbox_input_video";
			this->textbox_input_video->Size = System::Drawing::Size(284, 20);
			this->textbox_input_video->TabIndex = 18;
			// 
			// label_select_inputvideo
			// 
			this->label_select_inputvideo->AutoSize = true;
			this->label_select_inputvideo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_select_inputvideo->Location = System::Drawing::Point(10, 77);
			this->label_select_inputvideo->Name = L"label_select_inputvideo";
			this->label_select_inputvideo->Size = System::Drawing::Size(131, 20);
			this->label_select_inputvideo->TabIndex = 17;
			this->label_select_inputvideo->Text = L"select input video";
			// 
			// label_seperator_2
			// 
			this->label_seperator_2->AutoSize = true;
			this->label_seperator_2->Location = System::Drawing::Point(7, 133);
			this->label_seperator_2->Name = L"label_seperator_2";
			this->label_seperator_2->Size = System::Drawing::Size(445, 13);
			this->label_seperator_2->TabIndex = 16;
			this->label_seperator_2->Text = L"---------------------------------------------------------------------------------" 
				L"-----------------------------------------------------------------";
			// 
			// label_seperator_1
			// 
			this->label_seperator_1->AutoSize = true;
			this->label_seperator_1->Location = System::Drawing::Point(7, 64);
			this->label_seperator_1->Name = L"label_seperator_1";
			this->label_seperator_1->Size = System::Drawing::Size(445, 13);
			this->label_seperator_1->TabIndex = 9;
			this->label_seperator_1->Text = L"---------------------------------------------------------------------------------" 
				L"-----------------------------------------------------------------";
			// 
			// button_select_negatives
			// 
			this->button_select_negatives->Enabled = false;
			this->button_select_negatives->Location = System::Drawing::Point(308, 244);
			this->button_select_negatives->Name = L"button_select_negatives";
			this->button_select_negatives->Size = System::Drawing::Size(134, 23);
			this->button_select_negatives->TabIndex = 5;
			this->button_select_negatives->Text = L"select negatives";
			this->button_select_negatives->UseVisualStyleBackColor = true;
			this->button_select_negatives->Click += gcnew System::EventHandler(this, &Form1::button_select_negatives_Click);
			// 
			// label_negative_folder
			// 
			this->label_negative_folder->AutoSize = true;
			this->label_negative_folder->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_negative_folder->Location = System::Drawing::Point(10, 215);
			this->label_negative_folder->Name = L"label_negative_folder";
			this->label_negative_folder->Size = System::Drawing::Size(167, 20);
			this->label_negative_folder->TabIndex = 1;
			this->label_negative_folder->Text = L"select negatives folder";
			// 
			// button_select_positives
			// 
			this->button_select_positives->Enabled = false;
			this->button_select_positives->Location = System::Drawing::Point(308, 166);
			this->button_select_positives->Name = L"button_select_positives";
			this->button_select_positives->Size = System::Drawing::Size(134, 23);
			this->button_select_positives->TabIndex = 4;
			this->button_select_positives->Text = L"select positives";
			this->button_select_positives->UseVisualStyleBackColor = true;
			this->button_select_positives->Click += gcnew System::EventHandler(this, &Form1::button_select_positives_Click);
			// 
			// label_positive_folder
			// 
			this->label_positive_folder->AutoSize = true;
			this->label_positive_folder->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_positive_folder->Location = System::Drawing::Point(10, 146);
			this->label_positive_folder->Name = L"label_positive_folder";
			this->label_positive_folder->Size = System::Drawing::Size(160, 20);
			this->label_positive_folder->TabIndex = 0;
			this->label_positive_folder->Text = L"select positives folder";
			// 
			// textbox_negatives
			// 
			this->textbox_negatives->Enabled = false;
			this->textbox_negatives->Location = System::Drawing::Point(10, 247);
			this->textbox_negatives->Name = L"textbox_negatives";
			this->textbox_negatives->Size = System::Drawing::Size(284, 20);
			this->textbox_negatives->TabIndex = 3;
			// 
			// textbox_positives
			// 
			this->textbox_positives->Enabled = false;
			this->textbox_positives->Location = System::Drawing::Point(10, 170);
			this->textbox_positives->Name = L"textbox_positives";
			this->textbox_positives->Size = System::Drawing::Size(284, 20);
			this->textbox_positives->TabIndex = 2;
			// 
			// checkbox_input_images
			// 
			this->checkbox_input_images->AutoSize = true;
			this->checkbox_input_images->Location = System::Drawing::Point(114, 36);
			this->checkbox_input_images->Name = L"checkbox_input_images";
			this->checkbox_input_images->Size = System::Drawing::Size(150, 17);
			this->checkbox_input_images->TabIndex = 12;
			this->checkbox_input_images->Text = L"use input image sequence";
			this->checkbox_input_images->UseVisualStyleBackColor = true;
			this->checkbox_input_images->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkbox_input_images_CheckedChanged);
			// 
			// label_configuration
			// 
			this->label_configuration->AutoSize = true;
			this->label_configuration->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_configuration->Location = System::Drawing::Point(3, 2);
			this->label_configuration->Name = L"label_configuration";
			this->label_configuration->Size = System::Drawing::Size(342, 20);
			this->label_configuration->TabIndex = 9;
			this->label_configuration->Text = L"Configuration of input data for annotation";
			this->label_configuration->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// checkbox_input_video
			// 
			this->checkbox_input_video->AutoSize = true;
			this->checkbox_input_video->Checked = true;
			this->checkbox_input_video->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkbox_input_video->Location = System::Drawing::Point(10, 36);
			this->checkbox_input_video->Name = L"checkbox_input_video";
			this->checkbox_input_video->Size = System::Drawing::Size(98, 17);
			this->checkbox_input_video->TabIndex = 10;
			this->checkbox_input_video->Text = L"use input video";
			this->checkbox_input_video->UseVisualStyleBackColor = true;
			this->checkbox_input_video->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkbox_input_video_CheckedChanged);
			// 
			// label_seperator_5
			// 
			this->label_seperator_5->AutoSize = true;
			this->label_seperator_5->Location = System::Drawing::Point(2, 195);
			this->label_seperator_5->Name = L"label_seperator_5";
			this->label_seperator_5->Size = System::Drawing::Size(550, 13);
			this->label_seperator_5->TabIndex = 30;
			this->label_seperator_5->Text = L"---------------------------------------------------------------------------------" 
				L"--------------------------------------------------------------------------------" 
				L"--------------------";
			// 
			// button_open_root
			// 
			this->button_open_root->Location = System::Drawing::Point(6, 269);
			this->button_open_root->Name = L"button_open_root";
			this->button_open_root->Size = System::Drawing::Size(163, 23);
			this->button_open_root->TabIndex = 29;
			this->button_open_root->Text = L"open root folder";
			this->button_open_root->UseVisualStyleBackColor = true;
			this->button_open_root->Click += gcnew System::EventHandler(this, &Form1::button_open_root_Click);
			// 
			// label_seperator_4
			// 
			this->label_seperator_4->AutoSize = true;
			this->label_seperator_4->Location = System::Drawing::Point(5, 107);
			this->label_seperator_4->Name = L"label_seperator_4";
			this->label_seperator_4->Size = System::Drawing::Size(550, 13);
			this->label_seperator_4->TabIndex = 27;
			this->label_seperator_4->Text = L"---------------------------------------------------------------------------------" 
				L"--------------------------------------------------------------------------------" 
				L"--------------------";
			// 
			// label_subwindow
			// 
			this->label_subwindow->AutoSize = true;
			this->label_subwindow->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_subwindow->Location = System::Drawing::Point(3, 62);
			this->label_subwindow->Name = L"label_subwindow";
			this->label_subwindow->Size = System::Drawing::Size(158, 17);
			this->label_subwindow->TabIndex = 26;
			this->label_subwindow->Text = L"Subwindow parameters:";
			this->label_subwindow->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_overlap
			// 
			this->label_overlap->AutoSize = true;
			this->label_overlap->Location = System::Drawing::Point(424, 62);
			this->label_overlap->Name = L"label_overlap";
			this->label_overlap->Size = System::Drawing::Size(47, 13);
			this->label_overlap->TabIndex = 25;
			this->label_overlap->Text = L"Overlap:";
			// 
			// button_open_negatives
			// 
			this->button_open_negatives->Location = System::Drawing::Point(5, 240);
			this->button_open_negatives->Name = L"button_open_negatives";
			this->button_open_negatives->Size = System::Drawing::Size(163, 23);
			this->button_open_negatives->TabIndex = 15;
			this->button_open_negatives->Text = L"open negative image folder";
			this->button_open_negatives->UseVisualStyleBackColor = true;
			this->button_open_negatives->Click += gcnew System::EventHandler(this, &Form1::button_open_negatives_Click);
			// 
			// button_reset_root
			// 
			this->button_reset_root->Location = System::Drawing::Point(200, 240);
			this->button_reset_root->Name = L"button_reset_root";
			this->button_reset_root->Size = System::Drawing::Size(162, 23);
			this->button_reset_root->TabIndex = 12;
			this->button_reset_root->Text = L"reset root folder";
			this->button_reset_root->UseVisualStyleBackColor = true;
			this->button_reset_root->Click += gcnew System::EventHandler(this, &Form1::button_reset_root_Click);
			// 
			// textbox_overlap
			// 
			this->textbox_overlap->Enabled = false;
			this->textbox_overlap->Location = System::Drawing::Point(472, 59);
			this->textbox_overlap->Name = L"textbox_overlap";
			this->textbox_overlap->Size = System::Drawing::Size(56, 20);
			this->textbox_overlap->TabIndex = 24;
			this->textbox_overlap->Text = L"0";
			// 
			// button_reset_form
			// 
			this->button_reset_form->Location = System::Drawing::Point(200, 211);
			this->button_reset_form->Name = L"button_reset_form";
			this->button_reset_form->Size = System::Drawing::Size(162, 23);
			this->button_reset_form->TabIndex = 11;
			this->button_reset_form->Text = L"reset this form";
			this->button_reset_form->UseVisualStyleBackColor = true;
			this->button_reset_form->Click += gcnew System::EventHandler(this, &Form1::button_reset_form_Click);
			// 
			// button_open_positives
			// 
			this->button_open_positives->Location = System::Drawing::Point(5, 211);
			this->button_open_positives->Name = L"button_open_positives";
			this->button_open_positives->Size = System::Drawing::Size(163, 23);
			this->button_open_positives->TabIndex = 14;
			this->button_open_positives->Text = L"open positive image folder";
			this->button_open_positives->UseVisualStyleBackColor = true;
			this->button_open_positives->Click += gcnew System::EventHandler(this, &Form1::button_open_positives_Click);
			// 
			// label_step_y
			// 
			this->label_step_y->AutoSize = true;
			this->label_step_y->Location = System::Drawing::Point(279, 84);
			this->label_step_y->Name = L"label_step_y";
			this->label_step_y->Size = System::Drawing::Size(83, 13);
			this->label_step_y->TabIndex = 23;
			this->label_step_y->Text = L"Step y direction:";
			// 
			// label_step_x
			// 
			this->label_step_x->AutoSize = true;
			this->label_step_x->Location = System::Drawing::Point(279, 64);
			this->label_step_x->Name = L"label_step_x";
			this->label_step_x->Size = System::Drawing::Size(83, 13);
			this->label_step_x->TabIndex = 22;
			this->label_step_x->Text = L"Step x direction:";
			// 
			// textbox_y_direction
			// 
			this->textbox_y_direction->Enabled = false;
			this->textbox_y_direction->Location = System::Drawing::Point(362, 81);
			this->textbox_y_direction->Name = L"textbox_y_direction";
			this->textbox_y_direction->Size = System::Drawing::Size(56, 20);
			this->textbox_y_direction->TabIndex = 21;
			this->textbox_y_direction->Text = L"500";
			// 
			// textbox_x_direction
			// 
			this->textbox_x_direction->Enabled = false;
			this->textbox_x_direction->Location = System::Drawing::Point(362, 59);
			this->textbox_x_direction->Name = L"textbox_x_direction";
			this->textbox_x_direction->Size = System::Drawing::Size(56, 20);
			this->textbox_x_direction->TabIndex = 20;
			this->textbox_x_direction->Text = L"500";
			// 
			// checkbox_subwindows
			// 
			this->checkbox_subwindows->AutoSize = true;
			this->checkbox_subwindows->Location = System::Drawing::Point(167, 63);
			this->checkbox_subwindows->Name = L"checkbox_subwindows";
			this->checkbox_subwindows->Size = System::Drawing::Size(106, 17);
			this->checkbox_subwindows->TabIndex = 19;
			this->checkbox_subwindows->Text = L"Use subwindows";
			this->checkbox_subwindows->UseVisualStyleBackColor = true;
			this->checkbox_subwindows->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkbox_subwindows_CheckedChanged);
			// 
			// label_output_parameters
			// 
			this->label_output_parameters->AutoSize = true;
			this->label_output_parameters->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_output_parameters->Location = System::Drawing::Point(3, 31);
			this->label_output_parameters->Name = L"label_output_parameters";
			this->label_output_parameters->Size = System::Drawing::Size(131, 17);
			this->label_output_parameters->TabIndex = 17;
			this->label_output_parameters->Text = L"Output parameters:";
			this->label_output_parameters->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// picturebox_negatives
			// 
			this->picturebox_negatives->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"picturebox_negatives.Image")));
			this->picturebox_negatives->Location = System::Drawing::Point(368, 162);
			this->picturebox_negatives->Name = L"picturebox_negatives";
			this->picturebox_negatives->Size = System::Drawing::Size(28, 28);
			this->picturebox_negatives->TabIndex = 8;
			this->picturebox_negatives->TabStop = false;
			this->picturebox_negatives->Visible = false;
			// 
			// picturebox_positives
			// 
			this->picturebox_positives->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"picturebox_positives.Image")));
			this->picturebox_positives->Location = System::Drawing::Point(368, 125);
			this->picturebox_positives->Name = L"picturebox_positives";
			this->picturebox_positives->Size = System::Drawing::Size(28, 28);
			this->picturebox_positives->TabIndex = 7;
			this->picturebox_positives->TabStop = false;
			this->picturebox_positives->Visible = false;
			// 
			// button_create_negative_file
			// 
			this->button_create_negative_file->Enabled = false;
			this->button_create_negative_file->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->button_create_negative_file->Location = System::Drawing::Point(8, 162);
			this->button_create_negative_file->Name = L"button_create_negative_file";
			this->button_create_negative_file->Size = System::Drawing::Size(354, 30);
			this->button_create_negative_file->TabIndex = 6;
			this->button_create_negative_file->Text = L"create negative background image file";
			this->button_create_negative_file->UseVisualStyleBackColor = true;
			this->button_create_negative_file->Click += gcnew System::EventHandler(this, &Form1::button_create_negative_file_Click);
			// 
			// button_annotate_positives
			// 
			this->button_annotate_positives->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->button_annotate_positives->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_annotate_positives->Location = System::Drawing::Point(8, 124);
			this->button_annotate_positives->Name = L"button_annotate_positives";
			this->button_annotate_positives->Size = System::Drawing::Size(354, 31);
			this->button_annotate_positives->TabIndex = 5;
			this->button_annotate_positives->Text = L"start annotating positive images";
			this->button_annotate_positives->UseVisualStyleBackColor = true;
			this->button_annotate_positives->Click += gcnew System::EventHandler(this, &Form1::button_annotate_positives_Click);
			// 
			// label_process_parameters
			// 
			this->label_process_parameters->AutoSize = true;
			this->label_process_parameters->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_process_parameters->Location = System::Drawing::Point(1, 2);
			this->label_process_parameters->Name = L"label_process_parameters";
			this->label_process_parameters->Size = System::Drawing::Size(302, 20);
			this->label_process_parameters->TabIndex = 4;
			this->label_process_parameters->Text = L"Configuration of process parameters";
			this->label_process_parameters->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// checkbox_image_name
			// 
			this->checkbox_image_name->AutoSize = true;
			this->checkbox_image_name->Checked = true;
			this->checkbox_image_name->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkbox_image_name->Location = System::Drawing::Point(140, 33);
			this->checkbox_image_name->Name = L"checkbox_image_name";
			this->checkbox_image_name->Size = System::Drawing::Size(84, 17);
			this->checkbox_image_name->TabIndex = 0;
			this->checkbox_image_name->Text = L"Image name";
			this->checkbox_image_name->UseVisualStyleBackColor = true;
			// 
			// checkbox_amount_annotations
			// 
			this->checkbox_amount_annotations->AutoSize = true;
			this->checkbox_amount_annotations->Checked = true;
			this->checkbox_amount_annotations->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkbox_amount_annotations->Location = System::Drawing::Point(230, 33);
			this->checkbox_amount_annotations->Name = L"checkbox_amount_annotations";
			this->checkbox_amount_annotations->Size = System::Drawing::Size(99, 17);
			this->checkbox_amount_annotations->TabIndex = 1;
			this->checkbox_amount_annotations->Text = L"Amount objects";
			this->checkbox_amount_annotations->UseVisualStyleBackColor = true;
			// 
			// checkbox_boundingbox
			// 
			this->checkbox_boundingbox->AutoSize = true;
			this->checkbox_boundingbox->Checked = true;
			this->checkbox_boundingbox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkbox_boundingbox->Location = System::Drawing::Point(335, 32);
			this->checkbox_boundingbox->Name = L"checkbox_boundingbox";
			this->checkbox_boundingbox->Size = System::Drawing::Size(148, 17);
			this->checkbox_boundingbox->TabIndex = 2;
			this->checkbox_boundingbox->Text = L"Bounding box ( x, y, w, h )";
			this->checkbox_boundingbox->UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(1024, 300);
			this->Controls->Add(this->splitContainer1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = L"Form1";
			this->Text = L"Universal annotation tool - http://www.eavise.be/tobcat";
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picturebox_negatives))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picturebox_positives))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	// ---------------------------------------------------------------------------------------------------------------
	// Functionality behind buttons, teckboxes, ...
	// The following pieces of code contain the item handles like operation on click, selection, ...
	// ---------------------------------------------------------------------------------------------------------------
	
	// ---------------------------------------------------------------------------------------------------------------
	// File selections
	// ---------------------------------------------------------------------------------------------------------------
	private: System::Void button_select_video_Click(System::Object^  sender, System::EventArgs^  e) {
		// Create system file browser dialog, ask to select file, display in corresponding textBox
		browser_file = gcnew OpenFileDialog();
		browser_file->ShowDialog();
		textbox_input_video->Text = browser_file->FileName;
		delete browser_file;
	}

	private: System::Void button_select_positives_Click(System::Object^  sender, System::EventArgs^  e) {
		// Create system folder browser dialog, ask to select folder, display in corresponding textBox
		browser_folder = gcnew FolderBrowserDialog();
		browser_folder->ShowDialog();
		textbox_positives->Text = browser_folder->SelectedPath;
		delete browser_folder;
	}

	private: System::Void button_select_negatives_Click(System::Object^  sender, System::EventArgs^  e) {
		// Create system folder browser dialog, ask to select folder, display in corresponding textBox
		browser_folder = gcnew FolderBrowserDialog();
		browser_folder->ShowDialog();
		textbox_negatives->Text = browser_folder->SelectedPath;
		delete browser_folder;
	}
	
	// ---------------------------------------------------------------------------------------------------------------
	// Button for starting functionality
	// ---------------------------------------------------------------------------------------------------------------
	private: System::Void button_annotate_positives_Click(System::Object^  sender, System::EventArgs^  e) {
		// Check if video input or image input is demanded - then check if the folders are selected correctly
		if(checkbox_input_video->Checked){
			if (textbox_input_video->Text == ""){
				MessageBox::Show("Please select an input video","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
			}
		}
		if(checkbox_input_images->Checked){
			if (textbox_positives->Text == ""){
				MessageBox::Show("Please select positive image folder","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
			}
		}
				
		create_positives();
		picturebox_positives->Visible = true;
	}

	private: System::Void button_create_negative_file_Click(System::Object^  sender, System::EventArgs^  e) {
		// Make sure that the negative folder textbox is filled with a location
		if (textbox_negatives->Text == ""){
			MessageBox::Show("Please select negative image folder","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
		}
		
		create_negatives();
		picturebox_negatives->Visible = true;
	}

	// ---------------------------------------------------------------------------------------------------------------
	// Checkboxes for input data
	// ---------------------------------------------------------------------------------------------------------------
	private: System::Void checkbox_input_video_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if(checkbox_input_video->Checked){
			textbox_input_video->Enabled = true;
			button_select_video->Enabled = true;

			checkbox_input_images->Checked = false;
			checkbox_create_negatives->Checked = false;
			textbox_positives->Enabled = false;
			textbox_negatives->Enabled = false;
			button_select_positives->Enabled = false;
			button_select_negatives->Enabled = false;
			button_create_negative_file->Enabled = false;
		}
	}

	private: System::Void checkbox_input_images_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if(checkbox_input_images->Checked){
			checkbox_input_video->Checked = false;
			textbox_input_video->Enabled = false;
			button_select_video->Enabled = false;

			textbox_positives->Enabled = true;
			button_select_positives->Enabled = true;
			
			if(checkbox_create_negatives->Checked){
				textbox_negatives->Enabled = true;
				button_select_negatives->Enabled = true;
				button_create_negative_file->Enabled = true;
			}
		}
	}

	private: System::Void checkbox_create_negatives_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if(checkbox_create_negatives->Checked){
			textbox_negatives->Enabled = true;
			button_select_negatives->Enabled = true;
			button_create_negative_file->Enabled = true;
		}
		if(!checkbox_create_negatives->Checked){
			textbox_negatives->Enabled = false;
			button_select_negatives->Enabled = false;
			button_create_negative_file->Enabled = false;
		}
		if(checkbox_input_video->Checked){
			checkbox_create_negatives->Checked = false;
			textbox_negatives->Enabled = false;
			button_select_negatives->Enabled = false;
			button_create_negative_file->Enabled = false;
		}
	}

	// ---------------------------------------------------------------------------------------------------------------
	// Processing options
	// ---------------------------------------------------------------------------------------------------------------

	private: System::Void checkbox_subwindows_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if (checkbox_subwindows->Checked){
			textbox_x_direction->Enabled = true;
			textbox_y_direction->Enabled = true;
			textbox_overlap->Enabled = true;
		}
		if (!checkbox_subwindows->Checked){
			textbox_x_direction->Enabled = false;
			textbox_y_direction->Enabled = false;
			textbox_overlap->Enabled = false;
		}
	}

	// ---------------------------------------------------------------------------------------------------------------
	// Extra functions - open folders + reset folders
	// ---------------------------------------------------------------------------------------------------------------
	private: System::Void button_reset_form_Click(System::Object^  sender, System::EventArgs^  e) {
		// Reset all elements on the form to their initial state
		textbox_input_video->Text = "";
		textbox_positives->Text = ""; textbox_positives->Enabled = false;
		textbox_negatives->Text = ""; textbox_negatives->Enabled = false;

		checkbox_input_video->Checked = true;
		checkbox_input_images->Checked = false;
		checkbox_create_negatives->Checked = false;

		button_select_video->Enabled = true;
		button_select_positives->Enabled = false;
		button_select_negatives->Enabled = false;

		button_create_negative_file->Enabled = false;

		textbox_x_direction->Text = "500"; textbox_x_direction->Enabled = false;
		textbox_y_direction->Text = "500"; textbox_y_direction->Enabled = false;
		textbox_overlap->Text = "0"; textbox_overlap->Enabled = false;

		checkbox_subwindows->Checked = false;
		
		picturebox_positives->Visible = false;
		picturebox_negatives->Visible = false;

		checkbox_image_name->Checked = true;
		checkbox_amount_annotations->Checked = true;
		checkbox_boundingbox->Checked = true;
	}
	
	private: System::Void button_reset_root_Click(System::Object^  sender, System::EventArgs^  e) {
		// Define root folder from positives
		if (textbox_input_video->Text == "" && textbox_positives->Text == ""){
			MessageBox::Show("No way of defining root folder, since no input is selected.","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
		}else{
			// Define root folder
			define_root_folder();
			// Reset the root folder, removing old positive and negative text files
			// Done using the global variable root folder
			reset_target_folder();
		}				
	}

	private: System::Void button_open_positives_Click(System::Object^  sender, System::EventArgs^  e) {
			if(!(textbox_positives->Text == "")){
				// First collect the source folder to open
				String^ temp = textbox_positives->Text;
				IntPtr ip_temp= Marshal::StringToHGlobalAnsi(temp);
				const char* result = static_cast<const char*>(ip_temp.ToPointer());
				string text = result;
				// Combine into complete string
				string command = "explorer " + text;
				// Perform system command
				system(command.c_str());
			}else{
				MessageBox::Show("Positive folder not defined, so cannot be opened.","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
			}
	}

	private: System::Void button_open_negatives_Click(System::Object^  sender, System::EventArgs^  e) {
			if(!(textbox_negatives->Text == "")){
				// First collect the source folder to open
				String^ temp = textbox_negatives->Text;
				IntPtr ip_temp= Marshal::StringToHGlobalAnsi(temp);
				const char* result = static_cast<const char*>(ip_temp.ToPointer());
				string text = result;
				// Combine into complete string
				string command = "explorer " + text;
				// Perform system command
				system(command.c_str());
			}else{
				MessageBox::Show("Negative folder not defined, so cannot be opened.","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
			}

	}

	private: System::Void button_open_root_Click(System::Object^  sender, System::EventArgs^  e) {
			if (textbox_input_video->Text == "" && textbox_positives->Text == ""){
				MessageBox::Show("No way of defining root folder, since no input is selected.","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
			}else{
				// Define the root_folder
				define_root_folder();
				// Combine into complete string
				string command = "explorer " + root_folder;
				// Perform system command
				system(command.c_str());
			}
	}

	// -------------------------------------------------------------------------------
	// Own created functionality for annotating, negative file creation, ...
	// This functionality is selected from the handlers behind GUI elements
	// -------------------------------------------------------------------------------
	
	// FUNCTION : Define root folder
	void define_root_folder()
	{
		if( !(textbox_input_video->Text == "") ){
			// Retrieve text
			String^ temp_IN = textbox_input_video->Text;
			IntPtr ip_IN= Marshal::StringToHGlobalAnsi(temp_IN);
			const char* str_IN = static_cast<const char*>(ip_IN.ToPointer());

			// Remove filename --> get path
			stringstream temp (str_IN);
			string element;
			string path = "";
			vector<string> elements;
			while( getline(temp, element,'\\') ){
				elements.push_back(element);
			}
			for(int i=0; i < elements.size() - 1; i++){
				if (i == 0){
					path = path + elements[i];
				}
				if (i > 0){
					path = path + "\\" + elements[i]; 
				}
			}

			// Add directive to root folder
			root_folder = path;
		}else{
			// Retrieve text
			String^ temp_IN = textbox_positives->Text;
			IntPtr ip_IN= Marshal::StringToHGlobalAnsi(temp_IN);
			const char* str_IN = static_cast<const char*>(ip_IN.ToPointer());

			// Add directive to root folder
			string temp (str_IN);
			root_folder = temp + "\\..";

		}
	}

	// FUNCTION : goes into the target folder and resets it to a cleaned environment for processing training data
	void reset_target_folder(){
		string filename_pos = root_folder + "\\positives.txt"; remove(filename_pos.c_str());
		string filename_pos_t = root_folder + "\\positivesTemp.txt"; remove(filename_pos_t.c_str());
		string filename_neg = root_folder + "\\negatives.txt"; remove(filename_neg.c_str());
	}

	// FUNCTION : Function that is called to create annotations on positive object image data
	void create_positives()
	{	
		// Define root folder for outputting all data correctly
		define_root_folder();
		
		// Creation of usefull parameters
		int pressed_key = 0;
		string temp;
		
		// Conversion of winforms text data to proper string format for use in OpenCV [const char* = string]
		String^ temp_pos = textbox_positives->Text;
		IntPtr ip_pos = Marshal::StringToHGlobalAnsi(temp_pos);
		const char* str_pos = static_cast<const char*>(ip_pos.ToPointer());

		// Get file listing for elements in input folder [Windows operating system]
		// Creation of variables can be done for all cases, however, the processing should only be when doing image sequences
		WIN32_FIND_DATA ffd;
		TCHAR szDir[MAX_PATH];
		vector<string> filenames;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		// If input images are used, we need to retrieve the filenames
		if(checkbox_input_images-> Checked){
			// Copy string into buffer and append * for searching in folder
			StringCchCopy(szDir, MAX_PATH, str_pos);
			StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

			// Get first file - to use in do while loop
			hFind = FindFirstFile(szDir, &ffd);
			do
			{
				filenames.push_back (ffd.cFileName);	
			}
			while (FindNextFile(hFind, &ffd) != 0);
			// Remove the current and parent directory name (. and ..)
			filenames.erase(filenames.begin(),filenames.begin()+2);

			FindClose( hFind );
		}
		
		// Initialization of output annotation file that will be generated
		// A temp_output is generated in case something goes wrong and the app crashes in order to save work performed
		ofstream output, temp_output;	
		output.open(root_folder + "\\positives.txt");
		temp_output.open(root_folder + "\\positivesTemp.txt");
		
		// Get basic parameters - amount of frames of video - amount of images
		int size;
		int count = 0;
		cv::VideoCapture video_capture;

		// Retrieve video location
		// Loop through all frames to define the length of the video
		// --> this is done by loading all frames, since header information is sometimes wrong...
		String^ temp_video = textbox_input_video->Text;
		IntPtr ip_video = Marshal::StringToHGlobalAnsi(temp_video);
		const char* str_video = static_cast<const char*>(ip_video.ToPointer());

		video_capture.open(str_video);

		if (checkbox_input_video->Checked){
			// Perform calculation of the amount of frames
			int temp_counter = 0;
			cv::Mat frame;

			for(;;){
				video_capture >> frame;
				// Once all frames are used up, break out of loop!
				if(frame.empty()){
					break;
				}
				temp_counter++;
			}

			size = temp_counter;
		}
		if (checkbox_input_images->Checked){
			size = (int)filenames.size();
		}

		// Check which checkboxes are checked
		bool bool_image = checkbox_input_images->Checked;
		bool bool_video = checkbox_input_video->Checked;
		bool name = checkbox_image_name->Checked;
		bool amount = checkbox_amount_annotations->Checked;
		bool bounding = checkbox_boundingbox->Checked;
		// Bool segment is global, because the on_mouse action needs the value frequently
		segment = checkbox_subwindows->Checked;
		
		// Make it possible to jump out of the looping over image
		bool stop = false;
		
		// Since video processing is added, make sure that the current frame can be stored
		// Also reset the VideoCapture counter
		video_capture.set(CV_CAP_PROP_POS_FRAMES, 0.0);

		if(!video_capture.isOpened()){
			MessageBox::Show("ERRROOOOOOR!","Error occured",MessageBoxButtons::OK,MessageBoxIcon::Exclamation);
		}
		// Loop over each image and make it possible for people to draw bounding boxes
		for ( count = 0; count < size; count++)
		{
			// add the name of the file to the temporary string
			if(name && bool_video){
				temp_output << root_folder << "video_frame_" << count;
			}
			if(name && bool_image){
				temp_output << str_pos <<  "\\" << filenames[count];
			}
			
			num_of_rec = 0;
			string result_string = "";
					
			//  load in the original image or the current frame
			if(bool_image){
				stringstream image_location;
				image_location << str_pos << "\\" << filenames[count];
				image = cv::imread(image_location.str(), 1);
			}
			if(bool_video){
				video_capture >> image;
			}

			//	init highgui - make sure that mouse actions are coupled to the correct window
			cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
			cv::setMouseCallback(window_name, on_mouse);

			// functionality that is called when segmentation is required
			// this is usefull when input data is larger than the actual computer screen size - and thus cannot be visualised in whole
			if (segment){
				// push the original image into a copy named img_large to perform processing
				cv::Mat img_large(image); 
				
				// Retrieve parameters for segmentation
				String^ temp_horiz = textbox_x_direction->Text;
				String^ temp_vertic = textbox_y_direction->Text;
				String^ temp_overlap = textbox_overlap->Text;
				IntPtr ip_horiz = Marshal::StringToHGlobalAnsi(temp_horiz);
				IntPtr ip_vertic = Marshal::StringToHGlobalAnsi(temp_vertic);
				IntPtr ip_overlap = Marshal::StringToHGlobalAnsi(temp_overlap);
				const char* str_horiz = static_cast<const char*>(ip_horiz.ToPointer());
				const char* str_vertic = static_cast<const char*>(ip_vertic.ToPointer());
				const char* str_overlap = static_cast<const char*>(ip_overlap.ToPointer());

				int v_part = atoi(str_vertic);
				int h_part = atoi(str_horiz);
				int overlap = atoi(str_overlap);				
				cv::Size dimensions = img_large.size();
				
				// Variables and calculations needed for processing - to make sure that subwindows have correct regions
				cv::Mat dst_img;
				stringstream ouput_text;
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
							dst_img = img_large(cv::Rect(j*h_part,i*v_part,h_part,v_part));
							original_location_horizontal = j*h_part;
							original_location_vertical = i*v_part;
						// FIRST ROW case where we are at the last column - add extra space (overlap region)
						}else if(j == (steps_horizontal - 1) && i == 0 ){
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*v_part,h_part+rest_horizontal,v_part));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*v_part;
						// FIRST COLUMN case where we are at the last row - add extra space (overlap region)
						}else if(i == (steps_vertical - 1) && j == 0){
							dst_img = img_large(cv::Rect(j*h_part,i*(v_part-overlap),h_part,v_part+rest_vertical));
							original_location_horizontal = j*h_part;
							original_location_vertical = i*(v_part-overlap);
						// LAST ROW LAST COLUMN
						}else if(j == (steps_horizontal - 1) && i == (steps_vertical - 1)){
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*(v_part-overlap),h_part+rest_horizontal,v_part+rest_vertical));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*(v_part-overlap);
						// case where we are at the last column - add extra space (overlap region)
						}else if(j == (steps_horizontal - 1) ){
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*(v_part-overlap),h_part+rest_horizontal,v_part));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*(v_part-overlap);
						// case where we are at the last row - add extra space (overlap region)
						}else if(i == (steps_vertical - 1) ){
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*(v_part-overlap),h_part,v_part+rest_vertical));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*(v_part-overlap);
						// case of the first row
						}else if(j == 0 && i != 0){
							dst_img = img_large(cv::Rect(j*h_part,i*(v_part-overlap),h_part,v_part));
							original_location_horizontal = j*h_part;
							original_location_vertical = i*(v_part-overlap);
						// case of the first column
						}else if(j != 0 && i == 0){
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*v_part,h_part,v_part));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*v_part;
						// all the other cases 
						}else{
							dst_img = img_large(cv::Rect(j*(h_part-overlap),i*(v_part-overlap),h_part,v_part));
							original_location_horizontal = j*(h_part-overlap);
							original_location_vertical = i*(v_part-overlap);
						}
			
						ouput_text << "Image: " << count+1 << " Vertical: "<< i+1 << " Horizontal: " << j+1;
						// Add counter info to the image
						cv::putText(dst_img, ouput_text.str(), cv::Point(25,25), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,0),2);
						// Copy the partial Mat image to a central container
						// This way the mouse refresh tool can use this knowledge for refreshing purposes
						image_part = dst_img;
						cv::imshow(window_name,dst_img);
						cv::moveWindow(window_name, 50, 50);
						// Wait 15 ms to actually process the visualization - only works when a window is open!
						cv::waitKey(15);
						// Clear counter for new input each print
						ouput_text.str("");
						
						// For each segment of the image, process the frame based on keystroke instructions
						do
						{
							// used cv::waitKey returns:
							//  <Numpad .> = 46			save added rectangles and show next image
							//	<Numpad Enter> = 13		go to next window region
							//	<ESC> = 27				exit program
							pressed_key = cv::waitKey(0);

							switch(pressed_key)
							{
							// ESC was pressed --> exit the program
							case 27:
									cv::destroyWindow(window_name);
									stop = true;
									break;
							// Numpad dot was pressed --> add the rectangle to the current image
							case 46:
									temp_output << "\n";
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
										// append rectangle coord to previous line content
										if(bounding){
											result_string+=" "+int_to_string(roi_x0_new)+" "+int_to_string(roi_y0_new)+" "+int_to_string(roi_x1_new-roi_x0_new)+" "+int_to_string(roi_y1_new-roi_y0_new);
										}
									}
									// Draw initiated from bottom right corner
									if(roi_x0>roi_x1 && roi_y0>roi_y1)
									{
										// append rectangle coord to previous line content
										if(bounding){
											result_string+=" "+int_to_string(roi_x1_new)+" "+int_to_string(roi_y1_new)+" "+int_to_string(roi_x0_new-roi_x1_new)+" "+int_to_string(roi_y0_new-roi_y1_new);
										}
									}
									// Draw initiated from top right corner
									if(roi_x0>roi_x1 && roi_y0<roi_y1)
									{
										// append rectangle coord to previous line content
										if(bounding){
											result_string+=" "+int_to_string(roi_x1_new)+" "+int_to_string(roi_y0_new)+" "+int_to_string(roi_x0_new-roi_x1_new)+" "+int_to_string(roi_y1_new-roi_y0_new);
										}
									}
									// Draw initiated from bottom left corner
									if(roi_x0<roi_x1 && roi_y0>roi_y1)
									{
										// append rectangle coord to previous line content
										if(bounding){
											result_string+=" "+int_to_string(roi_x0_new)+" "+int_to_string(roi_y1_new)+" "+int_to_string(roi_x1_new-roi_x0_new)+" "+int_to_string(roi_y0_new-roi_y1_new);
										}
									}
									temp_output << result_string;
									cv::rectangle(image_part, cv::Point(roi_x0,roi_y0), cv::Point(roi_x1,roi_y1), cv::Scalar(0,255,0));
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
			// This is likewise but more simple then the above steps
			if (!segment){			
				cv::imshow(window_name,image);
				cv::moveWindow(window_name, 50, 50);

				do
				{
					// used cv::waitKey returns:
					//  <Numpad .> = 46			add rectangle to current image
					//	<Numpad Enter> = 13		save added rectangles and show next image
					//	<ESC> = 27				exit program
					pressed_key=cv::waitKey(0);

					switch(pressed_key)
					{
					case 27:
							cv::destroyWindow(window_name);
							stop = true;
					case 46:
							temp_output << "\n";
						    num_of_rec++;
							// Draw initiated from top left corner
							if(roi_x0<roi_x1 && roi_y0<roi_y1)
							{
								// append rectangle coord to previous line content
								if(bounding){
									result_string+=" "+int_to_string(roi_x0)+" "+int_to_string(roi_y0)+" "+int_to_string(roi_x1-roi_x0)+" "+int_to_string(roi_y1-roi_y0);
								}
							}
							// Draw initiated from bottom right corner
							if(roi_x0>roi_x1 && roi_y0>roi_y1)
							{
								// append rectangle coord to previous line content
								if(bounding){
									result_string+=" "+int_to_string(roi_x1)+" "+int_to_string(roi_y1)+" "+int_to_string(roi_x0-roi_x1)+" "+int_to_string(roi_y0-roi_y1);
								}
							}
							// Draw initiated from top right corner
							if(roi_x0>roi_x1 && roi_y0<roi_y1)
							{
								// append rectangle coord to previous line content
								if(bounding){
									result_string+=" "+int_to_string(roi_x1)+" "+int_to_string(roi_y0)+" "+int_to_string(roi_x0-roi_x1)+" "+int_to_string(roi_y1-roi_y0);
								}
							}
							// Draw initiated from bottom left corner
							if(roi_x0<roi_x1 && roi_y0>roi_y1)
							{
								// append rectangle coord to previous line content
								if(bounding){
									result_string+=" "+int_to_string(roi_x0)+" "+int_to_string(roi_y1)+" "+int_to_string(roi_x1-roi_x0)+" "+int_to_string(roi_y0-roi_y1);
								}
							}
							temp_output << result_string;
							cv::rectangle(image, cv::Point(roi_x0,roi_y0), cv::Point(roi_x1,roi_y1), cv::Scalar(0,255,0), 1);
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
				if(name && bool_video){
					output << root_folder << "video_frame_" << count;
				}
				if(name && bool_image){
					output << str_pos <<  "\\" << filenames[count];
				}
				if(amount){
					output << " " << num_of_rec;
				}
				output << result_string <<"\n";
			}

			temp_output << "\n";

			// Check again if an urgent stop was requested. If so also exit for loop that loops over examples.
			if(stop)
			{
				break;
			}
		}
		
		FindClose( hFind );
		output.close();
		cv::destroyWindow(window_name);
	}

	// FUNCTION : Function that is called to a negative image data summation file
	void create_negatives(){
		// This is done by a simple system command. This can be applied in any unix like system.
		String^ temp_neg = textbox_negatives->Text;
		IntPtr ip_neg= Marshal::StringToHGlobalAnsi(temp_neg);
		const char* str_neg = static_cast<const char*>(ip_neg.ToPointer());

		stringstream command_line;
		command_line << "cd " << str_neg << "\\" << " && " << "dir /s /b >> ..\\negatives.txt";	
		system(command_line.str().c_str());
	}
};
}

