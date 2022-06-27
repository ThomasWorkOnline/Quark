#include "qkpch.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	namespace Utils {

		uint32_t GetBufferMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
		{
			auto vkPhysicalDevice = VulkanContext::GetCurrentDevice().GetPhysicalDevice();
			vk::PhysicalDeviceMemoryProperties memProperties = vkPhysicalDevice.getMemoryProperties();

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			QK_CORE_ASSERT(false, "Unable to find suitable memory type for buffer");
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, size_t size)
	{
		vk::BufferCreateInfo bufferInfo;
		bufferInfo.setSize(size);
		bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
		bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		m_VkBuffer = vkDevice.createBuffer(bufferInfo);

		vk::MemoryRequirements memRequirements = vkDevice.getBufferMemoryRequirements(m_VkBuffer);
		uint32_t memoryTypeIndex = Utils::GetBufferMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.setAllocationSize(memRequirements.size);
		allocInfo.setMemoryTypeIndex(memoryTypeIndex);

		m_VkBufferMemory = vkDevice.allocateMemory(allocInfo);
		vkDevice.bindBufferMemory(m_VkBuffer, m_VkBufferMemory, 0);

		void* mappedMemory = vkDevice.mapMemory(m_VkBufferMemory, 0, size);
		std::memcpy(mappedMemory, vertices, size);
		vkDevice.unmapMemory(m_VkBufferMemory);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size)
	{
		vk::BufferCreateInfo bufferInfo;
		bufferInfo.setSize(size);
		bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
		bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		m_VkBuffer = vkDevice.createBuffer(bufferInfo);

		vk::MemoryRequirements memRequirements = vkDevice.getBufferMemoryRequirements(m_VkBuffer);
		uint32_t memoryTypeIndex = Utils::GetBufferMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.setAllocationSize(memRequirements.size);
		allocInfo.setMemoryTypeIndex(memoryTypeIndex);

		m_VkBufferMemory = vkDevice.allocateMemory(allocInfo);
		vkDevice.bindBufferMemory(m_VkBuffer, m_VkBufferMemory, 0);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_VkBuffer);
		vkDevice.freeMemory(m_VkBufferMemory);
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		// TODO: flush memory?
		void* mappedMemory = vkDevice.mapMemory(m_VkBufferMemory, offset, size);
		std::memcpy(mappedMemory, data, size);
		vkDevice.unmapMemory(m_VkBufferMemory);
	}
}
