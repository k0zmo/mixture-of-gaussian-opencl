__constant sampler_t smp = 
		CLK_NORMALIZED_COORDS_FALSE | 
		CLK_FILTER_NEAREST | 
		CLK_ADDRESS_CLAMP_TO_EDGE;
		
__constant float4 grayscale = { 0.2989f, 0.5870f, 0.1140f, 0 };

__kernel void rgba2gray(
	__read_only image2d_t src,
	__write_only image2d_t dst)
{
	const int2 gid = { get_global_id(0), get_global_id(1) };
	const int2 size = { get_image_width(src), get_image_height(src) };
	
	if (all(gid < size))
	{
		float4 rgba = read_imagef(src, smp, gid);
		float gray = dot(grayscale, rgba);
		write_imagef(dst, gid, (float4) gray);
	}
}

typedef struct MixtureData
{
	float weight;
	float mean;
	float var;
} MixtureData;

typedef struct MogParams
{
	int nmixtures;
	float varThreshold;
	float backgroundRatio;
	float w0; // waga dla nowej mikstury
	float var0; // wariancja dla nowej mikstury
	float minVar; // dolny prog mozliwej wariancji
} MogParams;

__kernel void mog(
	__read_only image2d_t frame,
	__write_only image2d_t dst,
	__global MixtureData* mptr,
	__constant MogParams* params,
	const float alpha) // krzywa uczenia
{
	const int2 gid = { get_global_id(0), get_global_id(1) };
	const int2 size = { get_image_width(frame), get_image_height(frame) };
	const int gid1 = gid.x + gid.y * size.x;
	
	if (all(gid < size))
	{
		float pix = read_imagef(frame, smp, gid).x * 255.0f;
		
		bool pdfMatched = -1;	
		const int nmixtures = params->nmixtures;
		#define MIXTURE(mx) mptr[gid1 * nmixtures + mx]
		
		for(int mx = 0; mx < nmixtures; ++mx)
		{
			float diff = pix - MIXTURE(mx).mean;
			float d2 = diff*diff;
			float threshold = params->varThreshold * MIXTURE(mx).var;
			
			// To samo co:
			 // if (diff > -2.5f * var && 
					// diff < +2.5f * var)

			// Mahalanobis distance
			if(d2 < threshold)
			{
				pdfMatched = mx;
				break;
			}
		}
		
		if(pdfMatched < 0)
		{
			// No matching mixture found - replace the weakest one
			//pdfPatched = mx = std::min(mx, nmixtures-1);
			pdfMatched = nmixtures - 1; 

			// First, decrease sum of all mixture weights by old weight 
			// weightSum -= mptr[pdfMatched].weight;
			// Then, increase it by new initial weight
			// weightSum += w0;

			//MIXTURE(pdfMatched).weight = params->w0;
			//MIXTURE(pdfMatched).mean = pix;
			//MIXTURE(pdfMatched).var = params->var0;
		}
		else
		{
			for(int mx = 0; mx < nmixtures; ++mx)
			{
				float weight = MIXTURE(mx).weight;

				if(mx == pdfMatched)
				{
					float mu = MIXTURE(mx).mean;
					float diff = pix - mu;
					float var = MIXTURE(mx).var;

					//static const float PI = 3.14159265358979323846f;
					//float ni = 1.0f / sqrtf(2.0f * PI * var) * expf(-0.5f * diff*diff / var);

					MIXTURE(mx).weight = weight + alpha * (1 - weight);
					MIXTURE(mx).mean = mu + alpha * diff;
					MIXTURE(mx).var = max(params->minVar, var + alpha * (diff*diff - var));
				}
				else
				{
					// For the unmatched mixtures, mean and variance
					// are unchanged, only the weight is replaced by:
					// weight = (1 - alpha) * weight;

					MIXTURE(mx).weight = (1 - alpha) * weight;
				}
			}
		}
	
		// Normalize weight and calculate sortKey
		float weightSum = 0.0f;
		for(int mx = 0; mx < nmixtures; ++mx)
			weightSum += MIXTURE(mx).weight;

		float invSum = 1.0f / weightSum;
		float sortKey[5];
		for(int mx = 0; mx < nmixtures; ++mx)
		{
			MIXTURE(mx).weight *= invSum;
			sortKey[mx] = MIXTURE(mx).var > DBL_MIN
				? MIXTURE(mx).weight / sqrt(MIXTURE(mx).var)
				: 0;
		}

		// Sort mixtures (buble sort).
		// Every mixtures but the one with "completely new" weight and variance
		// are already sorted thus we need to reorder only that single mixture.

		for(int mx = 0; mx < pdfMatched; ++mx)
		{
			if(sortKey[pdfMatched] > sortKey[mx])
			{
				MixtureData tmp = MIXTURE(pdfMatched);
				MIXTURE(pdfMatched) = MIXTURE(mx);
				MIXTURE(mx) = tmp;
				break;
			}
		}

		// No match is found with any of the K Gaussians.
		// In this case, the pixel is classified as foreground
		if(pdfMatched < 0)
		{
			float pix = 1.0f;
			write_imagef(dst, gid, (float4) pix);
			return;
		}

		// If the Gaussian distribution is classified as a background one,
		// the pixel is classified as background,
		// otherwise pixel represents the foreground
		weightSum = 0.0f;
		for(int mx = 0; mx < nmixtures; ++mx)
		{
			// The first Gaussian distributions which exceed
			// certain threshold (backgroundRatio) are retained for 
			// a background distribution.

			// The other distributions are considered
			// to represent a foreground distribution
			weightSum += MIXTURE(mx).weight;

			if(weightSum > params->backgroundRatio)
			{
				float pix = pdfMatched > mx 
					? 1.0f // foreground
					: 0.0f;  // background
				write_imagef(dst, gid, (float4) pix);
				return;
			}
		}
	}
}