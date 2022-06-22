project "Demos"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
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
		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",

		"%{IncludeDir.glfw}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"Quark",
		"freetype",
		"Glad",
		"GLFW",
		"lodepng",
		"spdlog",

		"%{Library.Vulkan}"
	}

	filter { "configurations:Dist", "system:windows" }
		kind "WindowedApp"

	filter "system:macosx"
		kind "WindowedApp"
		
		links
		{
			"Cocoa.framework",
			"CoreVideo.framework",
			"Foundation.framework",
			"IOKit.framework",
			"Metal.framework",
			"OpenAL.framework",
			"OpenGL.framework",
			"QuartzCore.framework"
		}
	
	filter "system:linux"
		links
		{
			"GL",
			"X11",
			"openal"
		}

	filter "configurations:Debug"
		defines "QK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		runtime "Release"
		optimize "On"

		flags
		{
			"LinkTimeOptimization"
		}

	filter "configurations:Dist"
		defines "QK_DIST"
		runtime "Release"
		optimize "On"

		flags
		{
			"LinkTimeOptimization"
		}