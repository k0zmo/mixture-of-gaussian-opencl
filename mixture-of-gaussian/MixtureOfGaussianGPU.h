#pragma once

#include <clw/clw.h>

class MixtureOfGaussianGPU
{
public:
	MixtureOfGaussianGPU(const clw::Context& context,
		const clw::Device& device, 
		const clw::CommandQueue& queue);

	void setMixtureParameters(int history,
		float varianceThreshold,
		float backgroundRatio,
		float initialWeight,
		float initialVariance,
		float minVariance);

	void init(int imageWidth, int imageHeight, 
		int workGroupSizeX, int workGroupSizeY, int nmixtures = 5);

	void setKernelWorkGroupSize(int workGroupSizeX, int workGroupSizeY);

	clw::Event process(clw::Image2D& inputGrayFrame, float learningRate = -1);
	clw::Image2D output() const { return outputImage; }

private:
	void createMoGKernel(int nmixtures);
	void createMixtureDataBuffer(int npixels, int nmixtures);
	void createMixtureParamsBuffer();
	void createOutputImage(int width, int height);

private:
	clw::Context context;
	clw::Device device;
	clw::CommandQueue queue;
	clw::Kernel kernel;
	clw::Buffer mixtureDataBuffer;
	clw::Buffer mixtureParamsBuffer;
	clw::Image2D outputImage;

	int width, height;
	int nframe;
	int history;
	float varianceThreshold;
	float backgroundRatio;
	float initialWeight;
	float initialVariance;
	float minVariance;

private:
	MixtureOfGaussianGPU(const MixtureOfGaussianGPU&);
	MixtureOfGaussianGPU& operator=(const MixtureOfGaussianGPU&);
};