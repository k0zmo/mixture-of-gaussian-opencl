#pragma once

#include <clw/clw.h>

class GrayscaleGPU
{
public:
	GrayscaleGPU(const clw::Context& context,
		const clw::Device& device,
		const clw::CommandQueue& queue);

	void init(int imageWidth, int imageHeight,
		int workGroupSizeX, int workGroupSizeY);

	void setKernelWorkGroupSize(int workGroupSizeX, int workGroupSizeY);

	clw::Event process(clw::Buffer& inputImage);
	clw::Image2D output() const { return outputImage; }

private:

	void createRgb2GrayKernel();
	void createOutputImage(int width, int height);

private:
	clw::Context context;
	clw::Device device;
	clw::CommandQueue queue;
	clw::Kernel kernel;
	clw::Image2D outputImage;
	int width;
	int height;

private:
	GrayscaleGPU(const GrayscaleGPU&);
	GrayscaleGPU& operator=(const GrayscaleGPU&);
};