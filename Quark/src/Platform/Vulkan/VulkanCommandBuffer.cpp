#include "qkpch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

namespace Quark {

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(device->GetVkHandle(), &allocInfo, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
	}

	void VulkanCommandBuffer::End()
	{
		vkEndCommandBuffer(m_CommandBuffer);
	}

	void VulkanCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		auto vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
		auto descriptorSet = vulkanPipeline->GetDescriptorSet();

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVkHandle());
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(),
			0, 1, &descriptorSet, 0, nullptr);
	}

	void VulkanCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		auto vkFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer)->GetVkHandle();
		auto vkRenderPass = static_cast<VulkanRenderPass*>(renderPass)->GetVkHandle();

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass;
		renderPassInfo.framebuffer = vkFramebuffer;
		
		// Must be size of framebuffer
		renderPassInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ framebuffer->GetWidth(), framebuffer->GetHeight() };

		if (renderPass->GetSpecification().ClearBuffers)
		{
			VkClearValue clearColor = { 0.01f, 0.01f, 0.01f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;
		}

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
	}

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	void VulkanCommandBuffer::Draw(uint32_t vertexOffset, uint32_t vertexCount)
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, 1, vertexOffset, 0);
	}

	void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanCommandBuffer::DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount)
	{
		QK_CORE_ASSERT(false, "Vulkan instance rendering is not yet implemented");
	}

	void VulkanCommandBuffer::DrawLines(uint32_t vertexCount)
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
	}

	void VulkanCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		VkDeviceSize offsets[] = { 0 };
		VkBuffer buffer = static_cast<VulkanVertexBuffer*>(vertexBuffer)->GetVkHandle();
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &buffer, offsets);
	}

	void VulkanCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		VkBuffer buffer = static_cast<VulkanIndexBuffer*>(indexBuffer)->GetVkHandle();
		vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}
}
