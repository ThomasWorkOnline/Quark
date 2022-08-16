#include "qkpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanUtils.h"

namespace Quark {

	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, size_t size, uint32_t binding) : UniformBuffer(size),
		m_Device(device)
	{
		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_BufferMemory);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_CORE_ASSERT(size <= m_Size, "Size parameter must be less than or equal to the total buffer size");
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		void* mappedMemory;
		vkMapMemory(m_Device->GetVkHandle(), m_BufferMemory, 0, size, 0, &mappedMemory);
		std::memcpy(mappedMemory, data, size);
		vkUnmapMemory(m_Device->GetVkHandle(), m_BufferMemory);
	}
}
