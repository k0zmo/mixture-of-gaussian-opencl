#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

int main(int, char**)
{
	// Open sample video
	cv::VideoCapture cap("surveillance.webm");
	if(!cap.isOpened())
	{
		exit(-1);
	}

	// default parameters
	cv::BackgroundSubtractorMOG MOG(200, 5, 0.7, 30*0.5);

	for(;;)
	{
		// Get a new frame from camera
		cv::Mat frame;
		cap >> frame; 

		// Convert it to grayscale
		cvtColor(frame, frame, CV_BGR2GRAY);

		// Estimate the background
		cv::Mat	frameMask;
		MOG(frame, frameMask, -1);

		// Blend two images
		cv::Mat dst;
		cv::add(frameMask, frame, dst);

		// Show them
		cv::imshow("Mixture of Guassian", frameMask);
		cv::imshow("Original video", dst);

		// 'f' starts estimating from the beginning
		int key = cv::waitKey(30);
		if(key == 'f')
			MOG.initialize(frame.size(), frame.type());
		// and everything else quits 
		else if(key >= 0)
			break;
	}
}