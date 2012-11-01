#include "CommandQueue.h"

namespace clw
{
	namespace detail
	{
		bool commandQueueInfo(cl_command_queue id, cl_ulong prop)
		{
			cl_command_queue_properties props;
			cl_int error;
			if(!id || (error = clGetCommandQueueInfo(id, CL_QUEUE_PROPERTIES, 
			        sizeof(cl_command_queue_properties), &props, nullptr))
					    != CL_SUCCESS)
			{
				detail::reportError("commandQueueInfo: ", error);
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
		detail::reportError("CommandQueue::finish: ", err);
	}

	void CommandQueue::flush()
	{
		cl_int err = clFlush(id);
		detail::reportError("CommandQueue::flush: ", err);
	}
}