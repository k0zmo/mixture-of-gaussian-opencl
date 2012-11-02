#pragma once

#include "Prerequisites.h"
#include "MemoryObject.h"

namespace clw
{
	class Buffer : public MemoryObject
	{
	public:
		Buffer() {}
		Buffer(Context* ctx, cl_mem id)
			: MemoryObject(ctx, id) {}

		Buffer(const Buffer& other);
		Buffer& operator=(const Buffer& other);

		Buffer createSubBuffer(size_t offset, size_t size, EAccess access);
		Buffer parentBuffer() const;
		size_t offset() const;
	};
}