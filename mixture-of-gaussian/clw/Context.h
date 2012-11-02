#pragma once

#include "Prerequisites.h"
#include "Device.h"

namespace clw
{
	class Context
	{
	public:
		Context();
		~Context();

		bool create(EDeviceType type = EDeviceType::Default);
		bool create(const vector<Device>& devices);
		void release();

		cl_int lastError() const { return eid; }
		void setLastError(cl_int error) { eid = error; }

		CommandQueue createCommandQueue(cl_command_queue_properties properties,
		                                const Device& device);

		const vector<Device>& devices() const { return devs; }
		cl_context contextId() const { return id; }
	private:
		cl_context id;
		bool isCreated;
		cl_int eid;
		vector<Device> devs;
	};
}