#include "WorkerCPU.h"

#include "ConfigFile.h"
#include "FrameGrabber.h"

#include <memory>

enum EBayerFilter
{
	Bayer_RG,
	Bayer_BG,
	Bayer_GR,
	Bayer_GB
};

WorkerCPU::WorkerCPU(ConfigFile& cfg)
	: showIntermediateFrame(false)
	, cfg(cfg)
{}

bool WorkerCPU::init(const std::string& videoStream)
{
	learningRate = std::stof(cfg.value("LearningRate", "MogParameters"));
	const int nmixtures = std::stoi(cfg.value("NumMixtures", "MogParameters"));
	if(nmixtures <= 0)
	{
		std::cerr << "Parameter NumMixtures is wrong, must be more than 0\n";
		return false;
	}

	// Inicjalizuj frame grabbera
#if defined(SAPERA_SUPPORT)
	// Sprawdz suffix videoStream (.ccf)
	size_t pos = videoStream.find_last_of(".ccf");
	if(pos+1 == videoStream.length())
	{
		grabber = std::unique_ptr<FrameGrabber>(new SaperaFrameGrabber());
	}
	else
#endif
	{
		grabber = std::unique_ptr<FrameGrabber>(new OpenCvFrameGrabber());
	}

	if(!grabber->init(videoStream))
		return false;
	dstFrame = cv::Mat(grabber->frameHeight(), grabber->frameWidth(), CV_8UC1);

	// Pobierz dane o formacie ramki
	int width = grabber->frameWidth();
	int height = grabber->frameHeight();
	int channels = grabber->frameNumChannels();

	mog = cv::BackgroundSubtractorMOG(200, nmixtures,
		std::stof(cfg.value("BackgroundRatio", "MogParameters")));
	mog.initialize(cv::Size(height, width), CV_8UC1);

	std::cout << "\n  frame width: " << width <<
		"\n  frame height: " << height << 
		"\n  num channels: " << channels << "x" << grabber->framePixelDepth() << " bits \n";
	//inputFrameSize = width * height * channels * sizeof(cl_uchar);

	if(channels == 3)
	{
		std::cout << "  preprocessing frame: grayscalling\n";
		preprocess = 1;
	}
	else if(channels == 1 && grabber->needBayer())
	{
		std::string bayerCfg = cfg.value("Bayer", "General");
		if(bayerCfg == "RG") bayer = Bayer_RG;
		else if(bayerCfg == "BG") bayer = Bayer_BG;
		else if(bayerCfg == "GR") bayer = Bayer_GR;
		else if(bayerCfg == "GB") bayer = Bayer_GB;
		else
		{
			std::cerr << "Unknown 'Bayer' parameter (must be RG, BG, GR or GB)";
			return false;
		}
		std::cout << "  preprocessing frame: bayer " << bayerCfg << "\n";
		preprocess = 2;
	}
	else
	{
		std::cout << "  preprocessing frame: none (already monochrome format)\n";
		preprocess = 0;
	}

	showIntermediateFrame = cfg.value("ShowIntermediateFrame", "General") == "yes";
	if(showIntermediateFrame)
		interFrame = cv::Mat(height, width, CV_8UC1);

	return true;
}

void WorkerCPU::processFrame()
{
	cv::Mat sourceMogFrame;

	// Grayscalling
	if(preprocess == 1)
	{
		cv::cvtColor(srcFrame, sourceMogFrame, CV_BGR2GRAY);
	}
	// Bayer filter
	else if(preprocess == 2)
	{
		switch(bayer)
		{
		case Bayer_RG: cv::cvtColor(srcFrame, sourceMogFrame, CV_BayerRG2GRAY); break;
		case Bayer_BG: cv::cvtColor(srcFrame, sourceMogFrame, CV_BayerBG2GRAY); break;
		case Bayer_GR: cv::cvtColor(srcFrame, sourceMogFrame, CV_BayerGR2GRAY); break;
		case Bayer_GB: cv::cvtColor(srcFrame, sourceMogFrame, CV_BayerGB2GRAY); break;
		}			
	}
	// Passthrough
	else
	{
		sourceMogFrame = srcFrame;
	}

	if(showIntermediateFrame && preprocess != 0)
		interFrame = sourceMogFrame;

	mog(sourceMogFrame, dstFrame, learningRate);
}

bool WorkerCPU::grabFrame()
{
	bool success;
	srcFrame = grabber->grab(&success);
	return success;
}