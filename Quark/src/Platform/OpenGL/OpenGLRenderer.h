#pragma once

#include "Quark/Renderer/Renderer.h"

namespace Quark {

	class OpenGLRenderer final : public BackendRenderer
	{
	public:
		virtual ~OpenGLRenderer() = default;

		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void BeginFrame() override;
		virtual void Submit() override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;
	};
}
