#include "CommandQueue.h"
#include "Buffer.h"
#include "Image.h"
#include "Kernel.h"
#include "NDRange.h"

namespace clw
{
	namespace detail
	{
		bool commandQueueInfo(cl_command_queue id, cl_ulong prop)
		{
			cl_command_queue_properties props;
			cl_int error = CL_SUCCESS;
			if(!id || (error = clGetCommandQueueInfo(id, CL_QUEUE_PROPERTIES, 
			        sizeof(cl_command_queue_properties), &props, nullptr))
					    != CL_SUCCESS)
			{
				detail::reportError("commandQueueInfo(): ", error);
				return false;
			}
			return (props & prop) != 0;
		}
	}

	CommandQueue::~CommandQueue()
	{
		if(id)
			clReleaseCommandQueue(id);
	}

	CommandQueue::CommandQueue(const CommandQueue& other)
		: ctx(other.ctx), id(other.id)
	{
		if(id)
			clRetainCommandQueue(id);
	}

	CommandQueue& CommandQueue::operator=(const CommandQueue& other)
	{
		ctx = other.ctx;
		if(other.id)
			clRetainCommandQueue(other.id);
		if(id)
			clReleaseCommandQueue(id);
		id = other.id;
		return *this;
	}

	bool CommandQueue::isProfilingEnabled() const
	{
		return detail::commandQueueInfo(id, CL_QUEUE_PROFILING_ENABLE);
	}

	bool CommandQueue::isOutOfOrder() const
	{
		return detail::commandQueueInfo(id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
	}

	void CommandQueue::finish()
	{
		cl_int err = clFinish(id);
		detail::reportError("CommandQueue::finish(): ", err);
	}

	void CommandQueue::flush()
	{
		cl_int err = clFlush(id);
		detail::reportError("CommandQueue::flush(): ", err);
	}

	bool CommandQueue::readBuffer(const Buffer& buffer,
		                          size_t offset,
		                          void* data,
		                          size_t size)
	{
		cl_int error;
		if((error = clEnqueueReadBuffer(id, buffer.memoryId(), 
		        CL_TRUE, offset, size, data,
		        0, nullptr, nullptr)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::readBuffer() ", error);
			return false;
		}
		return true;
	}

	Event CommandQueue::asyncReadBuffer(const Buffer& buffer,
	                                    size_t offset,
	                                    void* data,
	                                    size_t size,
	                                    const EventList& after)
	{
		cl_event event;
		cl_int error;
		if((error = clEnqueueReadBuffer(id, buffer.memoryId(), 
		        CL_FALSE, offset, size, data,
		        after.size(), after, &event)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::asyncReadBuffer() ", error);
			return Event(event);
		}
		return Event(event);
	}

	bool CommandQueue::writeBuffer(Buffer& buffer,
	                               size_t offset, 
	                               const void* data, 
	                               size_t size)
	{
		cl_int error;
		if((error = clEnqueueWriteBuffer(id, buffer.memoryId(), 
		        CL_TRUE, offset, size, data,
		        0, nullptr, nullptr)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::writeBuffer() ", error);
			return false;
		}
		return true;
	}

	Event CommandQueue::asyncWriteBuffer(Buffer& buffer,
	                                    size_t offset, 
	                                    const void* data, 
	                                    size_t size,
	                                    const EventList& after)
	{
		cl_event event;
		cl_int error;
		if((error = clEnqueueWriteBuffer(id, buffer.memoryId(), 
		        CL_FALSE, offset, size, data,
		        after.size(), after, &event)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::asyncWriteBuffer() ", error);
			return Event(event);
		}
		return Event(event);
	}

	void* CommandQueue::mapBuffer(Buffer& buffer, 
		                          size_t offset, 
		                          size_t size,
		                          EAccess access)
	{
		cl_int error;
		void* data = clEnqueueMapBuffer(id, buffer.memoryId(), CL_TRUE,
			access, offset, size, 0, nullptr, nullptr, &error);
		detail::reportError("CommandQueue::mapBuffer() ", error);
		return data;
	}

	void* CommandQueue::mapBuffer(Buffer& buffer, EAccess access)
	{
		return mapBuffer(buffer, 0, buffer.size(), access);
	}

	Event CommandQueue::asyncMapBuffer(Buffer& buffer, 
	                                   void** data,
	                                   size_t offset, 
	                                   size_t size, 
	                                   EAccess access, 
	                                   const EventList& after)
	{
		cl_int error;
		cl_event event;
		*data = clEnqueueMapBuffer(id, buffer.memoryId(), CL_FALSE,
			access, offset, size, after.size(), after, &event, &error);
		if(error != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::asyncMapBuffer() ", error);
			return Event();
		}
		return Event();
	}

	bool CommandQueue::unmap(MemoryObject& obj, void* ptr)
	{
		cl_event event;
		cl_int error;
		if((error = clEnqueueUnmapMemObject(id, obj.memoryId(),
		        ptr, 0, nullptr, &event)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue()::unmap() ", error);
			return false;
		}
		else
		{
			clWaitForEvents(1, &event);
			clReleaseEvent(event);
			return true;
		}
	}

	Event CommandQueue::asyncUnmap(MemoryObject& obj,
	                               void* ptr,
	                               const EventList& after)
	{
		cl_event event;
		cl_int error;
		if((error = clEnqueueUnmapMemObject(id, obj.memoryId(),
			ptr, after.size(), after, &event)) != CL_SUCCESS)
		{
			detail::reportError("CommandQueue()::asyncUnmap() ", error);
			return Event();
		}
		else
		{
			return Event(event);
		}
	}

	bool CommandQueue::runKernel(const Kernel& kernel)
	{
		NDRange& offset = kernel.globalWorkOffset();
		NDRange& global = kernel.globalWorkSize();		
		NDRange& local = kernel.localWorkSize();
		size_t dims = global.dimensions();
		const size_t* l = local;
		if(local.width() == 0)
			l = nullptr;

		cl_event event;
		cl_int error = clEnqueueNDRangeKernel
			(id, kernel.kernelId(), 
			dims, offset, global, l,
			0, nullptr, &event);
		if(error != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::runKernel() ", error);
			return false;
		}
		else
		{
			clWaitForEvents(1, &event);
			clReleaseEvent(event);
			return true;
		}
	}

	Event CommandQueue::asyncRunKernel(
		const Kernel& kernel,
		const EventList& after)
	{
		NDRange& offset = kernel.globalWorkOffset();
		NDRange& global = kernel.globalWorkSize();		
		NDRange& local = kernel.localWorkSize();
		size_t dims = global.dimensions();
		const size_t* l = local;
		if(local.width() == 0)
			l = nullptr;

		cl_event event;
		cl_int error = clEnqueueNDRangeKernel
			(id, kernel.kernelId(), 
			 dims, offset, global, l,
			 after.size(), after, &event);
		if(error != CL_SUCCESS)
		{
			detail::reportError("CommandQueue::asyncRunKernel() ", error);
			return Event();
		}
		return Event(event);
	}
}
