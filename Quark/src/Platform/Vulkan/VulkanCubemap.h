#pragma once

#include "Quark/Renderer/Cubemap.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanCubemap final : public Cubemap
	{
	public:
		VulkanCubemap(VulkanDevice* device, const CubemapSpecification& spec);
		virtual ~VulkanCubemap() final override = default;

		virtual void Attach(uint32_t textureSlot = 0) const final override {}
		virtual void Detach() const final override {}

		virtual void SetData(uint32_t index, const void* data, size_t size) final override {}

		virtual bool operator==(const Cubemap& other) const final override
		{
			return false;
		}

		// Non-Copyable
		VulkanCubemap(const VulkanCubemap&) = delete;
		VulkanCubemap& operator=(const VulkanCubemap&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}
