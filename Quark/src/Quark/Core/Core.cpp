#include "qkpch.h"
#include "Core.h"

#include "Quark/Scripting/ScriptRuntime.h"

#include <filesystem>

namespace Quark {

	static bool s_RuntimeInitialized = false;

	void RuntimeCore::Init()
	{
		auto& runtimeDirectory = GetRuntimeDirectory();
		if (!runtimeDirectory.empty())
		{
			ScriptRuntime::InitializeMono(runtimeDirectory);
			s_RuntimeInitialized = true;
		}
	}

	void RuntimeCore::Shutdown()
	{
		if (s_RuntimeInitialized)
			ScriptRuntime::ShutdownMono();
	}
}
