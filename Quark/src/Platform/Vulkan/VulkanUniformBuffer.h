#pragma once

#include "Quark/Renderer/UniformBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanUniformBuffer final : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanDevice* device, size_t size, uint32_t binding);
		virtual ~VulkanUniformBuffer() override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		virtual bool operator==(const UniformBuffer& other) const override
		{
			return m_Buffer == reinterpret_cast<const VulkanUniformBuffer&>(other).m_Buffer;
		}

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
