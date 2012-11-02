#include "Buffer.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value bufferInfo(cl_mem id, cl_mem_info info)
		{
			Value value;
			cl_int error;
			if((error = clGetMemObjectInfo(id, info, sizeof(Value), 
				&value, nullptr)) != CL_SUCCESS)
			{
				reportError("memObjectInfo(): ", error);
				return Value(0);
			}
			return value;
		}
	}

	Buffer::Buffer(const Buffer& other)
	{
		setMemoryId(other.ctx, other.id);
	}

	Buffer& Buffer::operator=(const Buffer& other)
	{
		setMemoryId(other.ctx, other.id);
		return *this;
	}

	Buffer Buffer::createSubBuffer(size_t offset, size_t size, EAccess access)
	{
#if defined(HAVE_OPENCL_1_1)
		cl_int error;
		cl_buffer_region region = {
			/* .origin = */ offset,
			/* .size = */ size
		};
		cl_mem mem = clCreateSubBuffer(memoryId(), cl_mem_flags(access),
			CL_BUFFER_CREATE_TYPE_REGION, &region, &error);
		detail::reportError("Buffer::createSubBuffer(): ", error);
		return Buffer(context(), mem);
#else
		(void) offset;
		(void) size;
		(void) access;
		return Buffer();
#endif
	}

	Buffer Buffer::parentBuffer() const
	{
#if defined(HAVE_OPENCL_1_1)
		cl_mem parent = detail::bufferInfo<cl_mem>
			(memoryId(), CL_MEM_ASSOCIATED_MEMOBJECT);
		if(parent)
			clRetainMemObject(parent);
		return Buffer(ctx, parent);
#else
		return Buffer();
#endif
	}

	size_t Buffer::offset() const
	{
#if defined(HAVE_OPENCL_1_1)
		return detail::bufferInfo<size_t>(memoryId(), CL_MEM_OFFSET);
#else
		return size_t(0);
#endif
	}
}