#include "Device.h"
#include "Platform.h"
#include "StlUtils.h"

namespace clw 
{
	namespace detail
	{
		template<typename Value>
		Value deviceInfo(cl_device_id id, cl_device_info info)
		{
			Value value;
			cl_int error;
			if(!id || (error = clGetDeviceInfo(id, info, 
				    sizeof(Value), &value, nullptr)) != CL_SUCCESS)
			{
				reportError("deviceInfo(): ", error);
				return Value(0);
			}
			return value;
		}

		template<>
		bool deviceInfo(cl_device_id id, cl_device_info info)
		{
			cl_bool value = deviceInfo<cl_bool>(id, info);
			return value != 0;
		}

		template<>
		string deviceInfo(cl_device_id id, cl_device_info info)
		{
			size_t size;
			cl_int error;
			if(!id || (error = clGetDeviceInfo(id, info, 0, 0, &size))
			        != CL_SUCCESS)
			{
				reportError("deviceInfo(): ", error);
				return string();
			}
			vector<char> buf(size);
			clGetDeviceInfo(id, info, size, buf.data(), &size);
			return string(buf.data());
		}

		bool supportsExtension(const string& list, const char* ext)
		{
			const char* it = list.data();
			size_t listLen = list.length();
			size_t extLen = strlen(ext);
			while(listLen > 0)
			{
				if(*it == '\0')
				{
					break;
				}
				else if(*it == ' ')
				{
					++it;
					--listLen;
					continue;
				}

				size_t tempLen = 0;
				while(tempLen < listLen &&
						it[tempLen] != ' ' && it[tempLen] != '\0')
					++tempLen;
				if(extLen == tempLen && !strncmp(ext, it, tempLen))
					return true;
				it += tempLen;
				listLen -= tempLen;
			}

			return false;
		}

		vector<string> tokenize(const string& str, char delim, char group = 0)
		{
			vector<string> tokens;
			if(str.empty())
				return tokens;

			int curr = 0;
			int start = 0;

			// upewnij sie czy string nie rozpoczyna sie od delimitersow
			start = curr = static_cast<int>(str.find_first_not_of(delim));

			while(str[curr])
			{
				if(str[curr] == group)
				{
					curr = static_cast<int>(str.find_first_of(group, curr+1));
					if((size_t)curr == string::npos)
						return vector<string>();

					string token = str.substr(start+1, curr-start-1);
					tokens.push_back(token);
					start = curr + 1;
				}
				else if(str[curr] == delim)
				{
					if(str[curr-1] != delim && str[curr-1] != group)
					{
						string token = str.substr(start, curr-start);
						tokens.push_back(token);
					}
					start = curr + 1;
				}
				++curr;
			}

			if(tokens.size() == 0)
			{
				tokens.push_back(str);
				return tokens;
			}

			// dla ostatniego token'a
			if(str[curr-1] != delim && str[curr-1] != group)
			{
				string token = str.substr(start, curr - 1);
				tokens.push_back(token);
			}

			return tokens;
		}

		void trim(string* str, bool left, bool right)
		{
			if(left)
			{
				size_t pos = str->find_first_not_of(" \t\r");
				if(pos != 0 && pos != string::npos)
				{
					*str = str->substr(pos);
				}
			}

			if(right)
			{
				size_t pos = str->find_last_not_of(" \t\r");
				if(pos < str->length()-1)
				{
					*str = str->substr(0, pos+1);
				}
			}
		}
	}

	EDeviceType Device::deviceType() const
	{
		return EDeviceType(detail::deviceInfo<cl_device_type>
			(id, CL_DEVICE_TYPE));
	}

	Platform Device::platform() const
	{
		return Platform(detail::deviceInfo<cl_platform_id>
			(id, CL_DEVICE_PLATFORM));
	}

	bool Device::supportsExtension(const char* ext) const
	{
		string list = detail::deviceInfo<string>(id, CL_DEVICE_EXTENSIONS);
		return detail::supportsExtension(list, ext);
	}

	bool Device::supportsDouble() const
	{
		return supportsExtension("cl_khr_fp64");
	}

	bool Device::supportsHalf() const
	{
		return supportsExtension("cl_khr_fp16");
	}

	string Device::version() const
	{
		return detail::deviceInfo<string>(id, CL_DEVICE_VERSION);
	}

	string Device::name() const
	{
		string devname = detail::deviceInfo<string>(id, CL_DEVICE_NAME);
		detail::trim(&devname, true, true);
		return devname;
	}

	string Device::vendor() const
	{
		return detail::deviceInfo<string>(id, CL_DEVICE_VENDOR);
	}

	string Device::driverVersion() const
	{
		return detail::deviceInfo<string>(id, CL_DRIVER_VERSION);
	}

	string Device::languageVersion() const
	{
		return detail::deviceInfo<string>(id, CL_DEVICE_OPENCL_C_VERSION);
	}

	vector<string> Device::extensions() const
	{
		string list = detail::deviceInfo<string>(id, CL_DEVICE_EXTENSIONS);
		return detail::tokenize(list, ' ');
	}

	vector<Device> allDevices()
	{
		vector<Platform> platforms = availablePlatforms();
		vector<Device> devices;
		clw::for_each(platforms, [&devices](const Platform& platform)
		{
			size_t size;
			cl_int error;
			if((error = clGetDeviceIDs(platform.platformId(), CL_DEVICE_TYPE_ALL,
					0, nullptr, &size)) != CL_SUCCESS)
			{
				detail::reportError("allDevices(): ", error);
				return; // works like continue in ordinary loop
			}
			vector<cl_device_id> buf(size);
			clGetDeviceIDs(platform.platformId(), CL_DEVICE_TYPE_ALL,
				size, buf.data(), &size);
			for(size_t i = 0; i < buf.size(); ++i)
				devices.push_back(Device(buf[i]));
		});
		return devices;
	}

	vector<Device> devices(EDeviceType deviceTypes, const Platform& platform)
	{
		size_t size;
		cl_int error;
		if(platform.isNull() || 
			    (error = clGetDeviceIDs(platform.platformId(), deviceTypes, 
				               0, nullptr, &size)) != CL_SUCCESS)
		{
			detail::reportError("devices(): ", error);
			return vector<Device>();
		}
		vector<cl_device_id> buf(size);
		clGetDeviceIDs(platform.platformId(), deviceTypes,
		               size, buf.data(), &size);
		vector<Device> devs(size);
		for(size_t i = 0; i < buf.size(); ++i)
			devs[i] = Device(buf[i]);
		return devs;
	}
}
