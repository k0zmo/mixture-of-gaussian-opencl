#include "WorkerGPU.h"
#include "ConfigFile.h"
#include "FrameGrabber.h"

WorkerGPU::WorkerGPU(const clw::Context& context,
	const clw::Device& device, 
	const clw::CommandQueue& queue,
	ConfigFile& cfg)
	: context(context)
	, device(device)
	, queue(queue)
	, inputFrameSize(0)
	, showIntermediateFrame(false)
	, mogGPU(context, device, queue)
	, grayscaleGPU(context, device, queue)
	, bayerFilterGPU(context, device, queue)
	, cfg(cfg)
{
}

bool WorkerGPU::init(const std::string& videoStream)
{
	learningRate = std::stof(cfg.value("LearningRate", "MogParameters"));
	const int nmixtures = std::stoi(cfg.value("NumMixtures", "MogParameters"));
	if(nmixtures <= 0)
	{
		std::cerr << "Parameter NumMixtures is wrong, must be more than 0\n";
		return false;
	}

	int workGroupSizeX = std::stoi(cfg.value("X", "WorkGroupSize"));
	int workGroupSizeY = std::stoi(cfg.value("Y", "WorkGroupSize"));

	if(workGroupSizeX <= 0 || workGroupSizeY <= 0)
	{
		std::cerr << "Parameter X or Y in WorkGroupSize is wrong, must be more than 0\n";
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

	// Initialize MoG on GPU
	mogGPU.setMixtureParameters(200,
		std::stof(cfg.value("VarianceThreshold", "MogParameters")),
		std::stof(cfg.value("BackgroundRatio", "MogParameters")),
		std::stof(cfg.value("InitialWeight", "MogParameters")),
		std::stof(cfg.value("InitialVariance", "MogParameters")),
		std::stof(cfg.value("MinVariance", "MogParameters")));
	mogGPU.init(width, height, workGroupSizeX, workGroupSizeY, nmixtures);

	std::cout << "\n  frame width: " << width <<
		"\n  frame height: " << height << 
		"\n  num channels: " << channels << "x" << grabber->framePixelDepth() << " bits \n";
	inputFrameSize = width * height * channels * sizeof(cl_uchar);

	if(channels == 3)
	{
		std::cout << "  preprocessing frame: grayscalling\n";

		// Initialize Grayscaling on GPU
		grayscaleGPU.init(width, height, workGroupSizeX, workGroupSizeY);
		preprocess = 1;

		clFrame = context.createBuffer
			(clw::Access_ReadOnly, clw::Location_Device, inputFrameSize);
	}
	else if(channels == 1 && grabber->needBayer())
	{
		std::string bayerCfg = cfg.value("Bayer", "General");
		EBayerFilter bayer;
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

		bayerFilterGPU.init(width, height, workGroupSizeX, workGroupSizeX, bayer);
		preprocess = 2;

		clFrame = context.createBuffer
			(clw::Access_ReadOnly, clw::Location_Device, inputFrameSize);
	}
	else
	{
		std::cout << "  preprocessing frame: none (already monochrome format)\n";
		preprocess = 0;
		clFrameGray = context.createImage2D(
			clw::Access_ReadOnly, clw::Location_Device,
			clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8), width, height);
	}

	showIntermediateFrame = cfg.value("ShowIntermediateFrame", "General") == "yes";
	if(showIntermediateFrame)
		interFrame = cv::Mat(height, width, CV_8UC1);

	return true;
}

clw::Event WorkerGPU::processFrame()
{
	clw::Image2D sourceMogFrame;

	// Grayscaling
	if(preprocess == 1)
	{
		clw::Event e0 = queue.asyncWriteBuffer(clFrame, srcFrame.data, 0, inputFrameSize);
		clw::Event e1 = grayscaleGPU.process(clFrame);
		sourceMogFrame = grayscaleGPU.output();
	}
	// Bayer filter
	else if(preprocess == 2)
	{
		clw::Event e0 = queue.asyncWriteBuffer(clFrame, srcFrame.data, 0, inputFrameSize);
		clw::Event e1 = bayerFilterGPU.process(clFrame);
		sourceMogFrame = bayerFilterGPU.output();
	}
	// Passthrough
	else
	{
		clw::Event e0 = queue.asyncWriteImage2D(clFrameGray, srcFrame.data, 0, 0, srcFrame.cols, srcFrame.rows);
		sourceMogFrame= clFrameGray;
	}

	if(showIntermediateFrame && preprocess != 0)
	{
		queue.asyncReadImage2D(sourceMogFrame, interFrame.data, 0, 0, interFrame.cols, interFrame.rows);
	}
		
	clw::Event e2 = mogGPU.process(sourceMogFrame, learningRate);
	clw::Event e3 = queue.asyncReadImage2D(mogGPU.output(), dstFrame.data, 0, 0, dstFrame.cols, dstFrame.rows);
	return e3;
}

bool WorkerGPU::grabFrame()
{
	bool success;
	srcFrame = grabber->grab(&success);
	return success;
}