#include "qkpch.h"
#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, size_t size)
	{
		QK_PROFILE_FUNCTION();

		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		{
			void* mappedMemory;
			vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, vertices, size);
			vkUnmapMemory(vkDevice, stagingBufferMemory);
		}

		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkDevice, stagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size)
	{
		QK_PROFILE_FUNCTION();

		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyBuffer(vkDevice, m_Buffer, nullptr);
		vkFreeMemory(vkDevice, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Attach() const
	{
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

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		{
			void* mappedMemory;
			vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(vkDevice, stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkDevice, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		vk::DeviceMemory stagingBufferMemory;
		vk::Buffer stagingBuffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBufferMemory
		);

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		{
			void* mappedMemory;
			vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, indices, size);
			vkUnmapMemory(vkDevice, stagingBufferMemory);
		}

		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkDevice, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		size_t size = count * sizeof(uint32_t);
		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyBuffer(vkDevice, m_Buffer, nullptr);
		vkFreeMemory(vkDevice, m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::Attach() const
	{
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

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		{
			void* mappedMemory;
			vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, data, size);
			vkUnmapMemory(vkDevice, stagingBufferMemory);
		}

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDestroyBuffer(vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkDevice, stagingBufferMemory, nullptr);
	}
}
