#pragma once

#include "Prerequisites.h"
#include "NDRange.h"

namespace clw
{
	class Kernel
	{
	public:
		Kernel() 
			: ctx(nullptr)
			, id(0)
			, gblWorkOffset(0)
			, gblWorkSize(1)
			, lclWorkSize(0)
		{}
		Kernel(Context* ctx, cl_kernel id)
			: ctx(ctx)
			, id(id)
			, gblWorkOffset(0)
			, gblWorkSize(1)
			, lclWorkSize(0)
		{}
		~Kernel();

		Kernel(const Kernel& other);
		Kernel& operator=(const Kernel& other);

		bool isNull() const { return id == 0; }

		Context* context() const { return ctx; }
		cl_kernel kernelId() const { return id; }

		Program program() const;
		string name() const;
		int argCount() const;

		NDRange globalWorkSize() const;
		void setGlobalWorkSize(const NDRange& range);
		void setGlobalWorkSize(size_t width);
		void setGlobalWorkSize(size_t width, size_t height);
		void setGlobalWorkSize(size_t width, size_t height, size_t depth);

		void setRoundedGlobalWorkSize(const NDRange& range);
		void setRoundedGlobalWorkSize(size_t width);
		void setRoundedGlobalWorkSize(size_t width, size_t height);
		void setRoundedGlobalWorkSize(size_t width, size_t height, size_t depth);

		NDRange localWorkSize() const;
		void setLocalWorkSize(const NDRange& range);
		void setLocalWorkSize(size_t width);
		void setLocalWorkSize(size_t width, size_t height);
		void setLocalWorkSize(size_t width, size_t height, size_t depth);

		NDRange globalWorkOffset() const;
		void setGlobalWorkOffset(const NDRange& range);
		void setGlobalWorkOffset(size_t width);
		void setGlobalWorkOffset(size_t width, size_t height);
		void setGlobalWorkOffset(size_t width, size_t height, size_t depth);

		template<typename Value>
		void setArg(unsigned index, Value value);
		void setArg(unsigned index, const void* data, size_t size);

	private:
		Context* ctx;
		cl_kernel id;

		NDRange gblWorkOffset;
		NDRange gblWorkSize;
		NDRange lclWorkSize;
	};

	template<typename Value>
	inline void Kernel::setArg(unsigned index, Value value)
	{
		setArg(index, &value, sizeof(Value));
	}

	inline void Kernel::setArg(unsigned index, const void* data, size_t size)
	{
		clSetKernelArg(id, index, size, data);
	}

	inline NDRange Kernel::globalWorkSize() const
	{
		return gblWorkSize;
	}

	inline void Kernel::setGlobalWorkSize(const NDRange& range)
	{ 
		gblWorkSize = range;
	}

	inline void Kernel::setGlobalWorkSize(size_t width)
	{
		setGlobalWorkSize(NDRange(width));
	}

	inline void Kernel::setGlobalWorkSize(size_t width, size_t height) 
	{ 
		setGlobalWorkSize(NDRange(width, height));
	}

	inline void Kernel::setGlobalWorkSize(size_t width, size_t height, size_t depth)
	{
		setGlobalWorkSize(NDRange(width, height, depth));
	}

	inline void Kernel::setRoundedGlobalWorkSize(const NDRange& range)
	{
		setGlobalWorkSize(range.roundTo(localWorkSize()));
	}

	inline void Kernel::setRoundedGlobalWorkSize(size_t width)
	{
		setRoundedGlobalWorkSize(NDRange(width));
	}

	inline void Kernel::setRoundedGlobalWorkSize(size_t width, size_t height)
	{
		setRoundedGlobalWorkSize(NDRange(width, height));
	}

	inline void Kernel::setRoundedGlobalWorkSize(size_t width, size_t height, size_t depth)
	{
		setRoundedGlobalWorkSize(NDRange(width, height, depth));
	}

	inline NDRange Kernel::localWorkSize() const
	{
		return lclWorkSize;
	}

	inline void Kernel::setLocalWorkSize(const NDRange& range)
	{ 
		lclWorkSize = range;
	}

	inline void Kernel::setLocalWorkSize(size_t width)
	{
		setLocalWorkSize(width);
	}

	inline void Kernel::setLocalWorkSize(size_t width, size_t height)
	{
		setLocalWorkSize(width, height);
	}

	inline void Kernel::setLocalWorkSize(size_t width, size_t height, size_t depth)
	{
		setLocalWorkSize(width, height, depth);
	}

	inline NDRange Kernel::globalWorkOffset() const
	{
		return gblWorkOffset;
	}

	inline void Kernel::setGlobalWorkOffset(const NDRange& range)
	{ 
		gblWorkOffset = range;
	}

	inline void Kernel::setGlobalWorkOffset(size_t width)
	{
		setGlobalWorkOffset(width);
	}

	inline void Kernel::setGlobalWorkOffset(size_t width, size_t height)
	{
		setGlobalWorkOffset(width, height);
	}

	inline void Kernel::setGlobalWorkOffset(size_t width, size_t height, size_t depth)
	{
		setGlobalWorkOffset(width, height, depth);
	}
}