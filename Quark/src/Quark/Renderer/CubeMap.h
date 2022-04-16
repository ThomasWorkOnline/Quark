#pragma once

#include "Quark/Core/Core.h"

#include <array>

namespace Quark {

	class CubeMap
	{
	public:
		virtual ~CubeMap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const CubeMap& other) const = 0;

		static Ref<CubeMap> Create(uint32_t width, uint32_t height);
	};
}
