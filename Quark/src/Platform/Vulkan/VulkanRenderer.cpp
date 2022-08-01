#include "qkpch.h"
#include "VulkanRenderer.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "Quark/Renderer/Renderer.h"

namespace Quark {

	struct VulkanData
	{
		static constexpr uint32_t FramesInFlight = 2;

		std::vector<VkFence>          InFlightFences;
		std::vector<VkSemaphore>      RenderFinishedSemaphores;
		std::vector<VkSemaphore>      ImageAvailableSemaphores;
		std::vector<VkFramebuffer>    Framebuffers;

		std::vector<Ref<CommandBuffer>> CommandBuffers;
		uint32_t CurrentFrameIndex = 0;
	};

	static VulkanData s_Data;

	void VulkanRenderer::Initialize()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		// Synchronization
		const uint32_t framesInFlight = s_Data.FramesInFlight;
		s_Data.InFlightFences.resize(framesInFlight);
		s_Data.RenderFinishedSemaphores.resize(framesInFlight);
		s_Data.ImageAvailableSemaphores.resize(framesInFlight);

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			vkCreateFence(vkDevice, &fenceInfo, nullptr, &s_Data.InFlightFences[i]);
			vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &s_Data.RenderFinishedSemaphores[i]);
			vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &s_Data.ImageAvailableSemaphores[i]);
		}

		s_Data.CommandBuffers.resize(VulkanData::FramesInFlight);
		for (uint32_t i = 0; i < VulkanData::FramesInFlight; i++)
		{
			s_Data.CommandBuffers[i] = CommandBuffer::Create();
		}
	}

	void VulkanRenderer::Shutdown()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		for (uint32_t i = 0; i < s_Data.FramesInFlight; i++)
		{
			vkDestroyFence(vkDevice, s_Data.InFlightFences[i], nullptr);
			vkDestroySemaphore(vkDevice, s_Data.RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(vkDevice, s_Data.ImageAvailableSemaphores[i], nullptr);
		}
	}

	void VulkanRenderer::BeginFrame()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		VkFence waitFence = s_Data.InFlightFences[s_Data.CurrentFrameIndex];

		VkResult vkRes = vkWaitForFences(vkDevice, 1, &waitFence, VK_TRUE, UINT64_MAX);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not wait for fences");
		vkResetFences(vkDevice, 1, &waitFence);

		VulkanContext::Get().GetSwapChain()->AcquireNextImage();
	}

	void VulkanRenderer::Submit()
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = GetImageAvailableSemaphore();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		auto& activeCommandBuffer = Renderer::GetCommandBuffer();
		VkCommandBuffer commandBuffer = static_cast<VulkanCommandBuffer*>(activeCommandBuffer.get())->GetVkHandle();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphore = GetRenderFinishedSemaphore();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		vkQueueSubmit(VulkanContext::GetCurrentDevice()->GetGraphicsQueue(), 1, &submitInfo, GetInFlightFence());

		s_Data.CurrentFrameIndex = (s_Data.CurrentFrameIndex + 1) % VulkanData::FramesInFlight;
	}

	const Ref<CommandBuffer>& VulkanRenderer::GetCommandBuffer() const
	{
		return s_Data.CommandBuffers[s_Data.CurrentFrameIndex];
	}

	void VulkanRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		VulkanContext::Get().GetSwapChain()->Resize(viewportWidth, viewportHeight);

#if 0
		for (auto& framebuffer : s_Data.Framebuffers)
		{
			framebuffer->Resize(viewportWidth, viewportHeight);
		}
#endif
	}

	uint32_t VulkanRenderer::GetCurrentFrameIndex()
	{
		return s_Data.CurrentFrameIndex;
	}

	VkFence VulkanRenderer::GetInFlightFence()
	{
		return s_Data.InFlightFences[s_Data.CurrentFrameIndex];
	}

	VkSemaphore VulkanRenderer::GetRenderFinishedSemaphore()
	{
		return s_Data.RenderFinishedSemaphores[s_Data.CurrentFrameIndex];
	}

	VkSemaphore VulkanRenderer::GetImageAvailableSemaphore()
	{
		return s_Data.ImageAvailableSemaphores[s_Data.CurrentFrameIndex];
	}

	VkFramebuffer VulkanRenderer::GetActiveFramebuffer()
	{
		const uint32_t imageIndex = VulkanContext::Get().GetSwapChain()->GetCurrentImageIndex();
		return s_Data.Framebuffers[imageIndex];
	}
}
