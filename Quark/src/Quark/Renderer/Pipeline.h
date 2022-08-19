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
		BufferLayout        Layout;
		Shader*             Shader = nullptr;
		RenderPass*         RenderPass = nullptr;
		UniformBuffer*      UniformBuffer = nullptr;
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
