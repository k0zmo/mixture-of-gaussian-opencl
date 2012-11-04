#include "Kernel.h"
#include "Program.h"

namespace clw
{
	namespace detail
	{
		template<typename Value>
		Value kernelInfo(cl_kernel id, cl_kernel_info info)
		{
			Value value;
			cl_int error;
			if((error = clGetKernelInfo(id, info, sizeof(value),
			        &value, nullptr)) != CL_SUCCESS)
			{
				reportError("kernelInfo(): ", error);
				return Value(0);
			}
			return value;
		}
	}

	Kernel::~Kernel()
	{
		if(id)
			clReleaseKernel(id);
	}

	Kernel::Kernel(const Kernel& other)
		: ctx(other.ctx)
		, id(other.id)
		, gblWorkOffset(other.gblWorkOffset)
		, gblWorkSize(other.gblWorkSize)
		, lclWorkSize(other.lclWorkSize)
	{
		if(id)
			clRetainKernel(id);
	}

	Kernel& Kernel::operator=(const Kernel& other)
	{
		ctx = other.ctx;
		if(other.id)
			clRetainKernel(other.id);
		if(id)
			clReleaseKernel(id);
		id = other.id;
		gblWorkOffset = other.gblWorkOffset;
		gblWorkSize = other.gblWorkSize;
		lclWorkSize = other.lclWorkSize;
		return *this;
	}

	Program Kernel::program() const
	{
		if(isNull())
			return Program();
		cl_program pid = detail::kernelInfo<cl_program>(id, CL_KERNEL_PROGRAM);
		if(!pid)
			return Program();
		return Program(ctx, pid);
	}

	string Kernel::name() const
	{
		if(isNull())
			return string();
		size_t size;
		cl_int error;
		if((error = clGetKernelInfo(id, CL_KERNEL_FUNCTION_NAME,
		        0, nullptr, &size)) != CL_SUCCESS || !size)
			return string();
		string n;
		n.resize(size);
		if((error = clGetKernelInfo(id, CL_KERNEL_FUNCTION_NAME,
		        size, const_cast<char*>(n.data()), nullptr)) != CL_SUCCESS)
			return string();
		return n;
	}

	int Kernel::argCount() const
	{
		if(isNull())
			return 0;
		return int(detail::kernelInfo<cl_uint>(id, CL_KERNEL_NUM_ARGS));
	}
}