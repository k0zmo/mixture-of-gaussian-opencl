#include "Platform.h"

namespace clw
{
	namespace detail
	{
		vector<cl_platform_id> getPlatformIDs()
		{
			cl_uint num;
			if(clGetPlatformIDs(0, nullptr, &num) != CL_SUCCESS)
				return vector<cl_platform_id>();
			vector<cl_platform_id> pids(num);
			clGetPlatformIDs(num, pids.data(), nullptr);
			return pids;
		}

		string platformInfo(cl_platform_id id, cl_platform_info name)
		{
			size_t size;
			if(!id || clGetPlatformInfo(id, name, 0, nullptr, &size) != CL_SUCCESS)
				return string();
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
			return EPlatformVersion::Version_Undefined;

		ver = ver.substr(prefix.length());
		string::size_type pos = 0;
		while(ver[pos] != '.' && pos < ver.length())
			++pos;
		if(pos == ver.length())
			return EPlatformVersion::Version_Undefined;

		int major = std::stoi(ver.substr(0, pos));
		string::size_type mpos = pos + 1;

		while(ver[mpos] != ' ' && mpos < ver.length())
			++mpos;
		if(mpos == ver.length())
			return EPlatformVersion::Version_Undefined;

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
			return EPlatformVersion::Version_Undefined;
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
		vector<cl_platform_id> pids = detail::getPlatformIDs();
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