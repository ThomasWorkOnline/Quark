#include "qkpch.h"
#include "VulkanBuffer.h"

#include "VulkanUtils.h"

namespace Quark {

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, size_t size)
		: m_Device(device), m_Size(size)
	{
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer, &m_BufferMemory);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, const void* vertices, size_t size)
		: m_Device(device), m_Size(size)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, vertices, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer, &m_BufferMemory);

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size, 0, 0);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_CORE_ASSERT(size + offset <= m_Size,
			"Written size is too large : Size and Offset parameters must be within the total buffer size");

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, offset, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size, 0, offset);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	bool VulkanVertexBuffer::operator==(const VertexBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Buffer == o->m_Buffer;

		return false;
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, uint32_t count)
		: m_Device(device), m_Count(count)
	{
		size_t size = count * sizeof(uint32_t);
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer, &m_BufferMemory);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, const uint32_t* indices, uint32_t count)
		: m_Device(device), m_Count(count)
	{
		size_t size = count * sizeof(uint32_t);
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, indices, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer, &m_BufferMemory);

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size, 0, 0);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint32_t count, uint32_t firstIndex)
	{
		QK_CORE_ASSERT(count + firstIndex <= m_Count,
			"Written size is too large : Count and FirstIndex parameters must be within the total buffer size");

		size_t size = count * sizeof(uint32_t);
		size_t offset = firstIndex * sizeof(uint32_t);
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, offset, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size, 0, offset);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	bool VulkanIndexBuffer::operator==(const IndexBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Buffer == o->m_Buffer;

		return false;
	}
}
