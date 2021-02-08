#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class EnvironmentMap
	{
	public:
		virtual ~EnvironmentMap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		static Ref<EnvironmentMap> Create(const std::array<std::string, 6>& filepaths);
	};
}
