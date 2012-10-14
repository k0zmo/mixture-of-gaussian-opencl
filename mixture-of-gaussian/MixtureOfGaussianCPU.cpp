#include "MixtureOfGaussianCPU.h"

MixtureOfGaussianCPU::MixtureOfGaussianCPU(int rows, int cols, int history)
	: rows(rows)
	, cols(cols)
	, nmixtures(defaultNumMixtures)
	, history(history)
	, nframe(0)
	, backgroundRatio(defaultBackgroundRatio)
	, varThreshold(defaultVarianceThreshold)
	, noiseSigma(defaultNoiseSigma)
	, initialWeight(defaultInitialWeight)
{
	// Gaussian mixtures data
	int mix_data_size = cols * rows * nmixtures;
	bgmodel.create(1, mix_data_size * sizeof(MixtureData) / sizeof(float), CV_32F);
	bgmodel = cv::Scalar::all(0);
}

void MixtureOfGaussianCPU::operator() (cv::InputArray in, cv::OutputArray out,
	float learningRate)
{
	cv::Mat frame = in.getMat();

	++nframe;
	float alpha = learningRate >= 0 && nframe > 1 
		? learningRate
		: 1.0f/std::min(nframe, history);

	out.create(frame.size(), CV_8U);
	cv::Mat mask = out.getMat();

	calc_impl(frame.data, mask.data, bgmodel.ptr<MixtureData>(), alpha);
}

void MixtureOfGaussianCPU::reinitialize(float backgroundRatio)
{
	this->backgroundRatio = backgroundRatio;
	bgmodel = cv::Scalar::all(0);
}

void MixtureOfGaussianCPU::calc_pix_impl(const uchar* src, uchar* dst, 
	MixtureData mptr[], float alpha)
{
	const float w0 = initialWeight; // 0.05 lub 0.001
	const float var0 = (noiseSigma*noiseSigma*4); // 900.0 lub 50.0f
	const float sk0 = w0/(noiseSigma*2); // omega/sqrt(variance)
	const float minVar = (noiseSigma*noiseSigma); // 225.0

	int k, k1, K = nmixtures;
	float vT = varThreshold, T = backgroundRatio;
	float wsum = 0;
	float pix = *src;
	int kHit = -1, kForeground = -1;

	for( k = 0; k < K; k++ )
	{
		float w = mptr[k].weight;
		wsum += w;
		if( w < FLT_EPSILON )
			break;
		float mu = mptr[k].mean;
		float var = mptr[k].var;
		float diff = pix - mu;
		float d2 = diff*diff;
		if( d2 < vT*var )
		{
			wsum -= w;
			mptr[k].weight = w + alpha*(1.f - w);
			mptr[k].mean = mu + alpha*diff;
			var = cv::max(var + alpha*(d2 - var), minVar);
			mptr[k].var = var;

			for( k1 = k-1; k1 >= 0; k1-- )
			{
				if(mptr[k1] >= mptr[k1+1])
					break;

				std::swap( mptr[k1], mptr[k1+1] );
			}

			kHit = k1+1;
			break;
		}
	}

	// no appropriate gaussian mixture found at all, 
	// remove the weakest mixture and create a new one
	if( kHit < 0 ) 
	{
		kHit = k = cv::min(k, K-1);
		wsum += w0 - mptr[k].weight;
		mptr[k].weight = w0;
		mptr[k].mean = pix;
		mptr[k].var = var0;
	}
	else
		for( ; k < K; k++ )
			wsum += mptr[k].weight;

	float wscale = 1.f/wsum;
	wsum = 0;
	for( k = 0; k < K; k++ )
	{
		wsum += mptr[k].weight *= wscale;
		if( wsum > T && kForeground < 0 )
			kForeground = k+1;
	}

	*dst = (uchar)(-(kHit >= kForeground));
}

void MixtureOfGaussianCPU::calc_impl(uchar* frame, uchar* mask,
	MixtureData* mptr, float alpha)
{
	for(int y = 0; y < rows; ++y)
	{
		const uchar* src = &frame[y * cols];
		uchar* dst = &mask[y * cols];

		for(int x = 0; x < cols; ++x, mptr += nmixtures)
		{
			calc_pix_impl(&src[x], &dst[x], mptr, alpha);
		}
	}
}