#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

namespace Quark {

	///
	/// Client entry point: must be defined by client
	///
	extern Application* CreateApplication();
}
