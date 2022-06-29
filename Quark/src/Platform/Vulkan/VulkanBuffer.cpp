#include "qkpch.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"

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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, vertices, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_Buffer);
		vkDevice.freeMemory(m_BufferMemory);
	}

	void VulkanVertexBuffer::Attach() const
	{
		vk::DeviceSize offsets[] = { 0 };

		auto commandBuffer = VulkanContext::GetCurrentDevice().GetCommandBuffer();
		commandBuffer.bindVertexBuffers(0, 1, &m_Buffer, offsets);
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

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		void* mappedMemory = vkDevice.mapMemory(stagingBufferMemory, 0, size);
		std::memcpy(mappedMemory, indices, size);
		vkDevice.unmapMemory(stagingBufferMemory);

		m_Buffer = Utils::AllocateBuffer(size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, m_BufferMemory);

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyBuffer(m_Buffer);
		vkDevice.freeMemory(m_BufferMemory);
	}

	void VulkanIndexBuffer::Attach() const
	{
		auto commandBuffer = VulkanContext::GetCurrentDevice().GetCommandBuffer();
		commandBuffer.bindIndexBuffer(m_Buffer, 0, vk::IndexType::eUint32);
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

		Utils::CopyBuffer(m_Buffer, stagingBuffer, size);

		vkDevice.destroyBuffer(stagingBuffer);
		vkDevice.freeMemory(stagingBufferMemory);
	}
}
