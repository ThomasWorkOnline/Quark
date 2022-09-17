#include "qkpch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanEnums.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanUniformBuffer.h"

#define QK_ASSERT_PIPELINE_VALID_STATE(pipeline) QK_CORE_ASSERT(pipeline, "No pipeline was actively bound to the current command buffer!")

namespace Quark {

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device)
		: m_Device(device)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Device->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(m_Device->GetVkHandle(), &allocInfo, &m_CommandBuffer);
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

	void VulkanCommandBuffer::SetCullMode(RenderCullMode mode)
	{
		vkCmdSetCullMode(m_CommandBuffer, CullModeToVulkan(mode));
	}

	void VulkanCommandBuffer::SetDepthFunction(DepthCompareFunction func)
	{
		vkCmdSetDepthCompareOp(m_CommandBuffer, DepthCompareFunctionToVulkan(func));
	}

	void VulkanCommandBuffer::BindPipeline(const Pipeline* pipeline)
	{
		m_BoundPipeline = static_cast<const VulkanPipeline*>(pipeline);
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BoundPipeline->GetVkHandle());
	}

	void VulkanCommandBuffer::BindDescriptorSets()
	{
		auto descriptorSet = m_BoundPipeline->GetDescriptorSet();
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BoundPipeline->GetPipelineLayout(),
			0, 1, &descriptorSet, 0, nullptr);
	}

	void VulkanCommandBuffer::PushConstant(ShaderStage stage, const void* data, size_t size)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		vkCmdPushConstants(m_CommandBuffer, m_BoundPipeline->GetPipelineLayout(), ShaderStageToVulkan(stage), 0, (uint32_t)size, data);
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

	void VulkanCommandBuffer::BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer)
	{
		m_CurrentRenderPass = renderPass;

		auto vkFramebuffer = static_cast<const VulkanFramebuffer*>(framebuffer)->GetVkHandle();
		auto vkRenderPass = static_cast<const VulkanRenderPass*>(renderPass)->GetVkHandle();

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

	void VulkanCommandBuffer::BindVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		QK_CORE_ASSERT(vertexBuffer->GetLayout() == m_BoundPipeline->GetLayout(), "Buffer layout does not match the currently bound pipeline layout");

		VkDeviceSize offsets[] = { 0 };
		VkBuffer buffer = static_cast<const VulkanVertexBuffer*>(vertexBuffer)->GetVkHandle();
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &buffer, offsets);
	}

	void VulkanCommandBuffer::BindIndexBuffer(const IndexBuffer* indexBuffer)
	{
		VkBuffer buffer = static_cast<const VulkanIndexBuffer*>(indexBuffer)->GetVkHandle();
		vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanCommandBuffer::BindUniformBuffer(const UniformBuffer* uniformBuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);

		auto* vulkanUniformBuffer = static_cast<const VulkanUniformBuffer*>(uniformBuffer);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = vulkanUniformBuffer->GetVkHandle();
		bufferInfo.offset = 0;
		bufferInfo.range = vulkanUniformBuffer->GetSize();

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstBinding = vulkanUniformBuffer->GetBinding(); // TODO: group by binding and call vkUpdateDescriptorSets once per set
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.pBufferInfo = &bufferInfo;
		writeDescriptorSet.dstSet = m_BoundPipeline->GetDescriptorSet();

		vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
	}

	void VulkanCommandBuffer::BindTexture(const Texture* texture, const Sampler* sampler, uint32_t samplerIndex)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);

		auto* vulkanTexture = static_cast<const VulkanTexture2D*>(texture);
		auto* vulkanSampler = static_cast<const VulkanSampler*>(sampler);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = vulkanSampler->GetVkHandle();
		imageInfo.imageView = vulkanTexture->GetVkHandle();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstBinding = vulkanSampler->GetBinding();
		writeDescriptorSet.dstArrayElement = samplerIndex;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.pImageInfo = &imageInfo;
		writeDescriptorSet.dstSet = m_BoundPipeline->GetDescriptorSet();

		vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
	}

	bool VulkanCommandBuffer::IsInsideRenderPass() const
	{
		return m_CurrentRenderPass;
	}
	
	bool VulkanCommandBuffer::operator==(const CommandBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_CommandBuffer == o->m_CommandBuffer;

		return false;
	}
}
