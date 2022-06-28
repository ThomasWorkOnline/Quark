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

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		virtual uint32_t GetRendererID() const override { return 0; }

		virtual bool operator==(const VertexBuffer& other) const override
		{
			return m_VkBuffer == ((VulkanVertexBuffer&)other).m_VkBuffer;
		}
		
	private:
		vk::Buffer m_VkBuffer;
		vk::DeviceMemory m_VkBufferMemory;

		BufferLayout m_Layout;
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

		virtual uint32_t GetCount() const override { return 0; }
		virtual uint32_t GetRendererID() const override { return 0; }

		virtual bool operator==(const IndexBuffer& other) const override
		{
			return m_VkBuffer == ((VulkanIndexBuffer&)other).m_VkBuffer;
		}

	private:
		vk::Buffer m_VkBuffer;
		vk::DeviceMemory m_VkBufferMemory;
	};
}
