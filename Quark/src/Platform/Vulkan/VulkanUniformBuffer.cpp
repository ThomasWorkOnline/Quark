#include "qkpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanUtils.h"

namespace Quark {

	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, const UniformBufferSpecification& spec)
		: UniformBuffer(spec)
		, m_Device(device)
	{
		m_Buffer = Utils::AllocateBuffer(m_Device, m_Spec.Size,
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
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");
		QK_CORE_ASSERT(size <= m_Spec.Size,
			"Written size is too large: parameter must be less than or equal to the total buffer size: expected {0} bytes but got {1}", m_Spec.Size, size);

		void* mappedMemory;
		vkMapMemory(m_Device->GetVkHandle(), m_BufferMemory, 0, size, 0, &mappedMemory);
		std::memcpy(mappedMemory, data, size);
		vkUnmapMemory(m_Device->GetVkHandle(), m_BufferMemory);
	}

	bool VulkanUniformBuffer::operator==(const UniformBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Buffer == o->m_Buffer;

		return false;
	}
}
