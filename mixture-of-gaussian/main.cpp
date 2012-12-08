#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>
#include <memory>

#include "QPCTimer.h"
#include "ConfigFile.h"

#include <clw/clw.h>

#include "MixtureOfGaussianGPU.h"
#include "GrayscaleGPU.h"

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
		, cfg(cfg)		
	{
	}

	bool init(int width, int height)
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

		// Input RGB image (as a clBuffer) - actually BGR
		const int channels = 3;
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

		// Initialize Grayscaling on GPU
		grayscaleGPU.init(width, height, workGroupSizeX, workGroupSizeY);

		return true;
	}


	clw::Event processFrame(const cv::Mat& frame, cv::Mat& dstFrame)
	{
		clw::Event e0 = queue.asyncWriteBuffer(clFrame, frame.data, 0, inputFrameSize);
		clw::Event e1 = grayscaleGPU.process(clFrame);
		clw::Event e2 = mogGPU.process(grayscaleGPU.output(), learningRate);
		clw::Event e3 = queue.asyncReadImage2D(mogGPU.output(), dstFrame.data, 0, 0, dstFrame.cols, dstFrame.rows);
		return e3;
	}

private:
	clw::Context context;
	clw::Device device;
	clw::CommandQueue queue;
	clw::Buffer clFrame;
	int inputFrameSize;

	MixtureOfGaussianGPU mogGPU;
	GrayscaleGPU grayscaleGPU;

	ConfigFile& cfg;
	float learningRate;

private:
	Worker(const Worker&);
	Worker& operator=(const Worker&);
};

struct WorkerData
{
	cv::VideoCapture cap;
	cv::Mat srcFrame;
	cv::Mat dstFrame;

	bool init(const std::string& videoStream)
	{
		// Check if video is opened
		cap.open(videoStream);
		if(!cap.isOpened())
		{
			std::cerr << "Can't load " << videoStream << ", qutting\n";
			return false;
		}

		// Retrieve frame size
		const int width = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
		const int height = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		const int format  = int(cap.get(CV_CAP_PROP_FORMAT));
		assert(format == CV_8U);

		// Output MoG Image
		dstFrame = cv::Mat(height, width, CV_8UC1);
		return true;
	}

	int width() { return int(cap.get(CV_CAP_PROP_FRAME_WIDTH)); }
	int height() { return int(cap.get(CV_CAP_PROP_FRAME_HEIGHT)); }

	bool grabFrame()
	{
		cap >> srcFrame;
		if(srcFrame.rows == 0 || srcFrame.cols == 0)
			return false;
		return true;
	}
};

namespace std
{
	template<class _Ty> inline
	unique_ptr<_Ty> make_unique()
	{	
		_Ty* _Rx = new _Ty();
		unique_ptr<_Ty> _Ret(_Rx);
		return _Ret;
	}

	template<class _Ty, class _Arg0, class _Arg1, class _Arg2, class _Arg3> inline
	unique_ptr<_Ty> make_unique(_Arg0&& _Ax0, _Arg1&& _Ax1, _Arg2&& _Ax2, _Arg3&& _Ax3)
	{	
		_Ty* _Rx = new _Ty(std::forward<_Arg0>(_Ax0), std::forward<_Arg1>(_Ax1), std::forward<_Arg2>(_Ax2), std::forward<_Arg3>(_Ax3));
		unique_ptr<_Ty> _Ret(_Rx);
		return _Ret;
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
	std::vector<std::unique_ptr<WorkerData>> workersData;
	std::vector<std::string> titles;

	for(int streamId = 1; streamId <= 5; ++streamId)
	{
		std::string cfgVideoStream = "VideoStream";
		cfgVideoStream += streamId + '0';
		std::string videoStream = cfg.value(cfgVideoStream, "General");

		if(!videoStream.empty())
		{
			++numVideoStreams;
			auto workerData = std::make_unique<WorkerData>();//std::unique_ptr<WorkerData>(new WorkerData);
			if(!workerData->init(videoStream))
				continue;

			auto worker = std::make_unique<Worker>(context ,device, queue, cfg);
			if(!worker->init(workerData->width(), workerData->height()))
				continue;

			workers.emplace_back(std::move(worker));
			workersData.emplace_back(std::move(workerData));
			finish.push_back(false);

			titles.emplace_back(std::move(videoStream));
		}
	}

	if(numVideoStreams < 1)
	{
		std::wcout << "videoStream is not defined at least once\n";
		std::cin.get();
		std::exit(-1);
	}

	QPCTimer timer;

	int frameInterval = std::stoi(cfg.value("FrameInterval", "General"));
	frameInterval = std::min(std::max(frameInterval, 1), 100);

	for(;;)
	{
		// Grab a new frame
		for(int i = 0; i < numVideoStreams; ++i)
			finish[i] = workersData[i]->grabFrame();

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
				workers[i]->processFrame(workersData[i]->srcFrame, workersData[i]->dstFrame);
				queue.flush();
			}
		}
		queue.finish();

		double stop = timer.currentTime();

		std::cout << "Total time: " << (stop - start) * 1000.0 << " ms\n";

		for(int i = 0; i < numVideoStreams; ++i)
		{
			cv::imshow(titles[i], workersData[i]->dstFrame);
		}

		int totalTime = int(stop - start);

		int key = cv::waitKey(std::min(frameInterval, frameInterval - int((stop - start) * 1000)));
		if(key >= 0)
			break;
	}
}
