#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>

#include "QPCTimer.h"
#include "MixtureOfGaussianCPU.h"

int main(int, char**)
{
	// Open sample video
	cv::VideoCapture cap("surveillance.webm");
	//cv::VideoCapture cap("video.avi");
	if(!cap.isOpened())
	{
		exit(-1);
	}

	// Retrieve frame size
	const int cols = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	const int rows = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	MixtureOfGaussianCPU MOG(rows, cols);
	//cv::BackgroundSubtractorMOG MOG;
	QPCTimer timer;;

	for(;;)
	{
		// Get a new frame from camera
		cv::Mat frame;
		cap >> frame; 

		if(frame.rows == 0 || frame.cols == 0)
			break;

		// Convert it to grayscale
		cvtColor(frame, frame, CV_BGR2GRAY);

		// Estimate the background
		double start = timer.currentTime();

		cv::Mat	frameMask;
		MOG(frame, frameMask, -1);

		double stop = timer.currentTime();

		std::cout << 1.0 / (stop - start) << std::endl;		

		// Show them
		cv::imshow("Mixture of Guassian", frameMask);
		cv::imshow("Original video", frame);

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
}