#include "qkpch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"

namespace Quark {

	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		auto& device = VulkanContext::GetCurrentDevice();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(device.GetVkHandle(), &allocInfo, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset)
	{
		vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, indexOffset, 0, 0);
	}

	void VulkanCommandBuffer::BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding)
	{
		VkDeviceSize offsets[] = { 0 };
		VkBuffer buffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer)->GetVkHandle();
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &buffer, offsets);
	}

	void VulkanCommandBuffer::BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		VkBuffer buffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer)->GetVkHandle();
		vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}
}
