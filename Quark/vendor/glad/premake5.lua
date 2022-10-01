project "Glad"
	kind "StaticLib"
	language "C"
	staticruntime "Off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
	    "include/glad/glad.h",
	    "include/KHR/khrplatform.h",
	    "src/glad.c"
	}
	
	includedirs
	{
		"include"
	}
	
	filter "system:windows"
		systemversion "latest"
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		symbols	"On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		symbols	"Off"

	filter { ("configurations:Release" or "configurations:Dist") and "system:windows" }
		flags
		{
			"LinkTimeOptimization"
		}
