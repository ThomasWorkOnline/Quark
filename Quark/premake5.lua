project "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",

		"vendor/glad/include/**.h",
		"vendor/glad/**.c"
	}

	includedirs {
		"vendor/freetype/include",
		"vendor/glad/include",
		"vendor/entt/include",
		"vendor/glm",
		"vendor/openal/include",
		"vendor/stb_image"
	}

	defines {
		"AL_LIBTYPE_STATIC",
		"GLFW_INCLUDE_NONE"
	}

	links {
		"freetype",
		"glfw3",
		"OpenAL32"
	}

	filter "system:windows"
		systemversion "latest"

		links {
			"opengl32.lib"
		}

		includedirs {
			"vendor/glfw-prebuilt-win/include"
		}

		libdirs {
			"vendor/freetype/vs2015-2019/win64",
			"vendor/glfw-prebuilt-win/lib-vc2022",
			"vendor/openal/libs/Win64"
		}

	filter "system:unix"
		includedirs {
			"vendor/glfw/include"
		}

		libdirs {
			"vendor/glfw/lib-linux-x86_64"
		}
