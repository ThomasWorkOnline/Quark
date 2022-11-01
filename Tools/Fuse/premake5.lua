project "Fuse"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Quark/src",
		"%{wks.location}/RuntimeCore/src",
		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"RuntimeCore"
	}

	filter { "configurations:Dist", "system:windows" }
		kind "WindowedApp"

	filter "system:windows"
		flags
		{
			"FatalCompileWarnings",
			"ShadowedVariables"
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING"
		}

	filter "system:macosx"
		kind "WindowedApp"
		
		links
		{
			"Quark",
			"freetype",
			"Glad",
			"GLFW",
			"ImGui",
			"lodepng",
			"spdlog",

			"Cocoa.framework",
			"CoreVideo.framework",
			"Foundation.framework",
			"IOKit.framework",
			"IOSurface.framework",
			"Metal.framework",
			"OpenAL.framework",
			"OpenGL.framework",
			"QuartzCore.framework",

			"vulkan",
			"MoltenVK",

			"spirv-cross-c",
			"spirv-cross-cpp",
			"spirv-cross-core",
			"spirv-cross-glsl",
			"spirv-cross-hlsl",
			"spirv-cross-msl",
			"spirv-cross-reflect",
			"spirv-cross-util",

			"shaderc_combined",
			"shaderc_util",

			"glslang"
		}

		libdirs
		{
			"/Users/thomaslessard/VulkanSDK/1.3.224.1/macOS/lib",
			"/Users/thomaslessard/VulkanSDK/1.3.224.1/MoltenVK/MoltenVK.xcframework/macos-arm64_x86_64"
		}

		includedirs
		{
			"/Users/thomaslessard/VulkanSDK/1.3.224.1/MoltenVK/include"
		}
	
	filter "system:linux"
		links
		{
			"Quark",
			"freetype",
			"Glad",
			"GLFW",
			"ImGui",
			"lodepng",
			"spdlog",

			"GL",
			"X11",
			"openal"
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
