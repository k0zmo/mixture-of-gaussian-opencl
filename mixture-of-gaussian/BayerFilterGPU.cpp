#include "BayerFilterGPU.h"



BayerFilterGPU::BayerFilterGPU(const clw::Context& context,
                               const clw::Device& device, 
                               const clw::CommandQueue& queue)
	: context(context)
	, device(device)
	, queue(queue)
{
}

void BayerFilterGPU::init(int imageWidth,
                          int imageHeight,
                          int workGroupSizeX,
                          int workGroupSizeY,
						  EBayerFilter filter)
{
	createBayer2GrayKernel(filter);
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

void BayerFilterGPU::setKernelWorkGroupSize(int workGroupSizeX,
                                            int workGroupSizeY)
{
	if(!kernel.isNull())
	{
		kernel.setLocalWorkSize(workGroupSizeX, workGroupSizeY);
		kernel.setRoundedGlobalWorkSize(width, height);
	}
}

clw::Event BayerFilterGPU::process(clw::Buffer& inputImage)
{
	if(kernel.isNull())
		return clw::Event();
	kernel.setArg(0, inputImage);
	return queue.asyncRunKernel(kernel);
}

void BayerFilterGPU::createBayer2GrayKernel(EBayerFilter bayerFilter)
{
	clw::Program progCvt = context.createProgramFromSourceFile("bayer.cl");
	if(!progCvt.build())
	{
		std::cout << progCvt.log();
		std::exit(-1);
	}
	std::cout << progCvt.log();

	switch(bayerFilter)
	{
	case Bayer_RG:
		kernel = progCvt.createKernel("convert_rg2gray");
		break;
	case Bayer_BG:
		kernel = progCvt.createKernel("convert_bg2gray");
		break;
	case Bayer_GR:
		kernel = progCvt.createKernel("convert_gr2gray");
		break;
	case Bayer_GB:
		kernel = progCvt.createKernel("convert_gb2gray");
		break;
	}
}

void BayerFilterGPU::createOutputImage(int width,
                                       int height)
{
	// Obraz w skali szarosci
	outputImage = context.createImage2D
		(clw::Access_ReadWrite, clw::Location_Device,
		 clw::ImageFormat(clw::Order_R, clw::Type_Normalized_UInt8),
		 width, height);
}