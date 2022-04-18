#pragma once

#include "Quark/Core/Core.h"

#include <array>

namespace Quark {

	class Cubemap
	{
	public:
		virtual ~Cubemap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(uint32_t index, const void* data, size_t size) = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const Cubemap& other) const = 0;

		static Ref<Cubemap> Create(uint32_t width, uint32_t height);
	};
}
