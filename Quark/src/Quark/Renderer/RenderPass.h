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
		float             ClearDepth = 1.0f;
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
		void SetClearDepth(float value) { m_Spec.ClearDepth = value; }

		uint32_t GetAttachmentCount() const { return m_AttachmentCount; }

		const RenderPassSpecification& GetSpecification() const { return m_Spec; }

		static Scope<RenderPass> Create(const RenderPassSpecification& spec);

	protected:
		RenderPassSpecification m_Spec{};
		uint32_t m_AttachmentCount = 0;
	};
}
