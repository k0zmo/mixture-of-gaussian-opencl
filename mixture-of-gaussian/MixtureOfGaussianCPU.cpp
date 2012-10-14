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
#if 0
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
#else
	float pix = static_cast<float>(*src);
	int pdfMatched = -1;

	for(int mix = 0; mix < nmixtures; ++mix)
	{
		float diff = pix - mptr[mix].mean;
		float d2 = diff*diff;
		float threshold = varThreshold * mptr[mix].var;

		// To samo co:
		//  if (diff > -2.5f * var && 
		//		diff < +2.5f * var)

		// Mahalanobis distance
		if(d2 < threshold)
		{
			pdfMatched = mix;
			break;
		}
	}

	if(pdfMatched < 0)
	{
		// No matching mixture found - replace the weakest one
		//pdfPatched = mix = std::min(mix, nmixtures-1);
		pdfMatched = nmixtures - 1; 

		// First, decrease sum of all mixture weights by old weight 
		// weightSum -= mptr[pdfMatched].weight;
		// Then, increase it by new initial weight
		// weightSum += w0;

		mptr[pdfMatched].weight = w0;
		mptr[pdfMatched].mean = pix;
		mptr[pdfMatched].var = var0;
	}
	else
	{
		for(int mix = 0; mix < nmixtures; ++mix)
		{
			float weight = mptr[mix].weight;

			if(mix == pdfMatched)
			{
				float mu = mptr[mix].mean;
				float diff = pix - mu;
				float var = mptr[mix].var;

				//static const float PI = 3.14159265358979323846f;
				//float ni = 1.0f / sqrtf(2.0f * PI * var) * expf(-0.5f * diff*diff / var);

				mptr[mix].weight = weight + alpha * (1 - weight);
				mptr[mix].mean = mu + alpha * diff;
				mptr[mix].var = std::max(minVar, var + alpha * (diff*diff - var));
			}
			else
			{
				// For the unmatched mixtures, mean and variance
				// are unchanged, only the weight is replaced by:
				// weight = (1 - alpha) * weight;

				mptr[mix].weight = (1 - alpha) * weight;
			}
		}
	}

	// Normalize weight and calculate sortKey
	float weightSum = 0.0f;
	for(int mix = 0; mix < nmixtures; ++mix)
		weightSum += mptr[mix].weight;

	float invSum = 1.0f / weightSum;
	float sortKey[5];
	for(int mix = 0; mix < nmixtures; ++mix)
	{
		mptr[mix].weight *= invSum;
		sortKey[mix] = mptr[mix].var > DBL_MIN
			? mptr[mix].weight / sqrtf(mptr[mix].var)
			: 0;
	}

	// Sort mixtures (buble sort).
	// Every mixtures but the one with "completely new" weight and variance
	// are already sorted thus we need to reorder only that single mixture.

	for(int mix = 0; mix < pdfMatched; ++mix)
	{
		if(sortKey[pdfMatched] > sortKey[mix])
		{
			swap(mptr[pdfMatched], mptr[mix]);
			break;
		}
	}

	// Determine background distribution
	weightSum = 0.0f;
	for(int mix = 0; mix < nmixtures; ++mix)
	{
		weightSum += mptr[mix].weight;

		if(weightSum > backgroundRatio)
		{
			if(pdfMatched > mix)
			{
				*dst = 255;
			}
			else
			{
				*dst = 0;
			}

			return;
		}
	}
#endif
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