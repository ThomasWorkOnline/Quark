#include "qkpch.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	namespace Utils {

		static uint32_t GetBufferMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
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

		static vk::Buffer AllocateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::DeviceMemory& bufferMemory)
		{
			QK_PROFILE_FUNCTION();

			vk::BufferCreateInfo bufferInfo;
			bufferInfo.setSize(size);
			bufferInfo.setUsage(usage);
			bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			vk::Buffer buffer = vkDevice.createBuffer(bufferInfo);

			vk::MemoryRequirements memRequirements = vkDevice.getBufferMemoryRequirements(buffer);
			uint32_t memoryTypeIndex = Utils::GetBufferMemoryType(memRequirements.memoryTypeBits, properties);

			vk::MemoryAllocateInfo allocInfo;
			allocInfo.setAllocationSize(memRequirements.size);
			allocInfo.setMemoryTypeIndex(memoryTypeIndex);

			bufferMemory = vkDevice.allocateMemory(allocInfo);
			vkDevice.bindBufferMemory(buffer, bufferMemory, 0);

			return buffer;
		}

		// TODO: copy allocator
		static void CopyBuffer(vk::Buffer dstBuffer, vk::Buffer srcBuffer, size_t size)
		{
			QK_PROFILE_FUNCTION();

			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			auto commandPool = VulkanContext::GetCurrentDevice().GetCommandPool();

			vk::CommandBufferAllocateInfo allocInfo;
			allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
			allocInfo.setCommandPool(commandPool);
			allocInfo.setCommandBufferCount(1);

			vk::CommandBuffer commandBuffer = vkDevice.allocateCommandBuffers(allocInfo)[0];

			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			vk::BufferCopy copyRegion;
			copyRegion.setSize(size);

			commandBuffer.begin(beginInfo);
			commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
			commandBuffer.end();

			vk::SubmitInfo submitInfo;
			submitInfo.setCommandBufferCount(1);
			submitInfo.setPCommandBuffers(&commandBuffer);

			auto graphicsQueue = VulkanContext::GetCurrentDevice().GetGraphicsQueue();

			graphicsQueue.submit(submitInfo);
			graphicsQueue.waitIdle();

			vkDevice.freeCommandBuffers(commandPool, 1, &commandBuffer);
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, size_t size)
	{
		QK_PROFILE_FUNCTION();

		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, vertices, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		m_VkBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_VkBufferMemory);

		Utils::CopyBuffer(m_VkBuffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size)
	{
		QK_PROFILE_FUNCTION();

		m_VkBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_VkBufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_VkBuffer);
		vkDevice.freeMemory(m_VkBufferMemory);
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_PROFILE_FUNCTION();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, data, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		Utils::CopyBuffer(m_VkBuffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, indices, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		m_VkBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_VkBufferMemory);

		Utils::CopyBuffer(m_VkBuffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		m_VkBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_VkBufferMemory);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_VkBuffer);
		vkDevice.freeMemory(m_VkBufferMemory);
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint32_t count, size_t offset)
	{
		QK_PROFILE_FUNCTION();
		QK_CORE_ASSERT(offset == 0, "offsets are currently not supported");

		size_t size = count * sizeof(uint32_t);
		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, data, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		Utils::CopyBuffer(m_VkBuffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
	}
}
