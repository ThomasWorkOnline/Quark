project "InstalledVulkanVersion"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++14"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/Main.cpp"
	}

	includedirs
	{
		"%{IncludeDir.VulkanSDK}"
	}

	flags
	{
		"FatalCompileWarnings",
		"ShadowedVariables"
	}

	filter "system:windows"
		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"%{Library.Vulkan}"
		}

	filter "system:macosx"
		links
		{
			"Vulkan.framework"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		symbols	"On"

		flags
		{
			"LinkTimeOptimization"
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		symbols "Off"

		flags
		{
			"LinkTimeOptimization"
		}
