#pragma once

#include "ColorFormats.h"

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
		ColorDataFormat   ColorFormat{};
		bool Clears = false;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& spec)
			: m_Spec(spec) {}

		virtual ~RenderPass() = default;
		virtual bool operator==(const RenderPass& other) const = 0;

		const RenderPassSpecification& GetSpecification() const { return m_Spec; }

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);

	protected:
		RenderPassSpecification m_Spec;
	};
}
