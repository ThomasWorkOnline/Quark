#pragma once

#include "Quark/Renderer/RenderPass.h"

namespace Quark {

	class OpenGLRenderPass final : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec);

		virtual bool operator==(const RenderPass& other) const final override;

		// Non-Copyable
		OpenGLRenderPass(const OpenGLRenderPass&) = delete;
		OpenGLRenderPass& operator=(const OpenGLRenderPass&) = delete;
	};
}
