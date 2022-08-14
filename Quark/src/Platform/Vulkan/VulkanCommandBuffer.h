#pragma once

#include "Quark/Renderer/CommandBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		VulkanCommandBuffer(VulkanDevice* device);

		virtual void Begin() override;
		virtual void End() override;

		virtual void BindPipeline(Pipeline* pipeline) override;

		virtual void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer) override;
		virtual void EndRenderPass() override;

		virtual void Reset() override;
		virtual void Draw(uint32_t vertexOffset, uint32_t vertexCount) override;
		virtual void DrawIndexed(uint32_t indexCount) override;
		virtual void DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount) override;

		virtual void DrawLines(uint32_t vertexCount) override;

		virtual void BindVertexBuffer(VertexBuffer* vertexBuffer) override;
		virtual void BindIndexBuffer(IndexBuffer* indexBuffer) override;

		virtual bool operator==(const CommandBuffer& other) const override
		{
			return m_CommandBuffer == reinterpret_cast<const VulkanCommandBuffer&>(other).m_CommandBuffer;
		}

		VkCommandBuffer GetVkHandle() const { return m_CommandBuffer; }

	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
