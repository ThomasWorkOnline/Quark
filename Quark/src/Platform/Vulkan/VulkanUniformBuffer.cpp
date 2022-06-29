#include "qkpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanUniformBuffer::VulkanUniformBuffer(size_t size, uint32_t binding)
	{
		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			m_BufferMemory);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_Buffer);
		vkDevice.freeMemory(m_BufferMemory);
	}

	void VulkanUniformBuffer::Attach(uint32_t binding) const
	{
	}

	void VulkanUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		void* mappedMemory = vkDevice.mapMemory(m_BufferMemory, 0, size);
		std::memcpy(mappedMemory, data, size);
		vkDevice.unmapMemory(m_BufferMemory);
	}
}
