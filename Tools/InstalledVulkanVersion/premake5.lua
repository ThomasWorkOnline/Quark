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

	links
	{
		"%{Library.Vulkan}"
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
