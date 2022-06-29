#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void Attach(uint32_t binding = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const UniformBuffer& other) const = 0;

		static Ref<UniformBuffer> Create(size_t size, uint32_t binding);
	};
}
