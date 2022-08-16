#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class UniformBuffer
	{
	public:
		UniformBuffer(size_t size)
			: m_Size(size) {}

		virtual ~UniformBuffer() = default;
		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

		virtual bool operator==(const UniformBuffer& other) const = 0;

		size_t GetSize() const { return m_Size; }

		static Scope<UniformBuffer> Create(size_t size, uint32_t binding);

	protected:
		size_t m_Size = 0;
	};
}
