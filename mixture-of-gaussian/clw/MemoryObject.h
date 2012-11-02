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
		Type_Buffer         = 0x10F0,
		Type_Image2D        = 0x10F1,
		Type_Image3D        = 0x10F2,
		Type_Image2DArray   = 0x10F3,
		Type_Image1D        = 0x10F4,
		Type_Image1DArray   = 0x10F5,
		Type_Image1DBuffer  = 0x10F6
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
