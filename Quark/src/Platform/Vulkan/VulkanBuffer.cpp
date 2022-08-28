#include "qkpch.h"
#include "VulkanBuffer.h"

#include "VulkanUtils.h"

namespace Quark {

	namespace Utils {

		static uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			QK_CORE_ASSERT(false, "Unable to find suitable memory type for buffer");
			return 0;
		}

		static VkBuffer AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory)
		{
			QK_PROFILE_FUNCTION();

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto vkPhysicalDevice = device->GetPhysicalDevice();

			VkBuffer buffer;
			vkCreateBuffer(device->GetVkHandle(), &bufferInfo, nullptr, &buffer);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device->GetVkHandle(), buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = Utils::GetBufferMemoryType(vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

			vkAllocateMemory(device->GetVkHandle(), &allocInfo, nullptr, &bufferMemory);
			vkBindBufferMemory(device->GetVkHandle(), buffer, bufferMemory, 0);

			return buffer;
		}

		// TODO: copy allocator
		static void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size)
		{
			QK_PROFILE_FUNCTION();

			auto commandPool = device->GetCommandPool();

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(device->GetVkHandle(), &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VkBufferCopy copyRegion{};
			copyRegion.size = size;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			auto graphicsQueue = device->GetGraphicsQueue();

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(graphicsQueue);

			vkFreeCommandBuffers(device->GetVkHandle(), commandPool, 1, &commandBuffer);
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, size_t size)
		: m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);
	}

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

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
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

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, uint32_t count)
		: m_Device(device), m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		m_Buffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);
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

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyBuffer(m_Device->GetVkHandle(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint32_t count, size_t offset)
	{
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
