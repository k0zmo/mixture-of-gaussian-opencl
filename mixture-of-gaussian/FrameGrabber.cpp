#include "FrameGrabber.h"

#include <opencv2/highgui/highgui.hpp>

OpenCvFrameGrabber::OpenCvFrameGrabber()
{
	// nothing
}

OpenCvFrameGrabber::~OpenCvFrameGrabber()
{
	deinit();
}

bool OpenCvFrameGrabber::init(const std::string& stream)
{
	size_t idx;
	int device;

	try
	{
		device = std::stoi(stream, &idx);
		if(idx < stream.length())
			device = 1;
	}
	catch(std::invalid_argument&)
	{
		device = -1;
	}

	// Check if video is opened
	if(device != -1)
		cap.open(device);
	else
		cap.open(stream);

	if(!cap.isOpened())
	{
		std::cerr << "Can't load " << stream << ", qutting\n";
		return false;
	}

	// Retrieve frame size
	width = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	height = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	format  = int(cap.get(CV_CAP_PROP_FORMAT));

	return true;
}

void OpenCvFrameGrabber::deinit()
{
	// nothing to do
}

cv::Mat OpenCvFrameGrabber::grab(bool* success)
{
	cv::Mat frame;
	cap >> frame;
	if(success != nullptr)
	{
		if(frame.rows == 0 || frame.cols == 0)
			*success = false;
		else
			*success = true;
	}
	return frame;
}

int OpenCvFrameGrabber::frameWidth() const
{ return width; }
int OpenCvFrameGrabber::frameHeight() const
{ return height; }
int OpenCvFrameGrabber::frameFormat() const
{ return format; }
bool OpenCvFrameGrabber::needBayer() const
{ return false; }