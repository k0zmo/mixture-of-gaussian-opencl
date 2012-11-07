#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>

#include "QPCTimer.h"
#include "MixtureOfGaussianCPU.h"

#include "clw/clw.h"

std::vector<clw::Device> getSingleDevice()
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

int main(int, char**)
{
	// Open sample video
	cv::VideoCapture cap("video-cfr23.mp4");
	if(!cap.isOpened())
	{
		exit(-1);
	}

	// Retrieve frame size
	const int cols    = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	const int rows    = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	const int format  = int(cap.get(CV_CAP_PROP_FORMAT));

	assert(format == CV_8U);

	// CV_CAP_PROP_FORMAT zwraca tylko format danych (np. CV_8U),
	// bez liczby kanalow
	//cv::Mat dummy;
	//cap.read(dummy);
	//int channels = dummy.channels();
	//int depth = dummy.depth();

#if 1

	// Initialize OpenCL
	clw::Context context;
	if(!context.create(getSingleDevice()))
	{
		std::cerr << "Couldn't create context, quitting\n";
		std::exit(-1);
	}

	clw::Device device = context.devices()[0];
	clw::CommandQueue queue = context.createCommandQueue(clw::Property_ProfilingEnabled, device);

	clw::Program prog = context.createProgramFromSourceFile("kernels.cl");
	if(!prog.build())
	{
		std::cout << prog.log();
		std::exit(-1);
	}
	clw::Kernel kernelRgbaToGray = prog.createKernel("rgba2gray");
	clw::Kernel kernelMoG = prog.createKernel("mog");

	// Obraz wejsciowy kolorwy - BGRA
	cl_int error;
	cl_image_format image_format;
	image_format.image_channel_order = CL_BGRA;
	image_format.image_channel_data_type = CL_UNORM_INT8;
	cl_mem cl_inputFrame = clCreateImage2D(context.contextId(), 
		CL_MEM_READ_ONLY, &image_format, cols, rows, 0, nullptr, &error);
	assert(error == CL_SUCCESS);
	clw::Image2D inputFrame(&context, cl_inputFrame);

	// Obraz w skali szarosci
	image_format.image_channel_order = CL_R;
	image_format.image_channel_data_type = CL_UNORM_INT8;
	cl_mem cl_grayFrame = clCreateImage2D(context.contextId(), 
		CL_MEM_READ_WRITE, &image_format, cols, rows, 0, nullptr, &error);
	assert(error == CL_SUCCESS);
	clw::Image2D grayFrame(&context, cl_grayFrame);

	// Obraz (w zasadzie maska) pierwszego planu
	image_format.image_channel_order = CL_R;
	image_format.image_channel_data_type = CL_UNORM_INT8;
	cl_mem cl_dstFrame = clCreateImage2D(context.contextId(), 
		CL_MEM_WRITE_ONLY, &image_format, cols, rows, 0, nullptr, &error);
	assert(error == CL_SUCCESS);
	clw::Image2D dstFrame(&context, cl_dstFrame);

	// Dane mikstur
	static const int nmixtures = 5;
	const int cl_mixturesSize = nmixtures * rows * cols * sizeof(MixtureData);
	cl_mem cl_mixtures = clCreateBuffer(context.contextId(),
		CL_MEM_READ_WRITE, cl_mixturesSize, nullptr, &error);
	assert(error == CL_SUCCESS);
	clw::Buffer mixtureData(&context, cl_mixtures);
	// Wyzerowane
	void* ptr = queue.mapBuffer(mixtureData, clw::Access_WriteOnly);
	memset(ptr, 0, cl_mixturesSize);
	queue.unmap(mixtureData, ptr);

	// Parametry stale dla kernela
	struct MogParams
	{
		int nmixtures;
		float varThreshold;
		float backgroundRatio;
		float w0; // waga dla nowej mikstury
		float var0; // wariancja dla nowej mikstury
		float minVar; // dolny prog mozliwej wariancji
	};
	MogParams mogParams;
	mogParams.nmixtures = nmixtures;
	mogParams.varThreshold = defaultVarianceThreshold;
	mogParams.backgroundRatio = defaultBackgroundRatio;
	mogParams.w0 = defaultInitialWeight;
	mogParams.var0 = (defaultNoiseSigma*defaultNoiseSigma*4);
	mogParams.minVar = (defaultNoiseSigma*defaultNoiseSigma);

	cl_mem cl_params = clCreateBuffer(context.contextId(),
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(MogParams), &mogParams, &error);
	assert(error == CL_SUCCESS);
	clw::Buffer params(&context, cl_params);

	// Ustawienie argumentow i parametrow kerneli
	kernelRgbaToGray.setLocalWorkSize(16, 16);
	kernelRgbaToGray.setRoundedGlobalWorkSize(cols, rows);
	kernelRgbaToGray.setArg(0, inputFrame);
	kernelRgbaToGray.setArg(1, grayFrame);

	kernelMoG.setLocalWorkSize(16 ,16);
	kernelMoG.setRoundedGlobalWorkSize(cols, rows);
	kernelMoG.setArg(0, grayFrame);
	kernelMoG.setArg(1, dstFrame);
	kernelMoG.setArg(2, mixtureData);
	kernelMoG.setArg(3, params);
	kernelMoG.setArg(4, 0.0f);

	int nframe = 0;
	QPCTimer timer;

	for(;;)
	{
		cv::Mat frame, gray(rows, cols, CV_8UC1), dst(rows, cols, CV_8UC1);
		cap >> frame;

		if(frame.rows == 0 || frame.cols == 0)
			break;

		cv::cvtColor(frame, frame, CV_BGR2BGRA);

		size_t origin[3] = {0, 0, 0};
		size_t region[3] = {cols, rows, 1};
		error = clEnqueueWriteImage(queue.commandQueueId(), cl_inputFrame, CL_TRUE,
			origin, region, 0, 0, frame.data, 0, nullptr, nullptr);
		assert(error == CL_SUCCESS);

		double start = timer.currentTime();

		++nframe;
		float learningRate = -1.0f;
		float alpha = learningRate >= 0 && nframe > 1 
			? learningRate
			: 1.0f/std::min(nframe, defaultHistory);
		kernelMoG.setArg(4, alpha);

		queue.runKernel(kernelRgbaToGray);
		queue.runKernel(kernelMoG);

		double stop = timer.currentTime();
		std::cout << 1.0 / (stop - start) << std::endl;		

		error = clEnqueueReadImage(queue.commandQueueId(), cl_dstFrame, CL_TRUE,
			origin, region, 0, 0, dst.data, 0, nullptr, nullptr);
		assert(error == CL_SUCCESS);
		cv::imshow("Result", dst);

		error = clEnqueueReadImage(queue.commandQueueId(), cl_grayFrame, CL_TRUE,
			origin, region, 0, 0, gray.data, 0, nullptr, nullptr);
		assert(error == CL_SUCCESS);
		cv::imshow("Gray", gray);

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
#else
	MixtureOfGaussianCPU MOG(rows, cols);
	//cv::BackgroundSubtractorMOG MOG;
	QPCTimer timer;

	for(;;)
	{

		// Get a new frame from camera
		cv::Mat frame;
		cap >> frame; 

		if(frame.rows == 0 || frame.cols == 0)
			break;

		// Convert it to grayscale
		cvtColor(frame, frame, CV_BGR2GRAY);

		// Estimate the background
		double start = timer.currentTime();

		cv::Mat	frameMask;
		MOG(frame, frameMask, -1);

		double stop = timer.currentTime();

		std::cout << 1.0 / (stop - start) << std::endl;		

		// Show them
		cv::imshow("Mixture of Guassian", frameMask);
		cv::imshow("Original video", frame);

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
#endif
}
