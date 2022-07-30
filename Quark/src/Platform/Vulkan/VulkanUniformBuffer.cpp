#include "qkpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanUniformBuffer::VulkanUniformBuffer(size_t size, uint32_t binding)
	{
		m_Buffer = Utils::AllocateBuffer(size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_BufferMemory);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyBuffer(vkDevice, m_Buffer, nullptr);
		vkFreeMemory(vkDevice, m_BufferMemory, nullptr);
	}

	void VulkanUniformBuffer::Attach(uint32_t binding) const
	{
	}

	void VulkanUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		void* mappedMemory;
		vkMapMemory(vkDevice, m_BufferMemory, 0, size, 0, &mappedMemory);
		std::memcpy(mappedMemory, data, size);
		vkUnmapMemory(vkDevice, m_BufferMemory);
	}
}
