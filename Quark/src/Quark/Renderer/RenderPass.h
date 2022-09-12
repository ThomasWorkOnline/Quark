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

	enum class SampleCount
	{
		SampleCount1 = 0,
		SampleCount2,
		SampleCount4,
		SampleCount8,
		SampleCount16,
		SampleCount32,
		SampleCount64
	};

	struct RenderPassSpecification
	{
		PipelineBindPoint BindPoint{};
		ColorDataFormat   ColorFormat{};
		Vec4f             ClearColor{};
		bool              ClearBuffers = false;
		SampleCount       Samples{};
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
