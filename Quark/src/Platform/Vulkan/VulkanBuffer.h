#pragma once

#include "Quark/Renderer/Buffer.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(VulkanDevice* device, size_t size);
		VulkanVertexBuffer(VulkanDevice* device, const void* vertices, size_t size);
		virtual ~VulkanVertexBuffer() final override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) final override;
		virtual bool operator==(const VertexBuffer& other) const final override;

		// Non-Copyable
		VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;

		VkBuffer GetVkHandle() const { return m_Buffer; }
		
	private:
		VulkanDevice* m_Device;

		size_t m_Size;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	class VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(VulkanDevice* device, uint32_t count, IndexType indexType);
		VulkanIndexBuffer(VulkanDevice* device, const uint32_t* indices, uint32_t count, IndexType indexType);
		virtual ~VulkanIndexBuffer() final override;

		virtual void SetData(const uint32_t* data, uint32_t count, uint32_t firstIndex = 0) final override;
		virtual bool operator==(const IndexBuffer& other) const final override;

		// Non-Copyable
		VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;

		VkBuffer GetVkHandle() const { return m_Buffer; }

	private:
		VulkanDevice* m_Device;

		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};
}
