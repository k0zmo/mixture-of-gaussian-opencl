#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>

#include <string>
#include <map>
#include <unordered_map>

// nonstandard extension used: enum used in qualified name
#pragma warning(disable:4482)

namespace clw
{
	using std::vector;
	using std::string;
	using std::unordered_map;
	
	class Platform;
	class Device;
	class Context;
	class CommandQueue;
	//class Program;
	//class Kernel;
	//class Event;
	//class Buffer;
	//class Image2D;
	//class Image3D;
	//class Sampler

	namespace detail
	{
		void reportError(const char* name, cl_int eid);
	}
}