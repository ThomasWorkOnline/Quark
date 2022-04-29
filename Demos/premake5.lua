project "Demos"
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
		"%{wks.location}/Quark/src",
		"%{wks.location}/Quark/vendor/glm",
		"%{wks.location}/Quark/vendor/entt/single_include",
		"%{wks.location}/Quark/vendor/spdlog/include"
	}

	links
	{
		"Quark",
		"freetype",
		"Glad",
		"GLFW",
		"spdlog"
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

	filter { "configurations:Debug or configurations:Release", "system:windows" }
		kind "ConsoleApp"

	filter { "configurations:Dist", "system:windows" }
		kind "WindowedApp"

	filter "system:macosx"
		kind "WindowedApp"
		
		links
		{
			"Cocoa.framework",
			"CoreVideo.framework",
			"IOKit.framework",
			"OpenAL.framework",
			"OpenGL.framework"
		}
	
	filter "system:linux"
		kind "ConsoleApp"

		links
		{
			"GL",
			"X11",
			"openal"
		}
		