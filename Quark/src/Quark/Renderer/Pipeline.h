#pragma once

#include "Quark/Core/Core.h"
#include "RenderModes.h"

#include "Buffer.h"
#include "RenderPass.h"
#include "Shader.h"

namespace Quark {

	enum class PrimitiveTopology
	{
		None = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan
	};

	struct PipelineSpecification
	{
		BufferLayout          Layout;
		PrimitiveTopology     Topology{};
		uint32_t              Samples = 1;
		uint32_t              DescriptorSetCount = 0;

		const Shader*         Shader = nullptr;
		const RenderPass*     RenderPass = nullptr;
	};

	class Pipeline
	{
	public:
		Pipeline(const PipelineSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Pipeline() = default;

		virtual bool operator==(const Pipeline& other) const = 0;
		
		const BufferLayout& GetLayout() const { return m_Spec.Layout; }
		const PipelineSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Pipeline> Create(const PipelineSpecification& spec);

	protected:
		PipelineSpecification m_Spec{};
	};
}
