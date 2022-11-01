#pragma once

#include <Quark/Core/Core.h>
#include <Quark/Core/Application.h>

namespace Quark {

	///
	/// Entry point: must be defined by client
	///
	extern Application* CreateApplication(CommandLineArguments args);
}
