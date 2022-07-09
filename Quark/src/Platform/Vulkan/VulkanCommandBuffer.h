#pragma once

#include "Quark/Renderer/CommandBuffer.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer();

		virtual void DrawIndexed(uint32_t indexCount, uint32_t indexOffset = 0) override;

		virtual void BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual bool operator==(const CommandBuffer& other) const override
		{
			return m_CommandBuffer == reinterpret_cast<const VulkanCommandBuffer&>(other).m_CommandBuffer;
		}

		vk::CommandBuffer GetVkHandle() const { return m_CommandBuffer; }

	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
