#include "MemoryObject.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value memObjectInfo(cl_mem id, cl_mem_info info)
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

	MemoryObject::~MemoryObject()
	{
		if(id)
			clReleaseMemObject(id);
	}

	EAccess MemoryObject::access() const
	{
		return EAccess(flags() & 
			(CL_MEM_READ_WRITE |
			 CL_MEM_WRITE_ONLY |
			 CL_MEM_READ_ONLY)
		);
	}

	EObjectType MemoryObject::type() const
	{
		return EObjectType(detail::memObjectInfo<cl_mem_object_type>(id, CL_MEM_TYPE));
	}

	cl_mem_flags MemoryObject::flags() const
	{
		return detail::memObjectInfo<cl_mem_flags>(id, CL_MEM_FLAGS);
	}

	void* MemoryObject::hostPointer() const
	{
		return detail::memObjectInfo<void*>(id, CL_MEM_HOST_PTR);
	}

	size_t MemoryObject::size() const
	{
		return detail::memObjectInfo<size_t>(id, CL_MEM_SIZE);
	}

	void MemoryObject::setMemoryId(Context* ctx, cl_mem id)
	{
		this->ctx = ctx;
		if (id)
			clRetainMemObject(id);
		if (this->id)
			clReleaseMemObject(this->id);
		this->id = id;
	}
}
