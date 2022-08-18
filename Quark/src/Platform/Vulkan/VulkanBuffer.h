#pragma once

#include "Quark/Renderer/Buffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(VulkanDevice* device, size_t size);
		VulkanVertexBuffer(VulkanDevice* device, const void* vertices, size_t size);
		virtual ~VulkanVertexBuffer();

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		virtual bool operator==(const VertexBuffer& other) const override
		{
			return m_Buffer == reinterpret_cast<const VulkanVertexBuffer&>(other).m_Buffer;
		}

		// Non-Copyable
		VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;

		VkBuffer GetVkHandle() const { return m_Buffer; }
		
	private:
		VulkanDevice* m_Device;

		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	class VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(VulkanDevice* device, uint32_t count);
		VulkanIndexBuffer(VulkanDevice* device, const uint32_t* indices, uint32_t count);
		virtual ~VulkanIndexBuffer() override;

		virtual void SetData(const uint32_t* data, uint32_t count, size_t offset = 0) override;
		virtual uint32_t GetCount() const override { return m_Count; }

		virtual bool operator==(const IndexBuffer& other) const override
		{
			return m_Buffer == reinterpret_cast<const VulkanIndexBuffer&>(other).m_Buffer;
		}

		// Non-Copyable
		VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;

		VkBuffer GetVkHandle() const { return m_Buffer; }

	private:
		VulkanDevice* m_Device;

		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
		uint32_t m_Count;
	};
}
