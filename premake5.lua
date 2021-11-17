workspace "Quark"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"

project "Quark"
	location "Quark"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor", -- glm
		"%{prj.name}/vendor/entt/include",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/glew/include",
		"%{prj.name}/vendor/glfw/include",
		"%{prj.name}/vendor/irrKlang/include",
		"%{prj.name}/vendor/freetype/include"
	}

	links
	{
		"Quark"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"QK_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Tests"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Proton")
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

	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Quark/src", -- project ref
		"%{prj.name}/vendor", -- glm
		"%{prj.name}/vendor/entt/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"QK_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "QK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		symbols "On"
		optimize "On"
