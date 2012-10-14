#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>

struct MixtureData
{
	float weight;
	float mean;
	float var;

	bool operator>=(const MixtureData& rhs)
	{
		return weight/sqrtf(var) >= rhs.weight/sqrtf(rhs.var);
	}
};

static const int defaultNumMixtures = 5;
static const float defaultHistory = 200;
static const float defaultBackgroundRatio = 0.7f;
static const float defaultVarianceThreshold = 2.5f * 2.5f;
static const float defaultNoiseSigma = 30.0f * 0.5f;
static const float defaultInitialWeight = 0.05f;

class MixtureOfGaussianCPU
{
public:
	MixtureOfGaussianCPU(int rows, int cols, int history = defaultHistory)
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

	void operator() (cv::InputArray in, cv::OutputArray out, float learningRate = 0.0f)
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

	void reinitialize(float backgroundRatio)
	{
		this->backgroundRatio = backgroundRatio;
		bgmodel = cv::Scalar::all(0);
	}

private:
	void swap(MixtureData& lhs, MixtureData& rhs)
	{
		MixtureData temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	inline void calc_pix_impl(const uchar* src, uchar* dst, 
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

		if( kHit < 0 ) // no appropriate gaussian mixture found at all, remove the weakest mixture and create a new one
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

	void calc_impl(uchar* frame, uchar* mask, MixtureData* mptr, float alpha)
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

int main(int, char**)
{
	// Open sample video
	cv::VideoCapture cap("surveillance.webm");
	if(!cap.isOpened())
	{
		exit(-1);
	}

	// Retrieve frame size
	const int cols = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	const int rows = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	MixtureOfGaussianCPU MOG(rows, cols);

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
		cv::Mat	frameMask;
		MOG(frame, frameMask, -1);

		// Show them
		cv::imshow("Mixture of Guassian", frameMask);
		cv::imshow("Original video", frame);

		int key = cv::waitKey(30);
		if(key >= 0)
			break;
	}
}