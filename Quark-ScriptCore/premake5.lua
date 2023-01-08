project "Quark-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Quark-RuntimeCore/Resources/Scripts")
	objdir ("%{wks.location}/Quark-RuntimeCore/Resources/Scripts/Intermediates")

	files
	{
		"Source/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "On"
		symbols "Off"
