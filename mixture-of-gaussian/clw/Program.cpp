#include "Program.h"
#include "Context.h"
#include "Kernel.h"
#include "StlUtils.h"

namespace clw
{
	namespace detail
	{
		extern "C" static void CL_API_CALL buildNotify(cl_program id, void* userData)
		{
			(void) id;
			(void) userData;
			std::cerr << "Build notification" << std::endl;
		}

		template<typename Value>
		Value programInfo(cl_program id, cl_program_info info)
		{
			Value value;
			cl_int error = 0;
			if((error = clGetProgramInfo(id, info, 
			        sizeof(Value), &value, nullptr)) != CL_SUCCESS)
			{
				reportError("programInfo(): ", error);
				return Value(0);
			}
			return value;
		}

		template<typename Value>
		bool programInfo(cl_program id, cl_program_info info, Value* buf, size_t length)
		{
			cl_int error = 0;
			if((error = clGetProgramInfo(id, info, 
			        sizeof(Value) * length, buf, nullptr)) != CL_SUCCESS)
			{
				reportError("programInfo(): ", error);
				return false;
			}
			return true;
		}
	}

	Program::~Program()
	{
		if(id)
			clReleaseProgram(id);
	}

	Program::Program(const Program& other)
		: ctx(other.ctx), id(other.id)
	{
		if(id)
			clRetainProgram(id);
	}

	Program& Program::operator=(const Program& other)
	{
		ctx = other.ctx;
		if(other.id)
			clRetainProgram(other.id);
		if(id)
			clReleaseProgram(id);
		id = other.id;
		return *this;
	}

	// Build program for all devices associated with program for which a source or binary has been loaded
	bool Program::build(const string& options)
	{
		if(!ctx)
			return false;
		cl_int error = clBuildProgram(id, 0, nullptr, 
		                              options.c_str(), 
		                              &detail::buildNotify, nullptr);
		detail::reportError("Program::build(): ", error);
		return error == CL_SUCCESS;
	}

	string Program::log() const
	{
		vector<Device> devs = devices();
		if(devs.empty())
			return string();
		string accLog;

		for_each(devs, [this, &accLog](const Device& dev)
		{
			size_t size;
			cl_int error;
			if((error = clGetProgramBuildInfo(id, dev.deviceId(),
			        CL_PROGRAM_BUILD_LOG, 0, nullptr, &size)) != CL_SUCCESS)
			{
				detail::reportError("Program::log(): ", error);
				return; // works like continue for ordinary loops
			};
			string buf;
			buf.resize(size);
			if((error = clGetProgramBuildInfo(id, dev.deviceId(), 
			        CL_PROGRAM_BUILD_LOG, size, 
					const_cast<char*>(buf.data()), nullptr)) != CL_SUCCESS)
			{
				detail::reportError("Program::log(): ", error);
				return; // works like continue for ordinary loops
			}
			accLog += buf;
		});
		return accLog;
	}

	vector<Device> Program::devices() const
	{
		if(!ctx || isNull())
			return vector<Device>();
		// !NOTE: Some claims CL_PROGRAM_DEVICES is unreliable
		cl_uint size = detail::programInfo<cl_uint>(id, CL_PROGRAM_NUM_DEVICES);
		if(size == 0)
			return vector<Device>();
		vector<cl_device_id> buf(size);
		if(detail::programInfo(id, CL_PROGRAM_DEVICES, buf.data(), buf.size()))
			return vector<Device>();
		vector<Device> devs(size);
		for(cl_uint i = 0; i < size; ++i)
			devs[i] = Device(buf[i]);
		return devs;
	}

	string Program::sourceCode() const
	{
		if(!ctx || isNull())
			return string();
		size_t size;
		cl_int error;		
		if((error = clGetProgramInfo(id, CL_PROGRAM_SOURCE,
		        0, nullptr, &size)) != CL_SUCCESS)
		{
			detail::reportError("Program::sourceCode(): ", error);
			return string();
		}
		if(size == 0)
			return string();
		string buf;
		buf.resize(size);
		if(detail::programInfo(id, CL_PROGRAM_SOURCE, 
		        const_cast<char*>(buf.data()), size))
			return string();
		return buf;
	}

	vector<ByteCode> Program::binaries() const
	{
		vector<ByteCode> bins;
		cl_uint size;
		if((size = detail::programInfo<cl_uint>(id, CL_PROGRAM_NUM_DEVICES)) == 0)
			return bins;
		vector<size_t> binSizes(size);
		if(!detail::programInfo(id, CL_PROGRAM_BINARY_SIZES,
		        binSizes.data(), size))
			return bins;
		vector<unsigned char*> binPtrs;
		for(size_t i = 0; i < size; ++i)
		{
			bins.push_back(ByteCode(binSizes[i], '\0'));
			binPtrs.push_back(bins[i].data());
		}
		cl_int error;
		if((error = clGetProgramInfo(id, CL_PROGRAM_BINARIES, 
		        size * sizeof(unsigned char*), binPtrs.data(), nullptr))
		        != CL_SUCCESS)
		{
			detail::reportError("Program::binaries(): ", error);
			return vector<ByteCode>();
		}
		return bins;
	}

	Kernel Program::createKernel(const char* name) const
	{
		cl_int error;
		cl_kernel kernel = clCreateKernel(id, name, &error);
		if(error == CL_SUCCESS && kernel)
			return Kernel(ctx, kernel);
		detail::reportError("Program::createKernel(): ", error);
		return Kernel();
	}

	Kernel Program::createKernel(const string& name) const
	{
		return createKernel(name.c_str());
	}

	vector<Kernel> Program::createKernels() const
	{
		vector<Kernel> vec;
		cl_uint size;
		cl_int error;
		if((error = clCreateKernelsInProgram(id, 0,
		        nullptr, &size)) != CL_SUCCESS)
		{
			detail::reportError("Program::createKernels(): ", error);
			return vec;
		}
		vector<cl_kernel> buf(size);
		if(clCreateKernelsInProgram(id, size, 
		        buf.data(), nullptr) != CL_SUCCESS)
		{
			detail::reportError("Program::createKernels(): ", error);
			return vec;
		}
		for(cl_uint i = 0; i < size; ++i)
			vec.push_back(Kernel(ctx, buf[i]));
		return vec;
	}
}