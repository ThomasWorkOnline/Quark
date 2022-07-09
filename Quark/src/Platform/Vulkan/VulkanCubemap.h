#pragma once

#include "Quark/Renderer/Cubemap.h"

namespace Quark {

	class VulkanCubemap final : public Cubemap
	{
	public:
		VulkanCubemap(const CubemapSpecification& spec) {}
		virtual ~VulkanCubemap() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual void SetData(uint32_t index, const void* data, size_t size) override {}

		virtual bool operator==(const Cubemap& other) const override
		{
			return false;
		}
	};
}
