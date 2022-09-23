#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	struct UniformBufferSpecification
	{
		size_t Size = 0;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(const UniformBufferSpecification& spec)
			: m_Spec(spec) {}

		virtual ~UniformBuffer() = default;
		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

		virtual bool operator==(const UniformBuffer& other) const = 0;

		size_t GetSize() const { return m_Spec.Size; }

		const UniformBufferSpecification& GetSpecification() const { return m_Spec; }

		static Scope<UniformBuffer> Create(const UniformBufferSpecification& spec);

	protected:
		UniformBufferSpecification m_Spec{};
	};
}
