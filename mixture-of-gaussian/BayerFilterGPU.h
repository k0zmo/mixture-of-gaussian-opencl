#pragma once

#include <clw/clw.h>

enum EBayerFilter
{
	Bayer_RG,
	Bayer_BG,
	Bayer_GR,
	Bayer_GB
};

class BayerFilterGPU
{
public:
	BayerFilterGPU(const clw::Context& context,
		const clw::Device& device,
		const clw::CommandQueue& queue);

	void init(int imageWidth, int imageHeight,
		int workGroupSizeX, int workGroupSizeY,
		EBayerFilter bayerFilter);

	void setKernelWorkGroupSize(int workGroupSizeX, int workGroupSizeY);

	clw::Event process(clw::Buffer& inputImage);
	clw::Image2D output() const { return outputImage; }

private:
	void createBayer2GrayKernel(EBayerFilter bayerFilter);
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
	BayerFilterGPU(const BayerFilterGPU&);
	BayerFilterGPU& operator=(const BayerFilterGPU&);
};

