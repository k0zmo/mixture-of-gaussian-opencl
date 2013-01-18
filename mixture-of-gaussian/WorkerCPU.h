#pragma once

#include <string>
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

class FrameGrabber;
class ConfigFile;

class WorkerCPU
{
public:
	WorkerCPU(ConfigFile& cfg);
	bool init(const std::string& videoStream);
	void processFrame();
	bool grabFrame();

	const cv::Mat& finalFrame() const { return dstFrame; }
	const cv::Mat& sourceFrame() const { return srcFrame; }
	const cv::Mat& intermediateFrame() const { return interFrame; }

private:
	int preprocess; // 0 - no preprocess (frame is gray)
	                // 1 - frame is rgb, grayscaling
	                // 2 - frame needs bayerFilter
	bool showIntermediateFrame;

	std::unique_ptr<FrameGrabber> grabber;
	cv::Mat srcFrame;
	cv::Mat dstFrame;
	cv::Mat interFrame;

	int bayer;
	cv::BackgroundSubtractorMOG mog;

	ConfigFile& cfg;
	float learningRate;

private:
	WorkerCPU(const WorkerCPU&);
	WorkerCPU& operator=(const WorkerCPU&);
};