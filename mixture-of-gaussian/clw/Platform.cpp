#include "Platform.h"

namespace clw
{
	namespace detail
	{
		vector<cl_platform_id> platformIDs()
		{
			cl_uint num;
			cl_int error;
			if((error = clGetPlatformIDs(0, nullptr, &num)) != CL_SUCCESS)
			{
				reportError("platformIDs(): ", error);
				return vector<cl_platform_id>();
			}
			vector<cl_platform_id> pids(num);
			clGetPlatformIDs(num, pids.data(), nullptr);
			return pids;
		}

		string platformInfo(cl_platform_id id, cl_platform_info name)
		{
			size_t size;
			cl_int error;
			if(!id || (error = clGetPlatformInfo(id, name,
			        0, nullptr, &size)) != CL_SUCCESS)
			{
				reportError("platfromInfo(): ", error);
				return string();
			}
			vector<char> infoBuf(size);
			clGetPlatformInfo(id, name, size, infoBuf.data(), &size);
			return string(infoBuf.data());
		}
	}

	string Platform::versionString() const
	{
		return detail::platformInfo(id, CL_PLATFORM_VERSION);
	}

	EPlatformVersion Platform::version() const
	{
		// Format returned by clGetPlatformInfo with CL_PLATFORM_VERSION is: 
		// OpenCL<space><major_version.minor_version><space><platform-specific information>

		static const string prefix("OpenCL ");

		string ver = detail::platformInfo(id, CL_PLATFORM_VERSION);
		if(!prefix.compare(0, prefix.length(), ver))
			return EPlatformVersion(0);

		ver = ver.substr(prefix.length());
		string::size_type pos = 0;
		while(ver[pos] != '.' && pos < ver.length())
			++pos;
		if(pos == ver.length())
			return EPlatformVersion(0);

		int major = std::stoi(ver.substr(0, pos));
		string::size_type mpos = pos + 1;

		while(ver[mpos] != ' ' && mpos < ver.length())
			++mpos;
		if(mpos == ver.length())
			return EPlatformVersion(0);

		int minor = std::stoi(ver.substr(pos+1, mpos-(pos+1)));

		switch(major)
		{
		case 1:
			switch(minor)
			{
			case 0: return EPlatformVersion::Version_1_0;
			case 1: return EPlatformVersion::Version_1_1;
			case 2: return EPlatformVersion::Version_1_2;
			}
		default:
			return EPlatformVersion(0);
		}
	}

	string Platform::name() const
	{
		return detail::platformInfo(id, CL_PLATFORM_NAME);
	}

	string Platform::vendor() const
	{
		return detail::platformInfo(id, CL_PLATFORM_VENDOR);
	}

	string Platform::extensionSuffix() const
	{
		return detail::platformInfo(id, CL_PLATFORM_ICD_SUFFIX_KHR);
	}

	vector<Platform> availablePlatforms()
	{
		vector<cl_platform_id> pids = detail::platformIDs();
		if(pids.empty())
			return vector<Platform>();
		vector<Platform> platforms(pids.size());
		for(size_t i = 0; i < pids.size(); ++i)
			platforms[i] = Platform(pids[i]);
		return platforms;
	}

	Platform defaultPlatform()
	{
		cl_platform_id pid;
		if(clGetPlatformIDs(1, &pid, nullptr) != CL_SUCCESS)
			return Platform();
		return Platform(pid);
	}
}