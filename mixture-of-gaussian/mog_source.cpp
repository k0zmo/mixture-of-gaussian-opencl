class MixtureOfGaussian
{
public:
	//! the default constructor
	MixtureOfGaussian();
	//! the full constructor that takes the length of the history, the number of gaussian mixtures, the background ratio parameter and the noise strength
	MixtureOfGaussian(int history, int nmixtures, double backgroundRatio, double noiseSigma=0);
	//! the destructor
	virtual ~MixtureOfGaussian();
	//! the update operator
	virtual void operator()(cv::InputArray image, cv::OutputArray fgmask, double learningRate=0);

	//! re-initiaization method
	virtual void initialize(cv::Size frameSize, int frameType);

protected:    
	cv::Size frameSize;
	int frameType;
	cv::Mat bgmodel;
	int nframes;
	int history;
	int nmixtures;
	double varThreshold;
	double backgroundRatio;
	double noiseSigma;
};	

static const int defaultNMixtures = 5;
static const int defaultHistory = 200;
static const double defaultBackgroundRatio = 0.7;
static const double defaultVarThreshold = 2.5*2.5;
static const double defaultNoiseSigma = 30*0.5;
static const double defaultInitialWeight = 0.05;

MixtureOfGaussian::MixtureOfGaussian()
{
	frameSize = cv::Size(0,0);
	frameType = 0;

	nframes = 0;
	nmixtures = defaultNMixtures;
	history = defaultHistory;
	varThreshold = defaultVarThreshold;
	backgroundRatio = defaultBackgroundRatio;
	noiseSigma = defaultNoiseSigma;
}

MixtureOfGaussian::MixtureOfGaussian(int _history, int _nmixtures,
	double _backgroundRatio,
	double _noiseSigma)
{
	frameSize = cv::Size(0,0);
	frameType = 0;

	nframes = 0;
	nmixtures = cv::min(_nmixtures > 0 ? _nmixtures : defaultNMixtures, 8);
	history = _history > 0 ? _history : defaultHistory;
	varThreshold = defaultVarThreshold;
	backgroundRatio = cv::min(_backgroundRatio > 0 ? _backgroundRatio : 0.95, 1.);
	noiseSigma = _noiseSigma <= 0 ? defaultNoiseSigma : _noiseSigma;
}

MixtureOfGaussian::~MixtureOfGaussian()
{
}

void MixtureOfGaussian::initialize(cv::Size _frameSize, int _frameType)
{
	frameSize = _frameSize;
	frameType = _frameType;
	nframes = 0;

	int nchannels = CV_MAT_CN(frameType);
	CV_Assert( CV_MAT_DEPTH(frameType) == CV_8U );

	// for each gaussian mixture of each pixel bg model we store ...
	// the mixture sort key (w/sum_of_variances), the mixture weight (w),
	// the mean (nchannels values) and
	// the diagonal covariance matrix (another nchannels values)
	bgmodel.create( 1, frameSize.height*frameSize.width*nmixtures*(2 + 2*nchannels), CV_32F );
	bgmodel = cv::Scalar::all(0);
}

template<typename VT> struct MixData
{
	float sortKey;
	float weight;
	VT mean;
	VT var;
};

static void process8uC1( const cv::Mat& image, cv::Mat& fgmask, double learningRate,
	cv::Mat& bgmodel, int nmixtures, double backgroundRatio,
	double varThreshold, double noiseSigma )
{
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = (float)learningRate, T = (float)backgroundRatio, vT = (float)varThreshold;
	int K = nmixtures;
	MixData<float>* mptr = (MixData<float>*)bgmodel.data;

	const float w0 = (float)defaultInitialWeight;
	const float sk0 = (float)(w0/(defaultNoiseSigma*2));
	const float var0 = (float)(defaultNoiseSigma*defaultNoiseSigma*4);
	const float minVar = (float)(noiseSigma*noiseSigma);

	for( y = 0; y < rows; y++ )
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);

		for( x = 0; x < cols; x++, mptr += K )
		{
			float wsum = 0;
			float pix = src[x];
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
					float dw = alpha*(1.f - w);
					mptr[k].weight = w + dw;
					mptr[k].mean = mu + alpha*diff;
					var = cv::max(var + alpha*(d2 - var), minVar);
					mptr[k].var = var;
					mptr[k].sortKey = w/sqrt(var);

					for( k1 = k-1; k1 >= 0; k1-- )
					{
						if( mptr[k1].sortKey >= mptr[k1+1].sortKey )
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
				mptr[k].sortKey = sk0;
			}
			else
				for( ; k < K; k++ )
					wsum += mptr[k].weight;

			float wscale = 1.f/wsum;
			wsum = 0;
			for( k = 0; k < K; k++ )
			{
				wsum += mptr[k].weight *= wscale;
				mptr[k].sortKey *= wscale;
				if( wsum > T && kForeground < 0 )
					kForeground = k+1;
			}

			dst[x] = (uchar)(-(kHit >= kForeground));
		}
	}
}

void MixtureOfGaussian::operator()(cv::InputArray _image, cv::OutputArray _fgmask, double learningRate)
{
	cv::Mat image = _image.getMat();
	bool needToInitialize = nframes == 0 || learningRate >= 1 || image.size() != frameSize || image.type() != frameType;

	if( needToInitialize )
		initialize(image.size(), image.type());

	CV_Assert( image.depth() == CV_8U );
	_fgmask.create( image.size(), CV_8U );
	cv::Mat fgmask = _fgmask.getMat();

	++nframes;
	learningRate = learningRate >= 0 && nframes > 1
		? learningRate
		: 1./cv::min( nframes, history );
	CV_Assert(learningRate >= 0);

	if( image.type() == CV_8UC1 )
		process8uC1( image, fgmask, learningRate, bgmodel, nmixtures, backgroundRatio, varThreshold, noiseSigma );
	else
		CV_Error( CV_StsUnsupportedFormat, "Only 1- and 3-channel 8-bit images are supported in BackgroundSubtractorMOG" );
}