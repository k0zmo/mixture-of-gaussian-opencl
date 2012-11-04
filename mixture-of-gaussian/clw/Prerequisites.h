#pragma once

#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>

#include <cstdint>
#include <string>
#include <vector>

#ifdef _MSC_VER
// nonstandard extension used: enum used in qualified name
#pragma warning(disable:4482)
#endif

#define HAVE_OPENCL_1_0
#define HAVE_OPENCL_1_1
//#define HAVE_OPENCL_1_2

namespace clw
{
	using std::vector;
	using std::string;

	class Platform;
	class Device;
	class Context;
	class CommandQueue;
	class Program;
	class Kernel;
	class MemoryObject;
	class Buffer;
	struct ImageFormat;
	class Image2D;
	class Image3D;
	class NDRange;
	class Event;
	class EventList;
	class Sampler;

	namespace detail
	{
		void reportError(const char* name, cl_int eid);
		bool supportsExtension(const string& list, const char* ext);
		vector<string> tokenize(const string& str, char delim, char group = 0);
		void trim(string* str, bool left, bool right);
		bool readAsString(const string& filename, string* contents);
	}
}
