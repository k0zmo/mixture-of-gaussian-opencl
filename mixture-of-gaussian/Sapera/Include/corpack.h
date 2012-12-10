//
// Coreco format packing/unpacking macros.
//

#ifndef _CORPACK_H_
#define _CORPACK_H_

#include <corenv.h>

/****************************************************************************
*	Format Packing Macros
****************************************************************************/
#define CORDATA_PACK_MONO8(pData, pBuf) \
	*(UINT8 *)(pBuf) = (UINT8) (pData)->mono

#define CORDATA_PACK_MONO16(pData, pBuf) \
	*(UINT16 *)(pBuf) = (UINT16) (pData)->mono

#define CORDATA_PACK_MONO32(pData, pBuf) \
	*(UINT32 *)(pBuf) = (UINT32) (pData)->mono

#define CORDATA_PACK_RGB5551(pData, pBuf) \
	*(UINT16 *)(pBuf) = (UINT16) (((pData)->rgb.red & 0x1F) << 10); \
	*(UINT16 *)(pBuf) |= (UINT16) (((pData)->rgb.green & 0x1F) << 5); \
	*(UINT16 *)(pBuf) |= (UINT16) ((pData)->rgb.blue & 0x1F)

#define CORDATA_PACK_RGB565(pData, pBuf) \
	*(UINT16 *)(pBuf) = (UINT16) (((pData)->rgb.red & 0x1F) << 11); \
	*(UINT16 *)(pBuf) |= (UINT16) (((pData)->rgb.green & 0x3F) << 5); \
	*(UINT16 *)(pBuf) |= (UINT16) ((pData)->rgb.blue & 0x1F)

#define CORDATA_PACK_RGB888(pData, pBuf) \
	*((UINT8 *)(pBuf)+2) = (UINT8) (pData)->rgb.red; \
	*((UINT8 *)(pBuf)+1) = (UINT8) (pData)->rgb.green; \
	*(UINT8 *)(pBuf) = (UINT8) (pData)->rgb.blue

#define CORDATA_PACK_RGB8888(pData, pBuf) \
	CORDATA_PACK_RGB888(pData, pBuf)

#define CORDATA_PACK_RGB101010(pData, pBuf) \
	*(UINT32 *)(pBuf) = (UINT32) (((pData)->rgb.red & 0x3FF) << 20); \
	*(UINT32 *)(pBuf) |= (UINT32) (((pData)->rgb.green & 0x3FF) << 10); \
	*(UINT32 *)(pBuf) |= (UINT32) ((pData)->rgb.blue & 0x3FF)

#define CORDATA_PACK_RGB161616(pData, pBuf) \
	*((UINT16 *)(pBuf)+2) = (UINT16) (pData)->rgb.red; \
	*((UINT16 *)(pBuf)+1) = (UINT16) (pData)->rgb.green; \
	*(UINT16 *)(pBuf) = (UINT16) (pData)->rgb.blue

#define CORDATA_PACK_FLOAT(pData, pBuf) \
	*(FLOAT *)(pBuf) = (pData)->flt

#define CORDATA_PACK_COMPLEX(pData, pBuf) \
	*(FLOAT *)(pBuf) = (pData)->cplx.real; \
	*((FLOAT *)(pBuf)+1) = (pData)->cplx.imag

#define CORDATA_PACK_POINT(pData, pBuf) \
	*(INT32 *)(pBuf) = (pData)->point.x; \
	*((INT32 *)(pBuf)+1) = (pData)->point.y

#define CORDATA_PACK_FPOINT(pData, pBuf) \
	*(FLOAT *)(pBuf) = (pData)->fpoint.x; \
	*((FLOAT *)(pBuf)+1) = (pData)->fpoint.y

#define CORDATA_PACK_HSV(pData, pBuf) \
	*((UINT8 *)(pBuf)+2) = (UINT8) (pData)->hsv.h; \
	*((UINT8 *)(pBuf)+1) = (UINT8) (pData)->hsv.s; \
	*(UINT8 *)(pBuf) = (UINT8) (pData)->hsv.v

#define CORDATA_PACK_YUV(pData, pBuf) \
	*((UINT8 *)(pBuf)+2) = (UINT8) (pData)->yuv.y; \
	*((UINT8 *)(pBuf)+1) = (UINT8) (pData)->yuv.u; \
	*(UINT8 *)(pBuf) = (UINT8) (pData)->yuv.v

#define CORDATA_PACK_UYVY(pData, pBuf, xPos) \
	*(UINT8 *)(pBuf) = (UINT8) (pData)->yuv.u; \
	*((UINT8 *)(pBuf)+2) = (UINT8) (pData)->yuv.v; \
	*((UINT8 *)(pBuf)+1 + ((xPos & 1) ? 2 : 0)) = (UINT8) (pData)->yuv.y

#define CORDATA_PACK_YUY2(pData, pBuf, xPos) \
	*((UINT8 *)(pBuf)+1) = (UINT8) (pData)->yuv.u; \
	*((UINT8 *)(pBuf)+3) = (UINT8) (pData)->yuv.v; \
	*((UINT8 *)(pBuf) + ((xPos & 1) ? 2 : 0)) = (UINT8) (pData)->yuv.y

/****************************************************************************
*	Format Unpacking Macros
****************************************************************************/
#define CORDATA_UNPACK_MONO8_U(pBuf, pData) \
	(pData)->mono = (INT32) *(UINT8 *)(pBuf)

#define CORDATA_UNPACK_MONO8_S(pBuf, pData) \
	(pData)->mono = (INT32) *(INT8 *)(pBuf)

#define CORDATA_UNPACK_MONO16_U(pBuf, pData) \
	(pData)->mono = (INT32) *(UINT16 *)(pBuf)

#define CORDATA_UNPACK_MONO16_S(pBuf, pData) \
	(pData)->mono = (INT32) *(INT16 *)(pBuf)

#define CORDATA_UNPACK_MONO32_U(pBuf, pData) \
	(pData)->mono = (INT32) *(UINT32 *)(pBuf)

#define CORDATA_UNPACK_MONO32_S(pBuf, pData) \
	(pData)->mono = (INT32) *(INT32 *)(pBuf)

#define CORDATA_UNPACK_RGB5551(pBuf, pData) \
	(pData)->rgb.red  = (INT32) (*(UINT16 *)(pBuf) >> 10) & 0x1F; \
	(pData)->rgb.green= (INT32) (*(UINT16 *)(pBuf) >> 5) & 0x1F; \
	(pData)->rgb.blue = (INT32) (*(UINT16 *)(pBuf)) & 0x1F

#define CORDATA_UNPACK_RGB565(pBuf, pData) \
	(pData)->rgb.red  = (INT32) (*(UINT16 *)(pBuf) >> 11) & 0x1F; \
	(pData)->rgb.green= (INT32) (*(UINT16 *)(pBuf) >> 5) & 0x3F; \
	(pData)->rgb.blue = (INT32) (*(UINT16 *)(pBuf)) & 0x1F

#define CORDATA_UNPACK_RGB888(pBuf, pData) \
	(pData)->rgb.red  = (INT32) *((UINT8 *)(pBuf)+2); \
	(pData)->rgb.green= (INT32) *((UINT8 *)(pBuf)+1); \
	(pData)->rgb.blue = (INT32) *(UINT8 *)(pBuf)

#define CORDATA_UNPACK_RGB8888(pBuf, pData) \
	CORDATA_UNPACK_RGB888(pBuf, pData)

#define CORDATA_UNPACK_RGB101010(pBuf, pData) \
	(pData)->rgb.red  = (INT32) (*(UINT32 *)(pBuf) >> 20) & 0x3FF; \
	(pData)->rgb.green= (INT32) (*(UINT32 *)(pBuf) >> 10) & 0x3FF; \
	(pData)->rgb.blue = (INT32) (*(UINT32 *)(pBuf)) & 0x3FF

#define CORDATA_UNPACK_RGB161616(pBuf, pData) \
	(pData)->rgb.red  = (INT32) *((UINT16 *)(pBuf)+2); \
	(pData)->rgb.green= (INT32) *((UINT16 *)(pBuf)+1); \
	(pData)->rgb.blue = (INT32) *(UINT16 *)(pBuf)

#define CORDATA_UNPACK_FLOAT(pBuf, pData) \
	(pData)->flt = *(FLOAT *)(pBuf)

#define CORDATA_UNPACK_COMPLEX(pBuf, pData) \
	(pData)->cplx.real = *(FLOAT *)(pBuf); \
	(pData)->cplx.imag = *((FLOAT *)(pBuf)+1)

#define CORDATA_UNPACK_POINT(pBuf, pData) \
	(pData)->point.x = *(INT32 *)(pBuf); \
	(pData)->point.y = *((INT32 *)(pBuf)+1)

#define CORDATA_UNPACK_FPOINT(pBuf, pData) \
	(pData)->fpoint.x = *(FLOAT *)(pBuf); \
	(pData)->fpoint.y = *((FLOAT *)(pBuf)+1)

#define CORDATA_UNPACK_HSV(pBuf, pData) \
	(pData)->hsv.h  = (INT32) *((UINT8 *)(pBuf)+2); \
	(pData)->hsv.s= (INT32) *((UINT8 *)(pBuf)+1); \
	(pData)->hsv.v = (INT32) *(UINT8 *)(pBuf)

#define CORDATA_UNPACK_YUV(pBuf, pData) \
	(pData)->yuv.y  = (INT32) *((UINT8 *)(pBuf)+2); \
	(pData)->yuv.u= (INT32) *((UINT8 *)(pBuf)+1); \
	(pData)->yuv.v = (INT32) *(UINT8 *)(pBuf)

#define CORDATA_UNPACK_UYVY(pBuf, pData, xPos) \
	(pData)->yuv.u = (INT32) *(UINT8 *)(pBuf); \
	(pData)->yuv.v = (INT32) *((UINT8 *)(pBuf)+2); \
	(pData)->yuv.y = (INT32) *((UINT8 *)(pBuf)+1 + ((xPos & 1) ? 2 : 0))

#define CORDATA_UNPACK_YUY2(pBuf, pData, xPos) \
	(pData)->yuv.u = (INT32) *((UINT8 *)(pBuf)+1); \
	(pData)->yuv.v = (INT32) *((UINT8 *)(pBuf)+3); \
	(pData)->yuv.y = (INT32) *((UINT8 *)(pBuf) + ((xPos & 1) ? 2 : 0))

#endif   // _CORPACK_H_
