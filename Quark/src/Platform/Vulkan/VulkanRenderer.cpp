#include "qkpch.h"
#include "VulkanRenderer.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "Quark/Renderer/Renderer.h"

namespace Quark {

	struct VulkanData
	{
		static constexpr uint32_t FramesInFlight = 2;

		std::vector<VkFence>      InFlightFences;
		std::vector<VkSemaphore>  RenderFinishedSemaphores;
		std::vector<VkSemaphore>  ImageAvailableSemaphores;
	};

	static VulkanData s_Data;

	void VulkanRenderer::Initialize()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		// Synchronization
		uint32_t framesInFlight = s_Data.FramesInFlight;
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

	void VulkanRenderer::WaitForFences()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		VkFence waitFence = s_Data.InFlightFences[Renderer::GetCurrentFrameIndex()];

		VkResult vkRes = vkWaitForFences(vkDevice, 1, &waitFence, VK_TRUE, UINT64_MAX);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not wait for fences");
		vkResetFences(vkDevice, 1, &waitFence);
	}

	void VulkanRenderer::WaitAndRender()
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = VulkanRenderer::GetImageAvailableSemaphore();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		auto& activeCommandBuffer = Renderer::GetCommandBuffer();
		VkCommandBuffer commandBuffer = static_cast<VulkanCommandBuffer*>(activeCommandBuffer.get())->GetVkHandle();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphore = VulkanRenderer::GetRenderFinishedSemaphore();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		vkQueueSubmit(VulkanContext::GetCurrentDevice()->GetGraphicsQueue(), 1, &submitInfo, VulkanRenderer::GetInFlightFence());
	}

	uint32_t VulkanRenderer::GetFramesInFlight()
	{
		return s_Data.FramesInFlight;
	}

	VkFence VulkanRenderer::GetInFlightFence()
	{
		return s_Data.InFlightFences[Renderer::GetCurrentFrameIndex()];
	}

	VkSemaphore VulkanRenderer::GetRenderFinishedSemaphore()
	{
		return s_Data.RenderFinishedSemaphores[Renderer::GetCurrentFrameIndex()];
	}

	VkSemaphore VulkanRenderer::GetImageAvailableSemaphore()
	{
		return s_Data.ImageAvailableSemaphores[Renderer::GetCurrentFrameIndex()];
	}
}
