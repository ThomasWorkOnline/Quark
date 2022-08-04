#include "qkpch.h"
#include "VulkanBuffer.h"

#include "VulkanUtils.h"

namespace Quark {

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, const void* vertices, size_t size)
		: m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		VkDeviceMemory stagingBufferMemory;
		VkBuffer stagingBuffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, vertices, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, size_t size)
		: m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Attach() const
	{
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_PROFILE_FUNCTION();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		VkDeviceMemory stagingBufferMemory;
		VkBuffer stagingBuffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, const uint32_t* indices, uint32_t count)
		: m_Device(device), m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		VkDeviceMemory stagingBufferMemory;
		VkBuffer stagingBuffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, indices, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, uint32_t count)
		: m_Device(device), m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::Attach() const
	{
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint32_t count, size_t offset)
	{
		QK_PROFILE_FUNCTION();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		size_t size = count * sizeof(uint32_t);
		VkDeviceMemory stagingBufferMemory;
		VkBuffer stagingBuffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBufferMemory
		);

		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Device, m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}
}
