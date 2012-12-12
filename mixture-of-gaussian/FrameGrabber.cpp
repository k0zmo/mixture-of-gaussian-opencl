#include "FrameGrabber.h"

#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <iostream>

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

	// HACK
	cv::Mat dummy;
	cap >> dummy;

	// Retrieve frame size
	width = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	height = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	
	pixelDepth  = dummy.depth();
	numChannels = dummy.channels();

	switch(pixelDepth)
	{
	case CV_8U: 
	case CV_8S: 
		pixelDepth = 8;
		break;
	case CV_16U:
	case CV_16S:
		pixelDepth = 16;
		break;
	case CV_32S:
	case CV_32F:
		pixelDepth = 32;
		break;
	case CV_64F:
		pixelDepth = 64;
		break;
	}

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
int OpenCvFrameGrabber::frameNumChannels() const
{ return numChannels; }
int OpenCvFrameGrabber::framePixelDepth() const
{ return pixelDepth; }
bool OpenCvFrameGrabber::needBayer() const
{ return false; }

#if defined(SAPERA_SUPPORT)
#	include "SapClassBasic.h"
#	ifdef _DEBUG
#		pragma comment(lib, "SapClassBasicD.lib")
#	else
#		pragma comment(lib, "SapClassBasic.lib")
#	endif

SaperaFrameGrabber::SaperaFrameGrabber()
	: acq(nullptr)
	, buffer(nullptr)
	, xfer(nullptr)
	, image(nullptr)
{
}

SaperaFrameGrabber::~SaperaFrameGrabber()
{
	deinit();
}

bool SaperaFrameGrabber::init(const std::string& stream)
{
	SapLocation loc("X64-CL_iPro_1", 0);
	const char* cfg_file = stream.c_str();

	acq = new SapAcquisition(loc, cfg_file);
	buffer = new SapBuffer(1, acq);
	xfer = new SapAcqToBuf(acq, buffer);

	// Crate acquisition object
	if(acq && !acq->Create())
	{
		std::cout << "Create Acq error.\n";
		deinit();
		return false;
	}

	// Create buffer object
	if(buffer && !buffer->Create())
	{
		std::cout << "Create Buffers error.\n";
		deinit();
		return false;
	}

	// Create transfer object
	if (xfer && !xfer->Create())
	{
		std::cout << "Create Xfer error.\n";
		deinit();
		return false;
	}

	// Not sure if that's ok
	image = cvCreateImage(cvSize(buffer->GetWidth(), buffer->GetHeight()), IPL_DEPTH_16U, 1);
	dummyImageData = image->imageData;
	return true;
}

void SaperaFrameGrabber::deinit()
{
	// Close camera
	if(xfer)
	{
		xfer->Freeze();
		xfer->Wait(5000);
	}

	// Destroy transfer object
	if (xfer && !xfer->Destroy()) 
		return;

	// Destroy buffer object
	if (buffer && !buffer->Destroy())
		return;

	// Destroy acquisition object
	if (acq && !acq->Destroy()) 
		return;

	// Delete all objects
	if (xfer) delete xfer;
	if (buffer) delete buffer; 
	if (acq) delete acq; 

	image->imageData = dummyImageData;
	cvReleaseImage(&image);

	acq = nullptr;
	buffer = nullptr;
	xfer = nullptr;
	image = nullptr;
}

cv::Mat SaperaFrameGrabber::grab(bool* success)
{
	if(!xfer || !buffer)
	{
		if(success)
			*success = false;
		return cv::Mat();
	}

	// Grab a frame
	bool ret = xfer->Snap() > 0;
	ret &= xfer->Wait(1000) > 0;

	void* data;
	ret &= buffer->GetAddress(&data) > 0;
	image->imageData = static_cast<char*>(data);
	cv::Mat frame(image);

	if(success)
		*success = ret;

	if(frame.depth() != CV_8U)
	{
		cv::Mat tmp = frame.clone();
		tmp.convertTo(frame, CV_8UC1, 0.0625);
	}

	//// Nie wiem czy to do konca jest poprawne
	//if(frame.channels() != 1)
	//{
	//	int code = (frame.channels() == 3)
	//		? CV_BGR2GRAY
	//		: CV_BGRA2GRAY;
	//	cvtColor(frame, frame, code);
	//}

	return frame;
}

int SaperaFrameGrabber::frameWidth() const
{
	if(buffer)
		return buffer->GetWidth();
	return 0;
}

int SaperaFrameGrabber::frameHeight() const
{
	if(buffer)
		return buffer->GetHeight();
	return 0;
}

int SaperaFrameGrabber::frameNumChannels() const
{
	// NOT yet tested
	if(buffer)
		return (buffer->GetBytesPerPixel() * 8) / buffer->GetPixelDepth();
	return 0;
}

int SaperaFrameGrabber::framePixelDepth() const
{
	if(buffer)
		return buffer->GetPixelDepth();
	return 0;
}

bool SaperaFrameGrabber::needBayer() const
{
	// NOT yet tested
	if(buffer)
	{
		if(buffer->GetBytesPerPixel() > 2)
			return false;
		return true;
	}
	return false;
}

#endif