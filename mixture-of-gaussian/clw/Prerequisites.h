#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>

// nonstandard extension used: enum used in qualified name
#pragma warning(disable:4482)

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
	class Event;
}