project "Quark-RuntimeCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "qkpch.h"
	pchsource "src/qkpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Quark/src",
		"%{VendorDir}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"Quark"
	}

	defines
	{
		"GLM_FORCE_LEFT_HANDED"
	}

	flags
	{
		"FatalCompileWarnings",
		"ShadowedVariables"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING"
		}

	filter "configurations:Debug"
		defines "QK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		runtime "Release"
		optimize "On"
		symbols	"On"

	filter "configurations:Dist"
		defines "QK_DIST"
		runtime "Release"
		optimize "Full"
		symbols "Off"

	filter { "configurations:Release", "system:windows" }
		flags
		{
			"LinkTimeOptimization"
		}

	filter { "configurations:Dist", "system:windows" }
		flags
		{
			"LinkTimeOptimization"
		}
