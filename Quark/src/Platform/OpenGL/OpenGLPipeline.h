#pragma once

#include "Quark/Renderer/RenderPipeline.h"

namespace Quark {

	class OpenGLPipeline final : public RenderPipeline
	{
	public:
		virtual ~OpenGLPipeline() override = default;

		virtual void Begin(const Mat4f& cameraProjection, const Mat4f& cameraView) override {}
		virtual void End() override {}

		virtual void BeginRenderPass() override {}
		virtual void EndRenderPass() override {}
		virtual void Submit() override {}

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override {}
	};
}
