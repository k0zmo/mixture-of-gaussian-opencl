#pragma once

#include "Prerequisites.h"
#include "Event.h"

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

		bool readBuffer();
		Event asyncReadBuffer();

		bool writeBuffer();
		Event asyncWriteBuffer();

		bool copyBuffer();
		Event asyncCopyBuffer();

		bool readBufferRect();
		Event asyncReadBufferRect();

		bool writeBufferRect();
		Event asyncWriteBufferRect();

		bool copyBufferRect();
		Event asyncCopyBufferRect();

		// !TODO OpenCL 1.2
		// clEnqueueFillBuffer

		bool readImage();
		Event asyncReadImage();

		bool writeImage();
		Event asyncWriteImage();

		bool copyImage();
		Event asyncCopyImage();

		// !TODO OpenCL 1.2
		// clEnqueueFillImage

		bool copyImageToBuffer();
		Event asyncCopyImageToBuffer();

		bool copyBufferToImage();
		Event asyncCopyBufferToImage();

		void* mapBuffer();
		Event asyncMapBuffer();

		void* mapImage();
		Event asyncMapImage();

		bool unmap(MemoryObject& obj,
			       void* ptr);
		Event asyncUnmap(MemoryObject& obj,
			             void* ptr,
			             const EventList& after = EventList());

		Event asyncRunKernel();
		Event asyncRunTask();
		Event asyncRunNativeKernel();

		// !TODO OpenCL 1.2
		// clEnqueueMigrateMemObjects
		// clEnqueueMarkerWithWaitList

		cl_command_queue commandQueueId() const { return id; }
		Context* context() const { return ctx; }

	private:
		Context* ctx;
		cl_command_queue id;		
	};
}
