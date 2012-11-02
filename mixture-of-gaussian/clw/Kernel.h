#pragma once

#include "Prerequisites.h"

namespace clw
{
	class Kernel
	{
	public:
		Kernel() : ctx(nullptr), id(0) {}
		Kernel(Context* ctx, cl_kernel id)
			: ctx(ctx), id(id) {}
		~Kernel();

		Kernel(const Kernel& other);
		Kernel& operator=(const Kernel& other);

		bool isNull() const { return id == 0; }

		Context* context() const { return ctx; }
		cl_kernel kernelId() const { return id; }

		Program program() const;
		string name() const;
		int argCount() const;

		template<typename Value>
		void setArg(unsigned index, Value value);
		void setArg(unsigned index, const void* data, size_t size);

	private:
		Context* ctx;
		cl_kernel id;
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
}