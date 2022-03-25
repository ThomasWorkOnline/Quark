workspace "Quark"
	architecture "x86_64"
	startproject "Tests"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

	defines {
		"QK_64BIT_PRECISION"
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

outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

include "Quark"
include "Tests"
