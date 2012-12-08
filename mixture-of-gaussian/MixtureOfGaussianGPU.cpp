#include "MixtureOfGaussianGPU.h"

#include <opencv2/core/core.hpp>
#include <iostream>

MixtureOfGaussianGPU::MixtureOfGaussianGPU(const clw::Context& context,
										   const clw::Device& device, 
										   const clw::CommandQueue& queue)
	: context(context)
	, device(device)
	, queue(queue)
	, nframe(0)
	, history(200)
	, varianceThreshold(6.25f)
	, backgroundRatio(0.7f)
	, initialWeight(0.05f)
	, initialVariance(500)
	, minVariance(0.4f)
{
}

void MixtureOfGaussianGPU::setMixtureParameters(int history,
												float varianceThreshold,
												float backgroundRatio,
												float initialWeight,
												float initialVariance,
												float minVariance)
{
	this->history = history;
	this->varianceThreshold = varianceThreshold;
	this->backgroundRatio = backgroundRatio;
	this->initialWeight = initialWeight;
	this->initialVariance = initialVariance;
	this->minVariance = minVariance;

	createMixtureParamsBuffer();
}
	
void MixtureOfGaussianGPU::init(int imageWidth,
								int imageHeight, 
								int workGroupSizeX,
								int workGroupSizeY,
								int nmixtures)
{
	nframe = 0;
	createMoGKernel(nmixtures);
	createMixtureDataBuffer(imageWidth * imageHeight, nmixtures);
	createMixtureParamsBuffer();
	createOutputImage(imageWidth, imageHeight);

	width = imageWidth;
	height = imageHeight;

	kernel.setLocalWorkSize(workGroupSizeX, workGroupSizeY);
	kernel.setRoundedGlobalWorkSize(imageWidth, imageHeight);
	kernel.setArg(1, outputImage);
	kernel.setArg(2, mixtureDataBuffer);
	kernel.setArg(3, mixtureParamsBuffer);
	kernel.setArg(4, 0.0f);
}

void MixtureOfGaussianGPU::setKernelWorkGroupSize(int workGroupSizeX,
												  int workGroupSizeY)
{
	if(!kernel.isNull())
	{
		kernel.setLocalWorkSize(workGroupSizeX, workGroupSizeY);
		kernel.setRoundedGlobalWorkSize(width, height);
	}
}

clw::Event MixtureOfGaussianGPU::process(clw::Image2D& inputGrayFrame,
										 float learningRate)
{
	if(kernel.isNull())
		return clw::Event();
		
	// Calculate dynamic learning rate (if necessary)
	++nframe;
	float alpha = learningRate >= 0 && nframe > 1 
		? learningRate
		: 1.0f/std::min(nframe, history);

	kernel.setArg(0, inputGrayFrame);
	kernel.setArg(4, alpha);

	cv::Mat dst(height, width, CV_8UC1);

	return queue.asyncRunKernel(kernel);
}

void MixtureOfGaussianGPU::createMoGKernel(int nmixtures)
{
	std::string buildOptions = "-Dmixtures=";
	buildOptions += nmixtures + '0';

	clw::Program progMog = context.createProgramFromSourceFile("mixture-of-gaussian.cl");
	if(!progMog.build(buildOptions))
	{
		std::cout << progMog.log();
		std::exit(-1);
	}
	std::cout << progMog.log();
	kernel = progMog.createKernel("mog_image");
}

void MixtureOfGaussianGPU::createMixtureDataBuffer(int npixels, 
												   int nmixtures)
{
	// Dane mikstur (stan wewnetrzny estymatora tla)
	const int mixtureDataSize = nmixtures * npixels * 3 * sizeof(float);

	mixtureDataBuffer = context.createBuffer
		(clw::Access_ReadWrite, clw::Location_Device, mixtureDataSize);

	// Wyzerowane
	void* ptr = queue.mapBuffer(mixtureDataBuffer, clw::MapAccess_Write);
	memset(ptr, 0, mixtureDataSize);
	queue.unmap(mixtureDataBuffer, ptr);
}

void MixtureOfGaussianGPU::createMixtureParamsBuffer()
{
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
		varianceThreshold,
		backgroundRatio,
		initialWeight,
		initialVariance,
		minVariance
	};

	mixtureParamsBuffer = context.createBuffer(
		clw::Access_ReadOnly, clw::Location_Device, 
		sizeof(MogParams), &mogParams);
}

void MixtureOfGaussianGPU::createOutputImage(int width,
											 int height)
{
	// Obraz (w zasadzie maska) pierwszego planu
	outputImage = context.createImage2D
		(clw::Access_WriteOnly, clw::Location_Device,
		 clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8),
		 width, height);
}