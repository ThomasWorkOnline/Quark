project "Tests"
	kind "WindowedApp"
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
	
	-- for some reason, generating the project using gmake2 requires us to redefine the dependencies to link
	filter "system:macosx"
		includedirs {
			"%{wks.location}/Quark/vendor/glfw-prebuilt-macos/include",
			"%{wks.location}/Quark/vendor/freetype/include",
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
