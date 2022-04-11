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

outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

include "Quark"
include "Tests"
