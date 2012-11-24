#include "GrayscaleGPU.h"

GrayscaleGPU::GrayscaleGPU(const clw::Context& context,
						   const clw::Device& device,
						   const clw::CommandQueue& queue)
	: context(context)
	, device(device)
	, queue(queue)
{
}

void GrayscaleGPU::init(int imageWidth,
						int imageHeight,
						int workGroupSizeX,
						int workGroupSizeY)
{
	createRgb2GrayKernel();
	createOutputImage(imageWidth, imageHeight);

	width = imageWidth;
	height = imageHeight;
	cl_int2 frameSize = { width, height };

	// Ustawienie argumentow i parametrow kerneli
	kernel.setLocalWorkSize(workGroupSizeX, workGroupSizeY);
	kernel.setRoundedGlobalWorkSize(width, height);
	kernel.setArg(1, outputImage);
	kernel.setArg(2, frameSize);
}

void GrayscaleGPU::setKernelWorkGroupSize(int workGroupSizeX,
										  int workGroupSizeY)
{
	if(!kernel.isNull())
	{
		kernel.setLocalWorkSize(workGroupSizeX, workGroupSizeY);
		kernel.setRoundedGlobalWorkSize(width, height);
	}
}

clw::Event GrayscaleGPU::process(clw::Buffer& inputImage)
{
	if(kernel.isNull())
		return clw::Event();
	kernel.setArg(0, inputImage);
	return queue.asyncRunKernel(kernel);
}


void GrayscaleGPU::createRgb2GrayKernel()
{
	clw::Program progCvt = context.createProgramFromSourceFile("color-conversion.cl");
	if(!progCvt.build())
	{
		std::cout << progCvt.log();
		std::exit(-1);
	}
	std::cout << progCvt.log();
	kernel = progCvt.createKernel("rgb2gray_image");
}

void GrayscaleGPU::createOutputImage(int width,
									 int height)
{
	// Obraz w skali szarosci
	outputImage = context.createImage2D
		(clw::Access_ReadWrite, clw::Location_Device,
			clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8),
			width, height);
}