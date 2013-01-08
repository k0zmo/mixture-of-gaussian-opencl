#pragma once

#include <clw/clw.h>
#include "MixtureOfGaussianGPU.h"
#include "GrayscaleGPU.h"
#include "BayerFilterGPU.h"

class FrameGrabber;
class ConfigFile;

class WorkerGPU
{
public:
	WorkerGPU(const clw::Context& context,
		const clw::Device& device, 
		const clw::CommandQueue& queue,
		ConfigFile& cfg);
	bool init(const std::string& videoStream);
	clw::Event processFrame();
	bool grabFrame();

	const cv::Mat& finalFrame() const { return dstFrame; }
	const cv::Mat& sourceFrame() const { return srcFrame; }
	const cv::Mat& intermediateFrame() const { return interFrame; }

private:
	clw::Context context;
	clw::Device device;
	clw::CommandQueue queue;
	clw::Buffer clFrame;
	clw::Image2D clFrameGray;
	int inputFrameSize;
	int preprocess; // 0 - no preprocess (frame is gray)
	                // 1 - frame is rgb, grayscaling
	                // 2 - frame needs bayerFilter
	bool showIntermediateFrame;

	MixtureOfGaussianGPU mogGPU;
	GrayscaleGPU grayscaleGPU;
	BayerFilterGPU bayerFilterGPU;

	std::unique_ptr<FrameGrabber> grabber;
	cv::Mat srcFrame;
	cv::Mat dstFrame;
	cv::Mat interFrame;

	ConfigFile& cfg;
	float learningRate;

private:
	WorkerGPU(const WorkerGPU&);
	WorkerGPU& operator=(const WorkerGPU&);
};