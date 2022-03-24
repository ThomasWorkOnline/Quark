project "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",

		"vendor/glad/include/**.h",
		"vendor/glad/src/**.c"
	}

	defines {
		"AL_LIBTYPE_STATIC",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {
		"src",
		"vendor/freetype/include",
		"vendor/entt/include",
		"vendor/glad/include",
		"vendor/glm",
		"vendor/openal/include",
		"vendor/stb_image"
	}

	links {
		"freetype",
		"glfw3"
	}

	filter "system:windows"
		systemversion "latest"

		includedirs {
			"vendor/openal/libs/Win64",
			"vendor/glfw-prebuilt-win/include"
		}

		libdirs {
			"vendor/freetype/vs2015-2019/win64",
			"vendor/glfw-prebuilt-win/lib-vc2022"
		}

		links {
			"OpenAL32.lib",
			"opengl32.lib"
		}
	
	filter "system:macosx"

		includedirs {
			"vendor/glfw-prebuilt-macos/include"
		}

		libdirs {
			"vendor/glfw-prebuilt-macos/lib-x86_64"
		}

		links {
			"Cocoa.framework",
			"IOKit.framework",
			"OpenAL.framework",
			"OpenGL.framework"
		}
