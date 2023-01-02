#pragma once

#include "Quark/Renderer/UniformBuffer.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanUniformBuffer final : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanDevice* device, size_t size);
		virtual ~VulkanUniformBuffer() final override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) final override;

		virtual bool operator==(const UniformBuffer& other) const final override;

		// Non-Copyable
		VulkanUniformBuffer(const VulkanUniformBuffer&) = delete;
		VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;

		VkBuffer GetVkHandle() const { return m_Buffer; }

	private:
		VulkanDevice* m_Device;

		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};
}
