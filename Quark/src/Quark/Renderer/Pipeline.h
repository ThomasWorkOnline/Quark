#pragma once

#include "Quark/Core/Core.h"
#include "RenderModes.h"

#include "Buffer.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Quark {

	enum PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan
	};

	struct PipelineSpecification
	{
		BufferLayout        Layout;
		PrimitiveTopology   Topology{};
		Shader*             Shader = nullptr;
		RenderPass*         RenderPass = nullptr;

		std::vector<UniformBuffer*> UniformBuffers;
		std::vector<std::vector<Sampler2D*>> SamplersArray;
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
