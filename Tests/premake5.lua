project "Tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"%{wks.location}/Quark/src",
		"%{wks.location}/Quark/vendor/glm",
		"%{wks.location}/Quark/vendor/entt/include"
	}

	links {
		"Quark"
	}

	postbuildcommands {
		("{COPY} %{wks.location}/Quark/vendor/irrKlang/bin/winx64-visualStudio/**.dll ../bin/" .. outputdir .. "/%{prj.name}")
	}

	filter "system:windows"
		systemversion "latest"
