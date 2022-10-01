#pragma once

#include "Quark/Renderer/Cubemap.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanCubemap final : public Cubemap
	{
	public:
		VulkanCubemap(VulkanDevice* device, const CubemapSpecification& spec);
		virtual ~VulkanCubemap() final override;

		virtual void SetData(const void* data, size_t size, uint32_t index) final override;

		virtual const void* GetHandle() const final override { return nullptr; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanCubemap(const VulkanCubemap&) = delete;
		VulkanCubemap& operator=(const VulkanCubemap&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}
