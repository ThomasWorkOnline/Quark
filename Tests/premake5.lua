project "Tests"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Quark/src",
		"%{wks.location}/Quark/vendor/glm",
		"%{wks.location}/Quark/vendor/entt/include",
		"%{wks.location}/Quark/vendor/spdlog/include"
	}

	links
	{
		"Quark"
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

	filter "system:windows"
		kind "ConsoleApp"

	filter "system:macosx"
		kind "WindowedApp"
