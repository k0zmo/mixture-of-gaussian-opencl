#include "CommandQueue.h"
#include "MemoryObject.h"
#include "Event.h"

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

	bool CommandQueue::unmap(MemoryObject& obj, void* ptr)
	{
		cl_event event;
		cl_int error;
		if((error = clEnqueueUnmapMemObject(id, obj.memoryId(),
		        ptr, 0, 0, &event)) != CL_SUCCESS)
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
			detail::reportError("CommandQueue()::unmap() ", error);
			return Event();
		}
		else
		{
			Event(event);
		}
	}
}
