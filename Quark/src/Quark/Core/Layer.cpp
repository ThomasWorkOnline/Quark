#include "qkpch.h"
#include "Layer.h"
#include "Application.h"

namespace Quark {

	const Window& Layer::GetWindow() const
	{
		return Application::Get().GetWindow();
	}

	Window& Layer::GetWindow()
	{
		return Application::Get().GetWindow();
	}
}
