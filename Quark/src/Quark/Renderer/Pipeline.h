#pragma once

#include "Quark/Core/Core.h"
#include "RenderModes.h"

#include "Buffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Quark {

	struct PipelineSpecification
	{
		uint32_t            ViewportWidth = 0, ViewportHeight = 0;
		size_t              CameraUniformBufferSize = 0;
		UniformBuffer*      UniformBuffer = nullptr;

		BufferLayout        Layout;
		Shader*             Shader = nullptr;
		RenderPass*         RenderPass = nullptr;
	};

	class Pipeline
	{
	public:
		Pipeline(const PipelineSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Pipeline() = default;

		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual bool operator==(const Pipeline& other) const = 0;
		
		const BufferLayout& GetLayout() const { return m_Spec.Layout; }
		const PipelineSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Pipeline> Create(const PipelineSpecification& spec);

	protected:
		PipelineSpecification m_Spec;
	};
}
