#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>
#include <memory>

#include <clw/clw.h>

#include "QPCTimer.h"
#include "ConfigFile.h"
#include "FrameGrabber.h"

#include "MixtureOfGaussianGPU.h"
#include "GrayscaleGPU.h"
#include "BayerFilterGPU.h"

namespace clwutils
{
	std::vector<clw::Device> pickSingleDevice()
	{
		std::vector<clw::Platform> platforms = clw::availablePlatforms();
		if(platforms.empty())
		{
			std::cerr << "No OpenCL platfrom has been detected!\n";
			std::exit(-1);
		}

		clw::Platform platform;

		// Jesli mamy tylko jedna platforme to ja wybierz bez pytania
		if(platforms.size() == 1)
		{
			platform = platforms[0];
			std::cout << "Using OpenCL platform: " << platform.name() 
				<< ", " << platform.versionString() << std::endl;
		}
		else
		{
			std::cout << "List of available OpenCL platforms: \n";

			for(size_t index = 0; index < platforms.size(); ++index)
			{
				std::cout << "  " << index + 1 << ") " 
					<< platforms[index].name() << std::endl;
			}

			int choice = 0;
			while(choice > (int)(platforms.size()) || choice <= 0)
			{
				std::cout << "Choose OpenCL platform: ";
				std::cin >> choice;
				std::cin.clear();
				std::cin.sync();
			}
			platform = platforms[choice-1];
		}

		std::vector<clw::Device> devices = clw::devices(clw::All, platform);

		// Wybierz urzadzenie
		if(devices.size() == 1)
		{
			std::cout << "Using " << devices[0].name() << std::endl;
		}
		else
		{
			std::cout << "\nList of available devices:\n";
			for(size_t index = 0; index < devices.size(); ++index)
			{
				std::cout << "  " << index + 1 << ") " << devices[index].name()
					<< ", " << devices[index].vendor() << std::endl;
			}
			std::cout << "\n";

			int choice = 0;
			while(choice > (int)(devices.size()) || choice <= 0)
			{
				std::cout << "Choose OpenCL device: ";
				std::cin >> choice;
				std::cin.clear();
				std::cin.sync();
			}

			clw::Device dev = devices[choice-1];
			devices.clear();
			devices.push_back(dev);
		}

		return devices;
	}
}

class Worker
{
public:
	Worker(const clw::Context& context,
		const clw::Device& device, 
		const clw::CommandQueue& queue,
		ConfigFile& cfg)
		: context(context)
		, device(device)
		, queue(queue)
		, inputFrameSize(0)
		, mogGPU(context, device, queue)
		, grayscaleGPU(context, device, queue)
		, bayerFilterGPU(context, device, queue)
		, cfg(cfg)
	{
	}

	bool init(const std::string& videoStream)
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

		inputFrameSize = width * height * channels * sizeof(cl_uchar);
		clFrame = context.createBuffer
			(clw::Access_ReadOnly, clw::Location_Device, inputFrameSize);

		// Initialize MoG on GPU
		mogGPU.setMixtureParameters(200,
			std::stof(cfg.value("VarianceThreshold", "MogParameters")),
			std::stof(cfg.value("BackgroundRatio", "MogParameters")),
			std::stof(cfg.value("InitialWeight", "MogParameters")),
			std::stof(cfg.value("InitialVariance", "MogParameters")),
			std::stof(cfg.value("MinVariance", "MogParameters")));
		mogGPU.init(width, height, workGroupSizeX, workGroupSizeY, nmixtures);
		preprocess = 0;

		if(channels == 3)
		{
			// Initialize Grayscaling on GPU
			grayscaleGPU.init(width, height, workGroupSizeX, workGroupSizeY);
			preprocess = 1;
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
			bayerFilterGPU.init(width, height, workGroupSizeX, workGroupSizeX, bayer);
			preprocess = 2;
		}

		return true;
	}

	clw::Event processFrame()
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
		else
		{
			clw::Event e0 = queue.asyncWriteImage2D(sourceMogFrame, srcFrame.data, 0, 0, srcFrame.cols, srcFrame.rows);
		}
		
		clw::Event e2 = mogGPU.process(sourceMogFrame, learningRate);
		clw::Event e3 = queue.asyncReadImage2D(mogGPU.output(), dstFrame.data, 0, 0, dstFrame.cols, dstFrame.rows);
		return e3;
	}

	bool grabFrame()
	{
		bool success;
		srcFrame = grabber->grab(&success);
		return success;
	}

	const cv::Mat& finalFrame() const { return dstFrame; }
	const cv::Mat& sourceFrame() const { return srcFrame; }

private:
	clw::Context context;
	clw::Device device;
	clw::CommandQueue queue;
	clw::Buffer clFrame;
	int inputFrameSize;
	int preprocess; // 0 - no preprocess (frame is gray)
	                // 1 - frame is rgb, grayscaling
	                // 2 - frame needs bayerFilter

	MixtureOfGaussianGPU mogGPU;
	GrayscaleGPU grayscaleGPU;
	BayerFilterGPU bayerFilterGPU;

	std::unique_ptr<FrameGrabber> grabber;
	cv::Mat srcFrame;
	cv::Mat dstFrame;

	ConfigFile& cfg;
	float learningRate;

private:
	Worker(const Worker&);
	Worker& operator=(const Worker&);
};

int main(int, char**)
{
	ConfigFile cfg;
	if(!cfg.load("mixture-of-gaussian.cfg"))
	{
		std::cerr << "Can't load mixture-of-gaussian.cfg, qutting\n";
		std::cin.get();
		exit(-1);
	}
	std::string devpick = cfg.value("Device", "General");

	// Initialize OpenCL
	clw::Context context;

	if(devpick == "pick")
	{
		if(!context.create(clwutils::pickSingleDevice()))
		{
			std::cerr << "Couldn't create context, quitting\n";
			std::exit(-1);
		}
	}
	else
	{
		clw::EDeviceType type = clw::Default;
		if(devpick == "gpu")
			type = clw::Gpu;
		else if(devpick == "cpu")
			type = clw::Cpu;
		if(!context.create(type))
		{
			std::cerr << "Couldn't create context, quitting\n";
			std::exit(-1);
		}
	}
	clw::Device device = context.devices()[0];
	clw::CommandQueue queue = context.createCommandQueue
		(clw::Property_ProfilingEnabled, device);

	int numVideoStreams = 0;

	std::vector<bool> finish;
	std::vector<std::unique_ptr<Worker>> workers;
	std::vector<std::string> titles;

	for(int streamId = 1; streamId <= 5; ++streamId)
	{
		std::string cfgVideoStream = "VideoStream";
		cfgVideoStream += streamId + '0';
		std::string videoStream = cfg.value(cfgVideoStream, "General");

		if(!videoStream.empty())
		{
			auto worker = std::unique_ptr<Worker>(new Worker(context ,device, queue, cfg));
			if(!worker->init(videoStream))
				continue;

			workers.emplace_back(std::move(worker));
			titles.emplace_back(std::move(videoStream));
			finish.push_back(false);

			++numVideoStreams;
		}
	}

	if(numVideoStreams < 1)
	{
		std::wcout << "No video stream to process\n";
		std::cin.get();
		std::exit(-1);
	}

	QPCTimer timer;

	int frameInterval = std::stoi(cfg.value("FrameInterval", "General"));
	frameInterval = std::min(std::max(frameInterval, 1), 100);
	bool showSourceFrame = cfg.value("ShowSourceFrame", "General") == "yes";

	for(;;)
	{
		// Grab a new frame
		for(int i = 0; i < numVideoStreams; ++i)
			finish[i] = workers[i]->grabFrame();

		bool allFinish = false;
		for(int i = 0; i < numVideoStreams; ++i)
			allFinish = allFinish || finish[i];
		if(!allFinish)
			break;

		double start = timer.currentTime();

		for(int i = 0; i < numVideoStreams; ++i)
		{
			if(finish[i])
			{
				workers[i]->processFrame();
				queue.flush();
			}
		}
		queue.finish();

		double stop = timer.currentTime();

		std::cout << "Total time: " << (stop - start) * 1000.0 << " ms\n";

		for(int i = 0; i < numVideoStreams; ++i)
		{
			cv::imshow(titles[i], workers[i]->finalFrame());
			if(showSourceFrame)
				cv::imshow(titles[i] + " source", workers[i]->sourceFrame());
		}

		int totalTime = int(stop - start);

		int key = cv::waitKey(std::max(frameInterval, int((stop - start) * 1000)));
		if(key >= 0)
			break;
	}
}
