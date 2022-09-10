#pragma once

#include "Quark/Core/Core.h"
#include "RenderModes.h"

#include "Buffer.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"
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
		BufferLayout          Layout;
		PrimitiveTopology     Topology{};

		const Shader*         Shader = nullptr;
		const RenderPass*     RenderPass = nullptr;

		uint32_t              UniformBufferCount = 0;
		const UniformBuffer** UniformBuffers = nullptr;
	};

	class Pipeline
	{
	public:
		Pipeline(const PipelineSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Pipeline() = default;

		virtual void SetTexture(const Texture* texture, uint32_t textureIndex) = 0;
		virtual void PushDescriptorSets() = 0;

		virtual bool operator==(const Pipeline& other) const = 0;
		
		const BufferLayout& GetLayout() const { return m_Spec.Layout; }
		const PipelineSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Pipeline> Create(const PipelineSpecification& spec);

	protected:
		PipelineSpecification m_Spec{};
	};
}
