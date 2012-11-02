#pragma once

#include "Prerequisites.h"

namespace clw
{
	enum ECommandQueueProperty
	{
		Property_OutOfOrderExecModeEnabled   = (1 << 0),
		Property_ProfilingEnabled            = (1 << 1)
	};

	class CommandQueue
	{
	public:
		CommandQueue() : ctx(nullptr), id(0) {}
		CommandQueue(Context* ctx, cl_command_queue id)
			: ctx(ctx), id(id) {}
		~CommandQueue();

		CommandQueue(const CommandQueue& other);		
		CommandQueue& operator=(const CommandQueue& other);

		bool isNull() const { return id == 0; }
		bool isProfilingEnabled() const;
		bool isOutOfOrder() const;

		void finish();
		void flush();

		cl_command_queue commandQueueId() const { return id; }
		Context* context() const { return ctx; }

	private:
		Context* ctx;
		cl_command_queue id;		
	};
}
