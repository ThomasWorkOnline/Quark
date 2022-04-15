#include "Renderer.h"
#include "RenderCommand.h"

namespace Quark {

	uint32_t Renderer::s_ViewportWidth = 0;
	uint32_t Renderer::s_ViewportHeight = 0;

	void Renderer::Initialize()
	{
		QK_SCOPE_TIMER(Renderer::Initialize);

		RenderCommand::Init();
		RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });
		QK_CORE_INFO(RenderCommand::GetSpecification());

		Renderer2D::Initialize();
	}

	void Renderer::Dispose()
	{
		Renderer2D::Dispose();
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		s_ViewportWidth = width; s_ViewportHeight = height;
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
