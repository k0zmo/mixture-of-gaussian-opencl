#include "Image.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value imageInfo(cl_mem id, cl_image_info info)
		{
			Value value;
			cl_int error;
			if((error = clGetImageInfo(id, info, sizeof(Value),
			        &value, nullptr)) != CL_SUCCESS)
			{
				reportError("imageInfo(): ", error);
				Value value = {0};
				return value;
			}
			return value;
		}
	}

	Image2D::Image2D(const Image2D& other)
	{
		setMemoryId(other.ctx, other.id);
	}

	Image2D& Image2D::operator=(const Image2D& other)
	{
		setMemoryId(other.ctx, other.id);
		return *this;
	}

	ImageFormat Image2D::format() const
	{
		if(fmt.isNull())
		{
			cl_image_format iformat = detail::imageInfo<cl_image_format>
				(memoryId(), CL_IMAGE_FORMAT);
			fmt.order = EChannelOrder();
			fmt.type = EChannelType();
		}
		return fmt;
	}

	int Image2D::width() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_WIDTH);
	}

	int Image2D::height() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_HEIGHT);
	}

	int Image2D::bytesPerElement() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_ELEMENT_SIZE);
	}

	int Image2D::bytesPerLine() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_ROW_PITCH);
	}

	Image3D::Image3D(const Image3D& other)
	{
		setMemoryId(other.ctx, other.id);
	}

	Image3D& Image3D::operator=(const Image3D& other)
	{
		setMemoryId(other.ctx, other.id);
		return *this;
	}

	ImageFormat Image3D::format() const
	{
		if(fmt.isNull())
		{
			cl_image_format iformat = detail::imageInfo<cl_image_format>
				(memoryId(), CL_IMAGE_FORMAT);
			fmt.order = EChannelOrder();
			fmt.type = EChannelType();
		}
		return fmt;
	}

	int Image3D::width() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_WIDTH);
	}

	int Image3D::height() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_HEIGHT);
	}

	int Image3D::depth() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_DEPTH);
	}

	int Image3D::bytesPerElement() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_ELEMENT_SIZE);
	}

	int Image3D::bytesPerLine() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_ROW_PITCH);
	}

	int Image3D::bytesPerSlice() const
	{
		return detail::imageInfo<size_t>(memoryId(), CL_IMAGE_SLICE_PITCH);
	}
}