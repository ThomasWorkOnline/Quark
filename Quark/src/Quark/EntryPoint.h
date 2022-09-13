#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

namespace Quark {

	///
	/// Entry point: must be defined by client
	///
	extern Application* CreateApplication(CommandLineArguments args);
}
