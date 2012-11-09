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
	const int channels = 3;
	cl_int2 frameSize = { cols, rows };

	static const int nmixtures = 5;
	std::string buildOptions = "-Dmixtures=";
	buildOptions += nmixtures + '0';

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
	//if(!context.create(clw::Gpu))
	{
		std::cerr << "Couldn't create context, quitting\n";
		std::exit(-1);
	}

	clw::Device device = context.devices()[0];
	clw::CommandQueue queue = context.createCommandQueue
		(clw::Property_ProfilingEnabled, device);

	clw::Program prog = context.createProgramFromSourceFile("kernels.cl");
	if(!prog.build(buildOptions))
	{
		std::cout << prog.log();
		std::exit(-1);
	}
	std::cout << prog.log();
	clw::Kernel kernelRgbaToGray = prog.createKernel("rgb2gray_image");
	clw::Kernel kernelMoG = prog.createKernel("mog");

	// Obraz wejsciowy kolorowy - BGR (jako bufor)
	int inputFrameSize = rows * cols * channels * sizeof(cl_uchar);
	clw::Buffer inputFrame = context.createBuffer
		(clw::Access_ReadOnly, clw::Location_Device, inputFrameSize);

	// Obraz w skali szarosci
	clw::Image2D grayFrame = context.createImage2D
		(clw::Access_ReadWrite, clw::Location_Device,
		 clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8),
		 cols, rows);

	// Obraz (w zasadzie maska) pierwszego planu
	clw::Image2D dstFrame = context.createImage2D
		(clw::Access_WriteOnly, clw::Location_Device,
		 clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8),
		 cols, rows);

	// Dane mikstur (stan wewnetrzny estymatora tla)
	const int mixtureDataSize = nmixtures * rows * cols * sizeof(MixtureData);
	clw::Buffer mixtureData = context.createBuffer
		(clw::Access_ReadWrite, clw::Location_Device, mixtureDataSize);

	// Wyzerowane
	void* ptr = queue.mapBuffer(mixtureData, clw::Access_WriteOnly);
	memset(ptr, 0, mixtureDataSize);
	queue.unmap(mixtureData, ptr);

	// Parametry stale dla kernela
	struct MogParams
	{
		float varThreshold;
		float backgroundRatio;
		float w0; // waga dla nowej mikstury
		float var0; // wariancja dla nowej mikstury
		float minVar; // dolny prog mozliwej wariancji
	};
	MogParams mogParams = {
		/*.varThreshold    = */ defaultVarianceThreshold,
		/*.backgroundRatio = */ defaultBackgroundRatio,
		/*.w0              = */ defaultInitialWeight,
		/*.var0            = */ (defaultNoiseSigma*defaultNoiseSigma*4),
		/*.minVar          = */ (defaultNoiseSigma*defaultNoiseSigma),
	};

	clw::Buffer params = context.createBuffer(
		clw::Access_ReadOnly, clw::Location_Device, 
		sizeof(MogParams), &mogParams);

	// Ustawienie argumentow i parametrow kerneli
	kernelRgbaToGray.setLocalWorkSize(8, 8);
	kernelRgbaToGray.setRoundedGlobalWorkSize(cols, rows);
	kernelRgbaToGray.setArg(0, inputFrame);
	kernelRgbaToGray.setArg(1, grayFrame);
	kernelRgbaToGray.setArg(2, frameSize);

	kernelMoG.setLocalWorkSize(8 ,8);
	kernelMoG.setRoundedGlobalWorkSize(cols, rows);
	kernelMoG.setArg(0, grayFrame);
	kernelMoG.setArg(1, dstFrame);
	kernelMoG.setArg(2, mixtureData);
	kernelMoG.setArg(3, params);
	kernelMoG.setArg(4, 0.0f);

	int nframe = 0;

	for(;;)
	{
		cv::Mat frame, gray(rows, cols, CV_8UC1), dst(rows, cols, CV_8UC1);
		cap >> frame;

		if(frame.rows == 0 || frame.cols == 0)
			break;

		void* ptr = queue.mapBuffer
			(inputFrame, 0, inputFrameSize, clw::Access_WriteOnly);
		//void* ptr;
		//clw::Event e00 = queue.asyncMapBuffer
		//	(inputFrame, &ptr, 0, inputFrameSize, clw::Access_WriteOnly);
		memcpy(ptr, frame.data, inputFrameSize);
		clw::Event e0 = queue.asyncUnmap(inputFrame, ptr);

		++nframe;
		float learningRate = -1.0f;
		float alpha = learningRate >= 0 && nframe > 1 
			? learningRate
			: 1.0f/std::min(nframe, defaultHistory);
		kernelMoG.setArg(4, alpha);

		clw::Event e1 = queue.asyncRunKernel(kernelRgbaToGray);
		clw::Event e2 = queue.asyncRunKernel(kernelMoG);
		clw::Event e3 = queue.asyncReadImage2D(dstFrame, dst.data, 0, 0, cols, rows);
		clw::Event e4 = queue.asyncReadImage2D(grayFrame, gray.data, 0, 0, cols, rows);

		e4.waitForFinished();

		//double writeDuration = (e0.finishTime() - e0.startTime()) * 0.000001;
		//double colorDuration = (e1.finishTime() - e1.startTime()) * 0.000001;
		//double mogDuration = (e2.finishTime() - e2.startTime()) * 0.000001;		
		//double readDstDuration = (e3.finishTime() - e3.startTime()) * 0.000001;
		//double readGrayDuration = (e4.finishTime() - e4.startTime()) * 0.000001;
		//std::cout 
		//	<< "MoG: " << mogDuration << " [ms]" <<
		//	", color: " << colorDuration << " [ms]" <<
		//	", writeSrc: " << writeDuration << " [ms]" <<
		//	", readDst: " << readDstDuration << " [ms]" <<
		//	", readGray: " << readGrayDuration << " [ms]\n";

		cv::imshow("Result", dst);
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

		std::cout << (stop - start) * 1000.0 << std::endl;

		//cv::Canny(frameMask, frameMask, 3, 9);

		//for(int y = 0; y < frameMask.rows; ++y)
		//{
		//	uchar* ptr = frameMask.ptr<uchar>(y);
		//	uchar* src = frame.ptr<uchar>(y);
		//	for(int x = 0; x < frameMask.cols; ++x, ++ptr, ++src)
		//	{
		//		if(ptr[0] > 0)
		//			ptr[0] = src[0];
		//	}
		//}

		// Show them
		cv::imshow("Mixture of Guassian", frameMask);
		cv::imshow("Original video", frame);

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
#endif
}
