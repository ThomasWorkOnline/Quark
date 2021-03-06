#pragma once

#include "../Core/Core.h"

#include "Ressource.h"

namespace Entropy {

	class CubeMap : public Ressource
	{
	public:
		virtual ~CubeMap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		static Ref<CubeMap> Create(const std::array<std::string, 6>& filepaths);
	};
}
