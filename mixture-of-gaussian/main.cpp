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

#include "WorkerCPU.h"
#include "WorkerGPU.h"

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

void mainCPU(ConfigFile& cfg)
{
	int numVideoStreams = 0;

	std::vector<bool> finish;
	std::vector<std::unique_ptr<WorkerCPU>> workers;
	std::vector<std::string> titles;

	for(int streamId = 1; streamId <= 5; ++streamId)
	{
		std::string cfgVideoStream = "VideoStream";
		cfgVideoStream += streamId + '0';
		std::string videoStream = cfg.value(cfgVideoStream, "General");

		if(!videoStream.empty())
		{
			auto worker = std::unique_ptr<WorkerCPU>(new WorkerCPU(cfg));
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
	bool showIntermediateFrame = cfg.value("ShowIntermediateFrame", "General") == "yes";
	std::cout << "\n";

	double start = timer.currentTime();

	for(;;)
	{
		double oldStart = start;
		start = timer.currentTime();

		std::cout << "Time between consecutive frames: " << (start - oldStart) * 1000.0 << " ms\n";

		// Grab a new frame
		for(int i = 0; i < numVideoStreams; ++i)
			finish[i] = workers[i]->grabFrame();

		bool allFinish = false;
		for(int i = 0; i < numVideoStreams; ++i)
			allFinish = allFinish || finish[i];
		if(!allFinish)
			break;

		start = timer.currentTime();

		for(int i = 0; i < numVideoStreams; ++i)
		{
			if(finish[i])
			{
				workers[i]->processFrame();
			}
		}

		double stop = timer.currentTime();

		std::cout << "Total processing and transfer time: " << (stop - start) * 1000.0 << " ms\n\n";

		for(int i = 0; i < numVideoStreams; ++i)
		{
			cv::imshow(titles[i], workers[i]->finalFrame());
			if(showSourceFrame)
				cv::imshow(titles[i] + " source", workers[i]->sourceFrame());
			if(showIntermediateFrame)
				cv::imshow(titles[i] + " intermediate frame", workers[i]->intermediateFrame());
		}

		int time = int((stop - start) * 1000);
		int delay = std::max(1, frameInterval - time);

		int key = cv::waitKey(delay);
		if(key >= 0)
			break;
	}
}

void mainGPU(ConfigFile& cfg)
{
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
	std::vector<std::unique_ptr<WorkerGPU>> workers;
	std::vector<std::string> titles;

	for(int streamId = 1; streamId <= 5; ++streamId)
	{
		std::string cfgVideoStream = "VideoStream";
		cfgVideoStream += streamId + '0';
		std::string videoStream = cfg.value(cfgVideoStream, "General");

		if(!videoStream.empty())
		{
			auto worker = std::unique_ptr<WorkerGPU>(new WorkerGPU(context ,device, queue, cfg));
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
	bool showIntermediateFrame = cfg.value("ShowIntermediateFrame", "General") == "yes";
	std::cout << "\n";

	double start = timer.currentTime();

	for(;;)
	{
		double oldStart = start;
		start = timer.currentTime();

		std::cout << "Time between consecutive frames: " << (start - oldStart) * 1000.0 << " ms\n";

		// Grab a new frame
		for(int i = 0; i < numVideoStreams; ++i)
			finish[i] = workers[i]->grabFrame();

		bool allFinish = false;
		for(int i = 0; i < numVideoStreams; ++i)
			allFinish = allFinish || finish[i];
		if(!allFinish)
			break;

		start = timer.currentTime();

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

		std::cout << "Total processing and transfer time: " << (stop - start) * 1000.0 << " ms\n\n";

		for(int i = 0; i < numVideoStreams; ++i)
		{
			cv::imshow(titles[i], workers[i]->finalFrame());
			if(showSourceFrame)
				cv::imshow(titles[i] + " source", workers[i]->sourceFrame());
			if(showIntermediateFrame)
				cv::imshow(titles[i] + " intermediate frame", workers[i]->intermediateFrame());
		}

		int time = int((stop - start) * 1000);
		int delay = std::max(1, frameInterval - time);

		int key = cv::waitKey(delay);
		if(key >= 0)
			break;
	}
}

int main(int, char**)
{
	ConfigFile cfg;
	if(!cfg.load("mixture-of-gaussian.cfg"))
	{
		std::cerr << "Can't load mixture-of-gaussian.cfg, qutting\n";
		std::cin.get();
		exit(-1);
	}

	if(cfg.value("OpenCL", "General") == "yes")
		mainGPU(cfg);
	else
		mainCPU(cfg);
}
