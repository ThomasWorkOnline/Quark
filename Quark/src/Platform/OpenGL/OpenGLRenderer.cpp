#include "qkpch.h"
#include "OpenGLRenderer.h"

#include <glad/glad.h>

namespace Quark {

	struct OpenGLData
	{
		Ref<CommandBuffer> CommandBuffer;
	};

	static OpenGLData s_Data;

	void OpenGLRenderer::Initialize()
	{
		s_Data.CommandBuffer = CommandBuffer::Create();
	}

	void OpenGLRenderer::Shutdown()
	{
	}

	void OpenGLRenderer::BeginFrame()
	{
	}

	void OpenGLRenderer::Submit()
	{
	}

	const Ref<CommandBuffer>& OpenGLRenderer::GetCommandBuffer() const
	{
		return s_Data.CommandBuffer;
	}

	void OpenGLRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
	}
}
