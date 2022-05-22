#pragma once

#include "Core.h"
#include "Application.h"

namespace Quark {

	///
	/// Client entry point: must be defined by client
	///
	extern Application* CreateApplication();
}
