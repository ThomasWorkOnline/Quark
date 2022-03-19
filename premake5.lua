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

include "Quark"
include "Tests"
