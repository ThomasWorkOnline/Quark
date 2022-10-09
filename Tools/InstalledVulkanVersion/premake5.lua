project "InstalledVulkanVersion"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++14"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	removefiles
	{
		"src/Platform/**.*"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.VulkanSDK}"
	}

	flags
	{
		"FatalCompileWarnings",
		"ShadowedVariables"
	}

	filter "system:windows"
		files
		{
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp"
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"%{Library.Vulkan}"
		}

	filter "system:macosx"
		files
		{
			"src/Platform/Unix/**.h",
			"src/Platform/Unix/**.cpp"
		}
		
		includedirs
		{
			"/Users/thomaslessard/VulkanSDK/1.3.224.1/MoltenVK/include"
		}

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
		symbols "Off"

	filter { "configurations:Release", "system:windows" }
		flags
		{
			"LinkTimeOptimization"
		}

	filter { "configurations:Dist", "system:windows" }
		flags
		{
			"LinkTimeOptimization"
		}
