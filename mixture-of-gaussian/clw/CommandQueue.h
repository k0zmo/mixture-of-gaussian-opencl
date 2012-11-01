#pragma once

#include "Prerequisites.h"

namespace clw
{
	class CommandQueue
	{
	public:
		CommandQueue() : id(0), ctx(nullptr) {}
		CommandQueue(Context* ctx, cl_command_queue id)
			: ctx(ctx), id(id) {}
		~CommandQueue();

		CommandQueue(const CommandQueue& other);		
		CommandQueue& operator=(const CommandQueue& other);

		bool isNull() const { return id == 0; }
		bool isProfilingEnabled() const;
		bool isOutOfOrder() const;

		cl_command_queue commandQueueId() const { return id; }
		Context* context() const { return ctx; }

	private:
		Context* ctx;
		cl_command_queue id;		
	};
}