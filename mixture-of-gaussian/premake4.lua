if os.get() == "windows" then 
	_ACTION = _ACTION or "vs2010"
elseif os.get() == "linux"then
	_ACTION = _ACTION or "gmake"
end

solution "mixture-of-gaussian"
	configurations { "Debug", "Release" }
    project "mixture-of-gaussian"
    	language "C++"
	    location "proj"
    	kind "ConsoleApp"
    	includedirs "clw"
		defines "CL_USE_DEPRECATED_OPENCL_1_1_APIS"	
    	objdir "obj"
		targetdir "."
    	
    	files {
    		"clw/clw/*.cpp", "clw/clw/*.h",
    		"main.cpp", "MixtureOfGaussianCPU.*", "QPCTimer.*",
    	}
			
		links {
			"opencv_core", "opencv_imgproc", "opencv_highgui",
			"OpenCL"
		}

	configuration "Debug"
		targetsuffix "_d"
		defines { "DEBUG", "_DEBUG", }
		flags "Symbols"
	
	configuration "Release"
		defines "NDEBUG"	
		flags { "OptimizeSpeed", "NoEditAndContinue", "NoFramePointer" }
		
	configuration { "linux", "gmake" }
		buildoptions { "-std=c++0x", "-fPIC" }