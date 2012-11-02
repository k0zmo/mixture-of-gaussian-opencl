#pragma once

#include "Prerequisites.h"
#include "MemoryObject.h"

namespace clw
{
	// !TODO: Add new classes to reflect changes in OpenCL 1.2

	enum EChannelOrder
	{
		Order_R           = 0x10B0,
		Order_A           = 0x10B1,
		Order_RG          = 0x10B2,
		Order_RA          = 0x10B3,
		Order_RGB         = 0x10B4,
		Order_RGBA        = 0x10B5,
		Order_BGRA        = 0x10B6,
		Order_ARGB        = 0x10B7,
		Order_Intensity   = 0x10B8,
		Order_Luminance   = 0x10B9,
		Order_Rx          = 0x10BA,
		Order_RGx         = 0x10BB,
		Order_RGBx        = 0x10BC
	};

	enum EChannelType
	{
		Type_Normalized_Int8           = 0x10D0,
		Type_Normalized_Int16          = 0x10D1,
		Type_Normalized_UInt8          = 0x10D2,
		Type_Normalized_UInt16         = 0x10D3,
		Type_Normalized_565            = 0x10D4,
		Type_Normalized_555            = 0x10D5,
		Type_Normalized_101010         = 0x10D6,
		Type_Unnormalized_Int8         = 0x10D7,
		Type_Unnormalized_Int16        = 0x10D8,
		Type_Unnormalized_Int32        = 0x10D9,
		Type_Unnormalized_Uint8        = 0x10DA,
		Type_Unnormalized_Uint16       = 0x10DB,
		Type_Unnormalized_UInt32       = 0x10DC,
		Type_HalfFloat                 = 0x10DD,
		Type_Float                     = 0x10DE
	};

	struct ImageFormat
	{
		EChannelOrder order;
		EChannelType type;

		ImageFormat() : order(EChannelOrder(0)), type(EChannelType(0)) {}
		ImageFormat(EChannelOrder order, EChannelType type)
			: order(order), type(type) {}

		bool isNull() const
		{
			return order == EChannelOrder(0) ||
				type == EChannelType(0);
		}
	};

	class Image2D : public MemoryObject
	{
	public:
		Image2D() {}
		Image2D(Context* ctx, cl_mem id)
			: MemoryObject(ctx, id) {}

		Image2D(const Image2D& other);
		Image2D& operator=(const Image2D& other);

		ImageFormat format() const;

		int width() const;
		int height() const;

		int bytesPerElement() const;
		int bytesPerLine() const;

	private:
		mutable ImageFormat fmt;
	};

	class Image3D : public MemoryObject
	{
	public:
		Image3D() {}
		Image3D(Context* ctx, cl_mem id)
			: MemoryObject(ctx, id) {}

		Image3D(const Image3D& other);
		Image3D& operator=(const Image3D& other);

		ImageFormat format() const;

		int width() const;
		int height() const;
		int depth() const;

		int bytesPerElement() const;
		int bytesPerLine() const;
		int bytesPerSlice() const;

	private:
		mutable ImageFormat fmt;
	};
}
