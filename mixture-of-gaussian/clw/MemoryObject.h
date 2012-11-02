#pragma once

#include "Prerequisites.h"

namespace clw
{
	enum EAccess
	{
		ReadWrite    = (1 << 0),
		WriteOnly    = (1 << 1),
		ReadOnly     = (1 << 2)                        
	};

	enum EType
	{
		UndefinedType   = 0,
		Buffer          = CL_MEM_OBJECT_BUFFER,
		Image2D         = CL_MEM_OBJECT_IMAGE2D,
		Image3D         = CL_MEM_OBJECT_IMAGE3D,
#ifdef HAVE_OPENCL_1_2
		Image2DArray    = CL_MEM_OBJECT_IMAGE2D_ARRAY,
		Image1D         = CL_MEM_OBJECT_IMAGE1D,
		Image1DArray    = CL_MEM_OBJECT_IMAGE1D_ARRAY,
		Image1DBuffer   = CL_MEM_OBJECT_IMAGE1D_BUFFER,
#endif
	};

	class MemoryObject
	{
	public:
		bool isNull() const { return id == 0; }
		cl_mem memoryId() const { return id; }
		Context* context() const { return ctx; }

		EAccess access() const;
		EType type() const;
		cl_mem_flags flags() const;
		void* hostPointer() const;
		size_t size() const;

	protected:
		// Disable instantiating base class 
		MemoryObject(Context* ctx = nullptr) : ctx(ctx), id(0) {}
		MemoryObject(Context* ctx, cl_mem id)
			: ctx(ctx), id(id) {}
		~MemoryObject();

	private:
		Context* ctx;
		cl_mem id;

	private:
		// Disable copying
		MemoryObject(const MemoryObject& other);
		MemoryObject& operator=(const MemoryObject& other);
	};
}