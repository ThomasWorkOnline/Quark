project "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "qkpch.h"
	pchsource "src/qkpch.cpp"

	files
	{
		"src/Quark/**.h",
		"src/Quark/**.cpp",
		"src/Quark.h",
		"src/qkpch.h",
		"src/qkpch.cpp",
		"src/Quark/EntryPoint.h",
		"src/Quark/EntryPoint.cpp",
		"src/Platform/GLFW/**.h",
		"src/Platform/GLFW/**.cpp",
		"src/Platform/OpenAL/**.h",
		"src/Platform/OpenAL/**.cpp",
		"src/Platform/OpenGL/**.h",
		"src/Platform/OpenGL/**.cpp",
		"src/Platform/Vulkan/**.h",
		"src/Platform/Vulkan/**.cpp",

		"assets/shaders/**.vert",
		"assets/shaders/**.frag"
	}

	defines
	{
		"AL_LIBTYPE_STATIC",
		"GLFW_INCLUDE_NONE",
		"GLM_FORCE_LEFT_HANDED"
	}

	includedirs
	{
		"src",
		"%{VendorDir}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.OpenAL}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"freetype",
		"Glad",
		"GLFW",
		"lodepng",
		"spdlog",
		"%{Library.Vulkan}"
	}

	flags
	{
		"FatalCompileWarnings",
		"ShadowedVariables"
	}

	filter "system:windows"
		systemversion "latest"

		files
		{
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp"
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

		libdirs
		{
			"%{LibraryDir.OpenAL}"
		}

		links
		{
			"%{Library.OpenAL}",
			"%{Library.OpenGL}"
		}

		postbuildcommands
		{
			"%{wks.location}/Scripts/Windows/CompileShaders.bat"
		}

	filter "system:macosx"
		files
		{
			"src/Platform/MacOS/**.h",
			"src/Platform/MacOS/**.cpp"
		}

		includedirs
		{
			"%{IncludeDir.Metal}"
		}

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
		files
		{
			"src/Platform/Linux/**.h",
			"src/Platform/Linux/**.cpp"
		}

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

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRVCross_Debug}",
			"%{Library.SPIRVCross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "QK_RELEASE"
		runtime "Release"
		optimize "On"
		symbols	"On"

		flags
		{
			"LinkTimeOptimization"
		}

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRVCross_Release}",
			"%{Library.SPIRVCross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "QK_DIST"
		runtime "Release"
		optimize "Full"
		symbols "Off"

		flags
		{
			"LinkTimeOptimization"
		}

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRVCross_Release}",
			"%{Library.SPIRVCross_GLSL_Release}"
		}
