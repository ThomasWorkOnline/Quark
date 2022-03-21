#pragma once

#include "../Core/Core.h"

#include "DeferredObject.h"

namespace Quark {

	class CubeMap : public DeferredObject
	{
	public:
		virtual ~CubeMap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const CubeMap& other) const = 0;

		static Ref<CubeMap> Create(const std::array<std::string_view, 6>& filepaths);
	};
}
