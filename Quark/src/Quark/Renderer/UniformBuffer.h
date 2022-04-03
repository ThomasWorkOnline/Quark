#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual uint32_t GetRendererID() const = 0;
		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

		static Ref<UniformBuffer> Create(size_t size, uint32_t binding);
	};
}
