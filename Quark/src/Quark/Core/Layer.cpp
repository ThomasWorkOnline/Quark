#include "qkpch.h"
#include "Layer.h"
#include "Application.h"

namespace Quark {

	Layer::Layer(Application* app)
		: m_Application(app)
	{
	}

	Window* Layer::GetWindow() const
	{
		return m_Application->GetWindow();
	}
}
