#pragma once

#include "Prerequisites.h"
#include "Device.h"
#include "Sampler.h"

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

		Sampler createSampler(bool normalizedCoords, 
		                      EAddressingMode addressingMode, 
		                      EFilterMode filterMode);

		vector<ImageFormat> supportedImage2DFormats() const;
		vector<ImageFormat> supportedImage3DFormats() const;

		size_t numDevices() const { return devs.size(); }
		const vector<Device>& devices() const { return devs; }
		cl_context contextId() const { return id; }

		Program createProgramFromSourceCode(const string& sourceCode);
		Program createProgramFromSourceFile(const string& fileName);

		Program buildProgramFromSourceCode(const string& sourceCode);
		Program buildProgramFromSourceFile(const string& fileName);

	private:
		cl_context id;
		bool isCreated;
		cl_int eid;
		vector<Device> devs;
	};
}