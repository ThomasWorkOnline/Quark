project "Tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
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
	
	filter "system:macosx"
		includedirs {
			"%{wks.location}/Quark/vendor/glfw-prebuilt-macos/include",
			"%{wks.location}/Quark/vendor/freetype/include",
			"%{wks.location}/Quark/vendor/glad-macos/include",
			"%{wks.location}/Quark/vendor/openal/include"
		}

		libdirs {
			"%{wks.location}/Quark/vendor/glfw-prebuilt-macos/lib-x86_64"
		}

		links {
			"Cocoa.framework",
			"IOKit.framework",
			"OpenAL.framework",
			"OpenGL.framework",
			--"freetype",
			"glfw3"
		}
