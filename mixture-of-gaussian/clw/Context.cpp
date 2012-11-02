#include "Context.h"
#include "Platform.h"
#include "CommandQueue.h"
#include "Program.h"

#include <iostream>
#include <fstream>

namespace clw
{
	namespace detail
	{
		extern "C" void CL_API_CALL contextNotify(const char* errInfo,
		                                                 const void* privateInfo,
		                                                 size_t cb,
		                                                 void* userData)
		{
			(void) privateInfo;
			(void) cb;
			(void) userData;
			std::cerr << "Context notification: " << errInfo << std::endl;
		}

		string errorName(cl_int eid)
		{
			#define CASE(X) case X: return string(#X);
			switch(eid)
			{
			CASE(CL_SUCCESS);
			CASE(CL_DEVICE_NOT_FOUND);
			CASE(CL_DEVICE_NOT_AVAILABLE);
			CASE(CL_COMPILER_NOT_AVAILABLE);
			CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
			CASE(CL_OUT_OF_RESOURCES);
			CASE(CL_OUT_OF_HOST_MEMORY);
			CASE(CL_PROFILING_INFO_NOT_AVAILABLE);
			CASE(CL_MEM_COPY_OVERLAP);
			CASE(CL_IMAGE_FORMAT_MISMATCH);
			CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
			CASE(CL_BUILD_PROGRAM_FAILURE);
			CASE(CL_MAP_FAILURE);
			CASE(CL_MISALIGNED_SUB_BUFFER_OFFSET);
			CASE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
			CASE(CL_INVALID_VALUE);
			CASE(CL_INVALID_DEVICE_TYPE);
			CASE(CL_INVALID_PLATFORM);
			CASE(CL_INVALID_DEVICE);
			CASE(CL_INVALID_CONTEXT);
			CASE(CL_INVALID_QUEUE_PROPERTIES);
			CASE(CL_INVALID_COMMAND_QUEUE);
			CASE(CL_INVALID_HOST_PTR);
			CASE(CL_INVALID_MEM_OBJECT);
			CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			CASE(CL_INVALID_IMAGE_SIZE);
			CASE(CL_INVALID_SAMPLER);
			CASE(CL_INVALID_BINARY);
			CASE(CL_INVALID_BUILD_OPTIONS);
			CASE(CL_INVALID_PROGRAM);
			CASE(CL_INVALID_PROGRAM_EXECUTABLE);
			CASE(CL_INVALID_KERNEL_NAME);
			CASE(CL_INVALID_KERNEL_DEFINITION);
			CASE(CL_INVALID_KERNEL);
			CASE(CL_INVALID_ARG_INDEX);
			CASE(CL_INVALID_ARG_VALUE);
			CASE(CL_INVALID_ARG_SIZE);
			CASE(CL_INVALID_KERNEL_ARGS);
			CASE(CL_INVALID_WORK_DIMENSION);
			CASE(CL_INVALID_WORK_GROUP_SIZE);
			CASE(CL_INVALID_WORK_ITEM_SIZE);
			CASE(CL_INVALID_GLOBAL_OFFSET);
			CASE(CL_INVALID_EVENT_WAIT_LIST);
			CASE(CL_INVALID_EVENT);
			CASE(CL_INVALID_OPERATION);
			CASE(CL_INVALID_GL_OBJECT);
			CASE(CL_INVALID_BUFFER_SIZE);
			CASE(CL_INVALID_MIP_LEVEL);
			CASE(CL_INVALID_GLOBAL_WORK_SIZE);
			CASE(CL_INVALID_PROPERTY);
#ifdef HAVE_OPENCL_1_2
			CASE(CL_COMPILE_PROGRAM_FAILURE);
			CASE(CL_LINKER_NOT_AVAILABLE);
			CASE(CL_LINK_PROGRAM_FAILURE);
			CASE(CL_DEVICE_PARTITION_FAILED);
			CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
			CASE(CL_INVALID_IMAGE_DESCRIPTOR);
			CASE(CL_INVALID_COMPILER_OPTIONS);
			CASE(CL_INVALID_LINKER_OPTIONS);
			CASE(CL_INVALID_DEVICE_PARTITION_COUNT);
#endif
			// cl_khr_icd extension
			CASE(CL_PLATFORM_NOT_FOUND_KHR);
			// cl_khr_gl_sharing
			CASE(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR);
			default: return string("Error ") + std::to_string(
				static_cast<unsigned long long>(eid));
			}
			#undef CASE
		}

		void reportError(const char* name, cl_int eid)
		{
			if(eid != CL_SUCCESS)
				std::cerr << name << errorName(eid) << std::endl;;
		}

		bool readAsString(const string& filename, string* contents)
		{
			std::ifstream strm;
			strm.open(filename.c_str(), std::ios::binary | std::ios_base::in);
			if(!strm.is_open())
			{
				std::cerr << "Unable to open file " << filename << std::endl;
				return false;
			}
			strm.seekg(0, std::ios::end);
			contents->reserve(static_cast<size_t>(strm.tellg()));
			strm.seekg(0);

			contents->assign(std::istreambuf_iterator<char>(strm),
				std::istreambuf_iterator<char>());
			return true;
		}
	}

	Context::Context() 
		: id(0)
		, isCreated(false)
		, eid(CL_SUCCESS)
	{
	}

	Context::~Context()
	{
		if(isCreated && id != 0)
			clReleaseContext(id);
	}

	bool Context::create(EDeviceType type)
	{
		if(isCreated)
			return true;
		vector<Platform> pls = clw::availablePlatforms();
		for(size_t i = 0; i < pls.size(); ++i)
		{
			vector<Device> devices = clw::devices(type, pls[i]);
			for(size_t j = 0; j < devices.size(); ++j)
			{
				cl_device_id did = devices[j].deviceId();
				cl_context_properties props[] = {
					CL_CONTEXT_PLATFORM, cl_context_properties(pls[i].platformId()),
					0, 0
				};
				if((id = clCreateContext(props, 1, &did, 
					    &detail::contextNotify, nullptr, &eid)) != 0)
				{
					devs.clear();
					devs.push_back(Device(did));
					isCreated = true;
					return true;
				}
				detail::reportError("Context::create(type): ", eid);
			}
		}
		isCreated = false;
		return false;		
	}

	bool Context::create(const vector<Device>& devices)
	{
		if(isCreated)
			return true;
		if(devices.empty())
			return false;
		vector<cl_device_id> dids(devices.size());
		for(size_t i = 0; i < devices.size(); ++i)
			dids[i] = devices[i].deviceId();
		cl_platform_id pid = devices[0].platform().platformId();
		cl_context_properties props[] = {
			CL_CONTEXT_PLATFORM, cl_context_properties(pid),
			0, 0
		};
		if((id = clCreateContext(props, dids.size(), dids.data(), 
		        &detail::contextNotify, nullptr, &eid)) != 0)
		{
			size_t size;
			if((eid = clGetContextInfo(id, CL_CONTEXT_DEVICES, 0,
			        nullptr, &size)) == CL_SUCCESS)
			{
				// !FIXME: assert(size == dids.size())
				devs = devices;
				isCreated = true;
				return true;
			}			
		}
		detail::reportError("Context::create(devices): ", eid);
		return false;
	}

	void Context::release()
	{
		if(isCreated)
		{
			clReleaseContext(id);
			id = 0;
			isCreated = false;
		}
	}

	CommandQueue Context::createCommandQueue(cl_command_queue_properties properties,
	                                              const Device& device)
	{
		cl_command_queue cid = clCreateCommandQueue(id, device.deviceId(), properties, &eid);
		detail::reportError("Context::createCommandQueue(): ", eid);
		return cid ? CommandQueue(this, cid) : CommandQueue();
	}

	Program Context::createProgramFromSourceCode(const string& sourceCode)
	{
		size_t length = sourceCode.length();
		const char* code = sourceCode.c_str();
		cl_program pid = clCreateProgramWithSource(id, 1, &code, &length, &eid);
		detail::reportError("Context::createProgramFromSourceCode(): ", eid);
		return pid ? Program(this, pid) : Program();
	}

	Program Context::createProgramFromSourceFile(const string& fileName)
	{
		string sourceCode;
		if(!detail::readAsString(fileName, &sourceCode))
			return Program();
		return createProgramFromSourceCode(sourceCode);
	}

	Program Context::buildProgramFromSourceCode(const string& sourceCode)
	{
		Program program = createProgramFromSourceCode(sourceCode);
		if(program.isNull() || program.build())
			return program;
		return Program();
	}

	Program Context::buildProgramFromSourceFile(const string& fileName)
	{
		Program program = createProgramFromSourceFile(fileName);
		if(program.isNull() || program.build())
			return program;
		return Program();
	}
}
