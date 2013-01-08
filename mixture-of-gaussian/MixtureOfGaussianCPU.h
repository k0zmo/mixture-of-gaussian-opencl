#pragma once

#include <opencv2/core/core.hpp>

//
// Currenty unused, was used only during tests
//

static const int defaultNumMixtures = 5;
static const int defaultHistory = 200;
static const float defaultBackgroundRatio = 0.7f;
static const float defaultVarianceThreshold = 2.5f * 2.5f;
static const float defaultNoiseSigma = 30.0f * 0.5f;
static const float defaultInitialWeight = 0.05f;

struct MixtureData
{
	float weight;
	float mean;
	float var;

	bool operator>=(const MixtureData& rhs)
	{
		return weight/sqrtf(var) >= rhs.weight/sqrtf(rhs.var);
	}

	friend void swap(MixtureData& lhs, MixtureData& rhs)
	{
		MixtureData temp = lhs;
		lhs = rhs;
		rhs = temp;
	}
};

class MixtureOfGaussianCPU
{
public:
	MixtureOfGaussianCPU(int rows, int cols,
		int history = defaultHistory);

	void operator() (cv::InputArray in, cv::OutputArray out,
		float learningRate = 0.0f);
	void reinitialize(float backgroundRatio);

private:
	void calc_pix_impl(uchar src, uchar* dst,
		MixtureData mptr[], float alpha);
	void calc_impl(uchar* frame, uchar* mask,
		MixtureData* mptr, float alpha);

private:
	const int rows;
	const int cols;
	const int nmixtures;
	const int history;

	int nframe;

	float backgroundRatio;
	float varThreshold;
	float noiseSigma;
	float initialWeight;

	cv::Mat bgmodel;
};