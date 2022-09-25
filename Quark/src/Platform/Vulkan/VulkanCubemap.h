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

		virtual void SetData(uint32_t index, const void* data, size_t size) final override {}

		virtual const void* GetHandle() const final override { return nullptr; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanCubemap(const VulkanCubemap&) = delete;
		VulkanCubemap& operator=(const VulkanCubemap&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}
