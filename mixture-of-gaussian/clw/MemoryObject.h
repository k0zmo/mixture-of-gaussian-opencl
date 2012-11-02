#pragma once

#include "Prerequisites.h"

namespace clw
{
	enum EAccess
	{
		Access_ReadWrite    = (1 << 0),
		Access_WriteOnly    = (1 << 1),
		Access_ReadOnly     = (1 << 2)                        
	};

	enum EType
	{
		Type_Undefined      = 0,
		Type_Buffer         = CL_MEM_OBJECT_BUFFER,
		Type_Image2D        = CL_MEM_OBJECT_IMAGE2D,
		Type_Image3D        = CL_MEM_OBJECT_IMAGE3D,
#ifdef HAVE_OPENCL_1_2
		Type_Image2DArray   = CL_MEM_OBJECT_IMAGE2D_ARRAY,
		Type_Image1D        = CL_MEM_OBJECT_IMAGE1D,
		Type_Image1DArray   = CL_MEM_OBJECT_IMAGE1D_ARRAY,
		Type_Image1DBuffer  = CL_MEM_OBJECT_IMAGE1D_BUFFER,
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

		void setMemoryId(Context* ctx, cl_mem id);

	protected:
		Context* ctx;
		cl_mem id;

	private:
		// Disable copying
		MemoryObject(const MemoryObject& other);
		MemoryObject& operator=(const MemoryObject& other);
	};
}