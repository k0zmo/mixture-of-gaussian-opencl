newoption {
	trigger     = "openclincdir",
	value       = "PATH",
	description = "Set path to a directory that contains CL/cl.h"
}

newoption {
	trigger     = "opencllibdir",
	value       = "PATH",
	description = "Set path to a directory that contains OpenCL.lib or libOpenCL.so"
}

newoption {
	trigger     = "opencvincdir",
	value       = "PATH",
	description = "Set path to a directory that contains opencv2 header files"
}

newoption {
	trigger     = "opencvlibdir",
	value       = "PATH",
	description = "Set path to a directory that contains opencv2 dynamic libraries"
}


if os.get() == "linux" then 
	_ACTION = _ACTION or "gmake"
	_OPTIONS["openclincdir"] = _OPTIONS["openclincdir"] or "/usr/include"
	_OPTIONS["opencllibdir"] = _OPTIONS["opencllibdir"] or "/usr/lib"
	_OPTIONS["opencvincdir"] = _OPTIONS["opencvincdir"] or "/usr/include"
	_OPTIONS["opencvlibdir"] = _OPTIONS["opencvlibdir"] or "/usr/lib"
else
	print("premake4 file not yet supported for your OS, quitting")
	return
end

solution "mixture-of-gaussian"
	configurations { "Debug", "Release" }
	project "mixture-of-gaussian"
		language "C++"
	    location "proj"
		kind "ConsoleApp"
		includedirs { "clw", _OPTIONS["openclincdir"], _OPTIONS["opencvincdir"] }
		libdirs { _OPTIONS["opencllibdir"], _OPTIONS["opencvlibdir"] }
		defines "CL_USE_DEPRECATED_OPENCL_1_1_APIS"	
		objdir "obj"
		targetdir "."
		
		files {
			"clw/clw/*.cpp", "clw/clw/*.h",
			"main.cpp", 
			"MixtureOfGaussianCPU.*",
			"MixtureOfGaussianGPU.*",
			"GrayscaleGPU.*",
			"BayerFilterGPU.*",
			"FrameGrabber.*",
			"QPCTimer.*",
			"ConfigFile.*",
			"WorkerCPU.*",
			"WorkerGPU.*"
		}
			
		links {
			"opencv_core", "opencv_imgproc", "opencv_highgui", "opencv_video", 
			"OpenCL"
		}

	configuration "Debug"
		targetsuffix "_d"
		defines { "DEBUG", "_DEBUG", }
		flags { "Symbols", "ExtraWarnings" }
	
	configuration "Release"
		defines "NDEBUG"	
		flags { "OptimizeSpeed", "NoEditAndContinue", "NoFramePointer", "ExtraWarnings" }
		
	configuration { "linux", "gmake" }
		buildoptions { "-std=c++11", "-fPIC" }
