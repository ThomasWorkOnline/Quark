#pragma once

#include "Quark/Core/Core.h"
#include "Formats.h"

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
		ColorFormat       ColorAttachmentFormat{};
		ColorFormat       DepthAttachmentFormat{};
		uint32_t          Samples;
		Vec4f             ClearColor{};
		bool              ClearBuffers = false;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& spec)
			: m_Spec(spec) {}

		virtual ~RenderPass() = default;
		virtual bool operator==(const RenderPass& other) const = 0;

		void SetClearColor(const Vec4f& clearColor) { m_Spec.ClearColor = clearColor; }

		const RenderPassSpecification& GetSpecification() const { return m_Spec; }

		static Scope<RenderPass> Create(const RenderPassSpecification& spec);

	protected:
		RenderPassSpecification m_Spec{};
	};
}
