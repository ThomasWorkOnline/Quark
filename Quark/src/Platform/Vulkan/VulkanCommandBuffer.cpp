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

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	void VulkanCommandBuffer::SetCullFace(RenderCullMode mode)
	{
		QK_CORE_ASSERT(false, "Not implemented");
	}

	void VulkanCommandBuffer::SetDepthFunction(RenderDepthFunction func)
	{
		QK_CORE_ASSERT(false, "Not implemented");
	}

	void VulkanCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		auto vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
		auto descriptorSet = vulkanPipeline->GetDescriptorSet();

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVkHandle());
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(),
			0, 1, &descriptorSet, 0, nullptr);

		//vulkanPipeline->UpdateDescriptors();
	}

	void VulkanCommandBuffer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		VkViewport viewport{};
		viewport.width = (float)viewportWidth;
		viewport.height = (float)viewportHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VkExtent2D{ viewportWidth, viewportHeight };
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
	}

	void VulkanCommandBuffer::SetLineWidth(float width)
	{
		vkCmdSetLineWidth(m_CommandBuffer, width);
	}

	void VulkanCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		m_CurrentRenderPass = renderPass;

		auto vkFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer)->GetVkHandle();
		auto vkRenderPass = static_cast<VulkanRenderPass*>(renderPass)->GetVkHandle();

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass;
		renderPassInfo.framebuffer = vkFramebuffer;
		
		// Must be size of framebuffer
		renderPassInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ framebuffer->GetWidth(), framebuffer->GetHeight() };

		VkClearValue clearValue{};
		if (renderPass->GetSpecification().ClearBuffers)
		{
			auto& color = renderPass->GetSpecification().ClearColor;
			clearValue.color = { color.r, color.g, color.b, color.a };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearValue;
		}

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
		m_CurrentRenderPass = nullptr;
	}

	void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t vertexOffset)
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, 1, vertexOffset, 0);
	}

	void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanCommandBuffer::DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCommandBuffer::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount)
	{
		vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, 0, 0, 0);
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

	bool VulkanCommandBuffer::IsInsideRenderPass() const
	{
		return m_CurrentRenderPass;
	}
}
