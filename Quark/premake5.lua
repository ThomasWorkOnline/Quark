-- Quark Dependencies
group "Dependencies"
	include "vendor/freetype"
	include "vendor/glad"
	include "vendor/glfw"
group ""

project "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"AL_LIBTYPE_STATIC",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",
		"vendor/entt/include",
		"vendor/freetype/include",
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/glm",
		"vendor/openal/include",
		"vendor/spdlog/include",
		"vendor/stb"
	}

	links
	{
		"freetype",
		"Glad",
		"GLFW"
	}

	filter "configurations:Debug"
		defines "QK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "QK_DIST"
		runtime "Release"
		optimize "On"

	filter "system:windows"
		systemversion "latest"

		libdirs
		{
			"vendor/openal/libs/Win64"
		}

		links
		{
			"OpenAL32.lib",
			"opengl32.lib"
		}
	
	filter "system:macosx"
		links
		{
			"Cocoa.framework",
			"CoreVideo.framework",
			"IOKit.framework",
			"OpenAL.framework",
			"OpenGL.framework"
		}

	filter "system:linux"
		links
		{
			"GL",
			"X11",
			"openal"
		}
