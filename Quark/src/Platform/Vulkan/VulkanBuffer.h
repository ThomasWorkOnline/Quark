#pragma once

#include "Quark/Renderer/Buffer.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const void* vertices, size_t size);
		VulkanVertexBuffer(size_t size);
		virtual ~VulkanVertexBuffer();

		virtual void Attach() const override;
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		VkBuffer GetVkHandle() const { return m_Buffer; }

		virtual bool operator==(const VertexBuffer& other) const override
		{
			return m_Buffer == static_cast<const VulkanVertexBuffer&>(other).m_Buffer;
		}
		
	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	class VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const uint32_t* indices, uint32_t count);
		VulkanIndexBuffer(uint32_t count);
		virtual ~VulkanIndexBuffer() override;

		virtual void Attach() const override;
		virtual void Detach() const override {}

		virtual void SetData(const uint32_t* data, uint32_t count, size_t offset = 0) override;
		virtual uint32_t GetCount() const override { return m_Count; }

		VkBuffer GetVkHandle() const { return m_Buffer; }

		virtual bool operator==(const IndexBuffer& other) const override
		{
			return m_Buffer == static_cast<const VulkanIndexBuffer&>(other).m_Buffer;
		}

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
		uint32_t m_Count;
	};
}
