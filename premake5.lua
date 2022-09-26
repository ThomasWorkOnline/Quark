include "Dependencies.lua"

workspace "Quark"
	architecture "x86_64"
	startproject "Demos"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

-- Quark Dependencies
group "Dependencies"
	include "Quark/vendor/freetype"
	include "Quark/vendor/glad"
	include "Quark/vendor/glfw"
	include "Quark/vendor/imgui"
	include "Quark/vendor/lodepng"
	include "Quark/vendor/spdlog"
group ""

group "Core"
	include "Quark"
group ""

group "Tools"
	include "Tools/InstalledVulkanVersion"
group ""

include "Demos"
