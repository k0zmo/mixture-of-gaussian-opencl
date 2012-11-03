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
    	-- includedirs "include"
    	
	    configuration "Debug"
    		targetdir "../Debug"
    	configuration "Release"
    		targetdir "../Release"
    	configuration {}
		
		configuration { "linux", "gmake" }
			buildoptions { "-std=c++0x", "-fPIC" }
	
    	objdir "obj"
    	
    	files {
    		"clw/*.cpp", "clw/*.h",
    		"main.cpp", "MixtureOfGaussianCPU.*", "QPCTimer.*",
    	}
			
		links {
			"opencv_core", "opencv_imgproc", "opencv_highgui",
			"OpenCL"
		}
