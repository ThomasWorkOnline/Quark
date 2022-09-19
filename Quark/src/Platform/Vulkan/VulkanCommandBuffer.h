#pragma once

#include "Quark/Renderer/CommandBuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanPipeline;

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		VulkanCommandBuffer(VulkanDevice* device);

		virtual void Begin() final override;
		virtual void End() final override;
		virtual void Reset() final override;

		virtual void SetCullMode(RenderCullMode mode) final override;
		virtual void SetDepthFunction(DepthCompareFunction func) final override;

		virtual void BindPipeline(const Pipeline* pipeline) final override;
		virtual void BindDescriptorSets() final override;
		virtual void PushConstant(ShaderStage stage, const void* data, size_t size) final override;

		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) final override;
		virtual void SetLineWidth(float width) final override;

		virtual void BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer) final override;
		virtual void EndRenderPass() final override;

		virtual void Draw(uint32_t vertexCount, uint32_t vertexOffset) final override;
		virtual void DrawIndexed(uint32_t indexCount) final override;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount) final override;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) final override;

		virtual void BindVertexBuffer(const VertexBuffer* vertexBuffer) final override;
		virtual void BindIndexBuffer(const IndexBuffer* indexBuffer) final override;
		virtual void BindUniformBuffer(const UniformBuffer* uniformBuffer) final override;

		virtual void BindTexture(const Texture* texture, const Sampler* sampler, uint32_t samplerIndex = 0) final override;

		virtual bool IsInsideRenderPass() const final override;

		virtual bool operator==(const CommandBuffer& other) const final override;

		VkCommandBuffer GetVkHandle() const { return m_CommandBuffer; }

	private:
		VulkanDevice* m_Device = nullptr;
		VkCommandBuffer m_CommandBuffer = nullptr;
		const VulkanPipeline* m_BoundPipeline = nullptr;
		const RenderPass* m_CurrentRenderPass = nullptr;
	};
}
