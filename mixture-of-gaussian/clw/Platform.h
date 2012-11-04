#pragma once

#include "Prerequisites.h"

namespace clw
{
	enum EPlatformVersion
	{
		Version_1_0 = 1,
		Version_1_1 = 2,
		Version_1_2 = 3
	};

	// Thin wrapper over OpenCL platform 
	class Platform
	{
	public:
		Platform() : id(0) {}
		Platform(cl_platform_id id) : id(id) {}

		bool isNull() const { return id == 0; }
		EPlatformVersion version() const;
		string versionString() const;
		string name() const;
		string vendor() const;
		string extensionSuffix() const;
		vector<string> extensions() const;

		cl_platform_id platformId() const { return id; }

		// !TODO: unloadCompiler(); - only for 1.2

	private:
		cl_platform_id id;
	};

	vector<Platform> availablePlatforms();
	Platform defaultPlatform();
}