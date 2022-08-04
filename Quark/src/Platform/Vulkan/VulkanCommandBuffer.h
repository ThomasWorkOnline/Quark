#pragma once

#include "Quark/Renderer/CommandBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device);

		virtual void Begin() override;
		virtual void End() override;

		virtual void BindPipeline(const Ref<Pipeline>& pipeline) override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer) override;
		virtual void EndRenderPass() override;

		virtual void Reset() override;
		virtual void DrawIndexed(uint32_t indexCount) override;

		virtual void BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual bool operator==(const CommandBuffer& other) const override
		{
			return m_CommandBuffer == reinterpret_cast<const VulkanCommandBuffer&>(other).m_CommandBuffer;
		}

		VkCommandBuffer GetVkHandle() const { return m_CommandBuffer; }

	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
