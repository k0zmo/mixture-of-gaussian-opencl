#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>

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

	void init(int stream)
	{
		std::string cfgVideoStream = "VideoStream";
		cfgVideoStream += stream + '0';

		std::string videoStream = cfg.value(cfgVideoStream, "General");

		// Check if video is opened
		cap.open(videoStream);
		if(!cap.isOpened())
		{
			std::cerr << "Can't load " << videoStream << ", qutting\n";
			std::cin.get();
			exit(-1);
		}

		// Retrieve frame size
		cols = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
		rows = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		const int format  = int(cap.get(CV_CAP_PROP_FORMAT));
		const int channels = 3; // !TODO
		assert(format == CV_8U);

		// CV_CAP_PROP_FORMAT zwraca tylko format danych (np. CV_8U),
		// bez liczby kanalow
		//cv::Mat dummy;
		//cap.read(dummy);
		//int channels = dummy.channels();
		//int depth = dummy.depth();

		learningRate = std::stof(cfg.value("LearningRate", "MogParameters"));
		nmixtures = std::stoi(cfg.value("NumMixtures", "MogParameters"));
		if(nmixtures <= 0)
		{
			std::cerr << "Parameter NumMixtures is wrong, must be more than 0\n";
			std::cin.get();
			std::exit(-1);
		}

		int workGroupSizeX = std::stoi(cfg.value("X", "WorkGroupSize"));
		int workGroupSizeY = std::stoi(cfg.value("Y", "WorkGroupSize"));

		if(workGroupSizeX <= 0 || workGroupSizeY <= 0)
		{
			std::cerr << "Parameter X or Y in WorkGroupSize is wrong, must be more than 0\n";
			std::cin.get();
			std::exit(-1);
		}

		// Input RGB image (as a clBuffer) - actually BGR
		inputFrameSize = rows * cols * channels * sizeof(cl_uchar);
		clFrame = context.createBuffer
			(clw::Access_ReadOnly, clw::Location_Device, inputFrameSize);

		// Output MoG Image
		dstFrame = cv::Mat(rows, cols, CV_8UC1);

		// Initialize MoG on GPU
		mogGPU.setMixtureParameters(200,
			std::stof(cfg.value("VarianceThreshold", "MogParameters")),
			std::stof(cfg.value("BackgroundRatio", "MogParameters")),
			std::stof(cfg.value("InitialWeight", "MogParameters")),
			std::stof(cfg.value("InitialVariance", "MogParameters")),
			std::stof(cfg.value("MinVariance", "MogParameters")));
		mogGPU.init(cols, rows, workGroupSizeX, workGroupSizeY, nmixtures);

		// Initialize Grayscaling on GPU
		grayscaleGPU.init(cols, rows, workGroupSizeX, workGroupSizeY);
	}

	bool newFrame()
	{
		cap >> frame;
		if(frame.rows == 0 || frame.cols == 0)
			return false;
		return true;
	}

	const cv::Mat& inputFrame() const { return frame; }
	const cv::Mat& mogFrame() const { return dstFrame; }

	clw::Event processFrame()
	{
		clw::Event e0 = queue.asyncWriteBuffer(clFrame, frame.data, 0, inputFrameSize);

		clw::Event e1 = grayscaleGPU.process(clFrame);
		clw::Image2D grayFrame = grayscaleGPU.output();

		clw::Event e2 = mogGPU.process(grayFrame, learningRate);
		clw::Image2D outputImage = mogGPU.output();

		clw::Event e3 = queue.asyncReadImage2D(outputImage, dstFrame.data, 0, 0, cols, rows);

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
	cv::VideoCapture cap;
	cv::Mat frame;
	cv::Mat dstFrame;
	int rows, cols, channels;
	int nmixtures;
	float learningRate;
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

	Worker w1(context, device, queue, cfg);
	Worker w2(context, device, queue, cfg);
	Worker w3(context, device, queue, cfg);

	w1.init(1);
	w2.init(2);
	w3.init(3);

	QPCTimer timer;

	for(;;)
	{
		bool f1 = w1.newFrame();
		bool f2 = w2.newFrame();
		bool f3 = w3.newFrame();

		if(!f1 && !f2 && !f3)
			break;

		double start = timer.currentTime();

		if(f1)
		{
			w1.processFrame();
			queue.flush();
		}
		if(f2)
		{
			w2.processFrame();
			queue.flush();
		}
		if(f3)
		{
			w3.processFrame();
			queue.flush();
		}
		queue.finish();

		double stop = timer.currentTime();

		std::cout << "Total time: " << (stop - start) * 1000 << " ms\n";

		//cv::imshow("Input1", w1.inputFrame());
		cv::imshow("MoG1", w1.mogFrame());

		//cv::imshow("Input2", w2.inputFrame());
		cv::imshow("MoG2", w2.mogFrame());

		//cv::imshow("Input2", w2.inputFrame());
		cv::imshow("MoG3", w3.mogFrame());

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
}
