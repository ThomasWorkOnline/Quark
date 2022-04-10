include "vendor/glad"
include "vendor/glfw"

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
		"vendor/freetype/include",
		"vendor/entt/include",
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/glm",
		"vendor/openal/include",
		"vendor/spdlog/include",
		"vendor/stb_image"
	}

	links
	{
		"freetype",
		"Glad",
		"GLFW"
	}

	filter "system:windows"
		systemversion "latest"

		libdirs
		{
			"vendor/openal/libs/Win64",
			"vendor/freetype/vs2015-2019/win64"
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
			"IOKit.framework",
			"OpenAL.framework",
			"OpenGL.framework"
		}
