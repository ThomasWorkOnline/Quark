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

	defines {
		"QK_64BIT_PRECISION"
	}

	filter "configurations:Debug"
		defines "QK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "QK_RELEASE"
		symbols "On"
		optimize "On"

outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

include "Quark"
include "Tests"
