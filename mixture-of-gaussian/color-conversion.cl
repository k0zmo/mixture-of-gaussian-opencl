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
	float gray = dot(grayscale, rgba / 255.0f);
#else
	uchar3 rgb = vload3(gid1, src);
	float3 rgba = convert_float3(rgb);
	float gray = dot(grayscale.xyz, rgba / 255.0f);
#endif
	dst[gid1] = convert_uchar_sat(gray * 255.0f);
}