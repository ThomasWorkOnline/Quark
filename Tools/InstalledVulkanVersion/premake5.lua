project "InstalledVulkanVersion"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Quark/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"Quark"
	}

	flags
	{
		"FatalCompileWarnings",
		"ShadowedVariables"
	}

	filter { "configurations:Dist", "system:windows" }
		kind "WindowedApp"

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
		includedirs
		{
			"/Users/thomaslessard/VulkanSDK/1.3.224.1/MoltenVK/include"
		}

		links
		{
			"Quark",
			"spdlog",

			"vulkan",
			"MoltenVK"
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
