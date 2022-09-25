#include "qkpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanUtils.h"

#include "Quark/Renderer/Renderer.h"

namespace Quark {

	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, const UniformBufferSpecification& spec)
		: UniformBuffer(spec)
		, m_Device(device)
	{
		QK_CORE_ASSERT(spec.Size <= Renderer::GetCapabilities().UniformBuffer.MaxBufferSize,
			"Uniform buffer Size too large: see Renderer::GetCapabilities() for more info");

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
		QK_CORE_ASSERT(size + offset <= m_Spec.Size,
			"Written size is too large: Size and Offset parameters must be within the total buffer size");

		void* mappedMemory;
		vkMapMemory(m_Device->GetVkHandle(), m_BufferMemory, offset, size, 0, &mappedMemory);
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
