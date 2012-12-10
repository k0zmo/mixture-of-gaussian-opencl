#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <string>

class FrameGrabber
{
public:
	virtual bool init(const std::string& stream) = 0;
	virtual void deinit() = 0;
	virtual cv::Mat grab(bool* success) = 0;
	virtual int frameWidth() const = 0;
	virtual int frameHeight() const = 0;
	virtual int frameFormat() const = 0;
	virtual bool needBayer() const = 0;
};

class OpenCvFrameGrabber : public FrameGrabber
{
public:
	OpenCvFrameGrabber();
	virtual ~OpenCvFrameGrabber();

	virtual bool init(const std::string& stream) override;
	virtual void deinit() override;
	virtual cv::Mat grab(bool* success) override;
	virtual int frameWidth() const override;
	virtual int frameHeight() const override;
	virtual int frameFormat() const override;
	virtual bool needBayer() const override;

private:
	cv::VideoCapture cap;
	int width,
		height,
		format;
};

/*

class SaperaFrameGrabber : FrameGrabber
{
public:

};

*/

