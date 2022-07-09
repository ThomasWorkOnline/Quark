#pragma once

#include "ColorFormats.h"
#include "Framebuffer.h"

namespace Quark {

	enum class PipelineBindPoint
	{
		None = 0,
		Graphics,
		Compute
	};

	struct RenderPassSpecification
	{
		PipelineBindPoint BindPoint{};
		InternalColorFormat ColorFormat{};
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual const RenderPassSpecification& GetSpecification() const = 0;

		virtual bool operator==(const RenderPass& other) const = 0;

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	};
}
