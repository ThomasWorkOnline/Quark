project "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	defines {
		"AL_LIBTYPE_STATIC",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {
		"src",
		"vendor/freetype/include",
		"vendor/entt/include",
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

		files {
			"vendor/glad/include/**.h",
			"vendor/glad/**.c"
		}

		includedirs {
			"vendor/openal/libs/Win64",
			"vendor/glad/include",
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
		files {
			"vendor/glad-macos/include/**.h",
			"vendor/glad-macos/**.c"
		}

		includedirs {
			"vendor/glad-macos/include",
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
