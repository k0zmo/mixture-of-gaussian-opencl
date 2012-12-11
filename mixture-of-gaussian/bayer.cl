__constant uint3 shift = { 0, 2, 2 };
__constant uint3 ddiv = { 2, 5, 2 };
__constant uint3 coeff = { 4899, 9617, 1864 };

__attribute__((always_inline))
 bool opTrue(bool o) { return o; }
__attribute__((always_inline)) 
bool opFalse(bool o) { return !o; }

__attribute__((always_inline)) 
int index2(int x, int y, int row_pitch)
{ return mad24(y, row_pitch, x); }

__attribute__((always_inline))
int descale(int x, int n)
{ return (x + (1 << (n + 1))) >> n; }

#define RADIUSX 1
#define RADIUSY 1

uchar3 convert_bayer2rgb(__global uchar* src, const int imgw, int2 gid, bool x_odd, bool y_odd)
{
	__private uint v[9];
	
	// fetch pixel with its full 3x3 context
	#pragma unroll
	for(int y = 0; y < 3; ++y) 
	{
		#pragma unroll
		for(int x = 0; x < 3; ++x)
		{
			v[index2(x,y,3)] = src[index2(gid.x + x - 1, gid.y + y - 1, imgw)];
		}
	}

	uint sum0 = v[3] + v[5];
	uint sum1 = v[1] + v[7];
	uint sum2 = v[0] + v[2];
	uint sum3 = v[6] + v[8];

	// Dla postaci gdzie G jest 4
	uint r = v[4];
	uint g = sum0 + sum1;
	uint b = sum2 + sum3;
	
	uint3 dd1 = (uint3)(r, g, b) >> shift;
	uchar3 out1 = convert_uchar3(dd1);

 	// Dla postaci gdzie G jest 5 a reszty po 2
	uint rr = sum0;
	uint gg = b + v[4];
	uint bb = sum1;

	uint3 dd2 = (uint3)(rr, gg, bb) / ddiv;
	uchar3 out2 = convert_uchar3(dd2);

 	uchar3 out = x_odd ?
 		(y_odd ? out1.xyz : out2.zyx) :
 		(y_odd ? out2.xyz : out1.zyx);

	return out;
}

#define DEFINE_BAYER_KERNEL_RGB(name, xo, yo) \
	__kernel void name(__global uchar* src, __write_only image2d_t dst, const int2 size) \
	{ \
		int2 gid = { get_global_id(0), get_global_id(1) }; \
		if(gid.x + RADIUSX >= size.x || gid.y + RADIUSY >= size.y) return; \
		bool x_odd = gid.x & 0x01; \
		bool y_odd = gid.y & 0x01; \
		uchar3 out = convert_bayer2rgb(src, size.x, gid, xo(x_odd), yo(y_odd)); \
		write_imagef(dst, gid, (float4)(out.x / 255.0f, out.y / 255.0f, out.z / 255.0f, 0)); \
	}

#define DEFINE_BAYER_KERNEL_GRAY(name, xo, yo) \
	__kernel void name(__global uchar* src, __write_only image2d_t dst, const int2 size) \
	{ \
		int2 gid = { get_global_id(0), get_global_id(1) }; \
		if(gid.x + RADIUSX >= size.x || gid.y + RADIUSY >= size.y) return; \
		int2 lid = { get_local_id(0), get_local_id(1) }; \
		bool x_odd = gid.x & 0x01; \
		bool y_odd = gid.y & 0x01; \
		uchar3 out = convert_bayer2rgb(src, size.x, gid, xo(x_odd), yo(y_odd)); \
		uint3 out_scaled = convert_uint3(out) * coeff; \
		uchar gray = convert_uchar_sat(descale(out_scaled.x + out_scaled.y + out_scaled.z, 14)); \
		write_imagef(dst, gid, (float4) (gray / 255.0f)); \
	}

DEFINE_BAYER_KERNEL_RGB(convert_rg2rgb, opTrue,  opTrue)
DEFINE_BAYER_KERNEL_RGB(convert_gb2rgb, opTrue,  opFalse)
DEFINE_BAYER_KERNEL_RGB(convert_gr2rgb, opFalse, opTrue)
DEFINE_BAYER_KERNEL_RGB(convert_bg2rgb, opFalse, opFalse)

DEFINE_BAYER_KERNEL_GRAY(convert_rg2gray, opTrue,  opTrue)
DEFINE_BAYER_KERNEL_GRAY(convert_gb2gray, opTrue,  opFalse)
DEFINE_BAYER_KERNEL_GRAY(convert_gr2gray, opFalse, opTrue)
DEFINE_BAYER_KERNEL_GRAY(convert_bg2gray, opFalse, opFalse)