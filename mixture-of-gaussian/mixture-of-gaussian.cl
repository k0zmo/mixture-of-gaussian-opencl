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
	
	if(!all(gid < size))
		return;

	float4 rgba = read_imagef(src, smp, gid);
	float gray = dot(grayscale, rgba);
	write_imagef(dst, gid, (float4) gray);
}

__kernel void rgb2gray_image(
	__global uchar* src,
	__write_only image2d_t dst,
	const int2 size)
{
	const int2 gid = { get_global_id(0), get_global_id(1) };
	
	if(!all(gid < size))
		return;
		
	const int gid1 = gid.x + gid.y * size.x;
	
#if __OPENCL_VERSION__ == CL_VERSION_1_0
	float r = convert_float(src[3*gid1 + 0]);
	float g = convert_float(src[3*gid1 + 1]);
	float b = convert_float(src[3*gid1 + 2]);
	float4 rgba = { r, g, b, 0 };
	float gray = dot(grayscale, rgba / 255.0f);
#else
	uchar3 rgb = vload3(gid1, src);
	float3 rgba = convert_float3(rgb);
	float gray = dot(grayscale.xyz, rgba / 255.0f);
#endif

	write_imagef(dst, gid, (float4) gray);
}

__kernel void rgb2gray(
	__global uchar* src,
	__global uchar* dst,
	const int2 size)
{
	const int2 gid = { get_global_id(0), get_global_id(1) };
	
	if(!all(gid < size))
		return;
		
	const int gid1 = gid.x + gid.y * size.x;

#if __OPENCL_VERSION__ == CL_VERSION_1_0
	float r = convert_float(src[3*gid1 + 0]);
	float g = convert_float(src[3*gid1 + 1]);
	float b = convert_float(src[3*gid1 + 2]);
	float4 rgba = { r, g, b, 0 };
	float gray = dot(grayscale_scaled, rgba / 255.0f);
#else
	uchar3 rgb = vload3(gid1, src);
	float3 rgba = convert_float3(rgb);
	float gray = dot(grayscale.xyz, rgba / 255.0f);
#endif
	dst[gid1] = convert_uchar_sat(gray * 255.0f);
}

typedef struct MixtureData
{
	float weight;
	float mean;
	float var;
} MixtureData;

typedef struct MogParams
{
	float varThreshold;
	float backgroundRatio;
	float w0; // waga dla nowej mikstury
	float var0; // wariancja dla nowej mikstury
	float minVar; // dolny prog mozliwej wariancji
} MogParams;

#ifndef nmixtures 
#define nmixtures 5
#endif

#define MIXTURE(mx) mptr[gid1 * nmixtures + mx]

__kernel void mog_image(
	__read_only image2d_t frame,
	__write_only image2d_t dst,
	__global MixtureData* mptr,
	__constant MogParams* params,
	const float alpha) // krzywa uczenia
{
	const int2 gid = { get_global_id(0), get_global_id(1) };
	const int2 size = { get_image_width(frame), get_image_height(frame) };
	
	if (!all(gid < size))
		return;
		
	float pix = read_imagef(frame, smp, gid).x * 255.0f;
	const int gid1 = gid.x + gid.y * size.x;
	int pdfMatched = -1;

	__private float weight[nmixtures];
	__private float mean[nmixtures];
	__private float var[nmixtures];
	__private float sortKey[nmixtures];

	#pragma unroll nmixtures
	for(int mx = 0; mx < nmixtures; ++mx)
	{
		weight[mx] = MIXTURE(mx).weight;
		mean[mx] = MIXTURE(mx).mean;
		var[mx] = MIXTURE(mx).var;

		if(pdfMatched < 0)
		{
			float diff = pix - mean[mx];
			float d2 = diff*diff;
			float threshold = params->varThreshold * var[mx];
		
			// To samo co:
			// if (diff > -2.5f * var && 
			//     diff < +2.5f * var)

			// Mahalanobis distance
			if(d2 < threshold)
				pdfMatched = mx;
		}
	}
	
	if(pdfMatched < 0)
	{
		// No matching mixture found - replace the weakest one
		pdfMatched = nmixtures - 1; 

		weight[pdfMatched] = params->w0;
		mean[pdfMatched] = pix;
		var[pdfMatched] = params->var0;
	}
	else
	{
		#pragma unroll nmixtures
		for(int mx = 0; mx < nmixtures; ++mx)
		{
			if(mx == pdfMatched)
			{
				float diff = pix - mean[mx];

				//static const float PI = 3.14159265358979323846f;
				//float ni = 1.0f / sqrtf(2.0f * PI * var) * expf(-0.5f * diff*diff / var);

				weight[mx] = weight[mx] + alpha * (1 - weight[mx]);
				mean[mx] = mean[mx] + alpha * diff;
				var[mx] = max(params->minVar, var[mx] + alpha * (diff*diff - var[mx]));
			}
			else
			{
				// For the unmatched mixtures, mean and variance
				// are unchanged, only the weight is replaced by:
				// weight = (1 - alpha) * weight;

				weight[mx] = (1 - alpha) * weight[mx];
			}
		}
	}

	// Normalize weight and calculate sortKey
	float weightSum = 0.0f;
	#pragma unroll nmixtures
	for(int mx = 0; mx < nmixtures; ++mx)
		weightSum += weight[mx];

	float invSum = 1.0f / weightSum;
	#pragma unroll nmixtures
	for(int mx = 0; mx < nmixtures; ++mx)
	{
		weight[mx] *= invSum;
		sortKey[mx] = var[mx] > FLT_MIN
			? weight[mx] / sqrt(var[mx])
			: 0;
	}

	// Sort mixtures (buble sort).
	// Every mixtures but the one with "completely new" weight and variance
	// are already sorted thus we need to reorder only that single mixture.
	for(int mx = 0; mx < pdfMatched; ++mx)
	{
		if(sortKey[pdfMatched] > sortKey[mx])
		{
			float weightTemp = weight[pdfMatched];
			float meanTemp = mean[pdfMatched];
			float varTemp = var[pdfMatched];

			weight[pdfMatched] = weight[mx];
			mean[pdfMatched] = mean[mx];
			var[pdfMatched] = var[mx];

			weight[mx] = weightTemp;
			mean[mx] = meanTemp;
			var[mx] = varTemp;
			break;
		}
	}

	#pragma unroll nmixtures
	for(int mx = 0; mx < nmixtures; ++mx)
	{
		MIXTURE(mx).weight = weight[mx];
		MIXTURE(mx).mean = mean[mx];
		MIXTURE(mx).var = var[mx];
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
		weightSum += weight[mx];

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