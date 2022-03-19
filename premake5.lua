workspace "Quark"
	architecture "x86_64"
	startproject "Tests"

	configurations {
		"Debug",
		"Release"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

project "Quark"
	location "Quark"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/vendor/glad/include/**.h",
		"%{prj.name}/vendor/glad/**.c",

		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/vendor", -- stb_image
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/entt/include",
		"%{prj.name}/vendor/freetype/include",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/glfw/include",
		"%{prj.name}/vendor/irrKlang/include"
	}

	defines {
		"GLFW_INCLUDE_NONE",
		"QK_DOUBLE_FLOATING_POINT"
	}

	filter "system:windows"
		systemversion "latest"

		links {
			"freetype.lib",
			"opengl32.lib",
			"glfw3.lib",
			"irrKlang.lib"
		}

		libdirs {
			"%{prj.name}/vendor/freetype/vs2015-2019/win64",
			"%{prj.name}/vendor/glfw/lib-vc2019/x64",
			"%{prj.name}/vendor/irrKlang/lib/Winx64-visualStudio"
		}

		postbuildcommands {
			("{COPY} vendor/irrKlang/bin/winx64-visualStudio/**.dll ../bin/" .. outputdir .. "/Tests")
		}

    buildcommands {
        "./Scripts/GenGLFWMakefile_Lin.sh",
        "./Scripts/BuildGLFW_Lin.sh"
    }

	filter "configurations:Debug"
		defines "QK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		symbols "On"
		optimize "On"

project "Tests"
	location "Tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Quark/src",
		"Quark/vendor",
		"Quark/vendor/glm",
		"Quark/vendor/entt/include"
	}

	libdirs {
		"bin/" .. outputdir .. "/%{prj.name}"
	}

	links {
		"Quark"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		symbols "On"
		optimize "On"
