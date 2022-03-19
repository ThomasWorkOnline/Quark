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
		"vendor/glad/**.c"
	}

	includedirs {
		"vendor/freetype/include",
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/entt/include",
		"vendor/glm",
		"vendor/stb_image",
		"vendor/irrKlang/include"
	}

	defines {
		"GLFW_INCLUDE_NONE",
		"QK_DOUBLE_FLOATING_POINT"
	}

	links {
		"freetype",
		"glfw3",
		"irrKlang"
	}

	filter "system:windows"
		systemversion "latest"

		links {
			"opengl32.lib"
		}

		libdirs {
			"vendor/freetype/vs2015-2019/win64",
			"vendor/glfw/lib-vc2019/x64",
			"vendor/irrKlang/lib/Winx64-visualStudio"
		}

	filter "system:unix"
		libdirs {
			"vendor/glfw/lib-linux-x86_64"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		symbols "On"
		optimize "On"
