#include "qkpch.h"
#include "VulkanRenderer.h"

#include "VulkanContext.h"
#include "Quark/Renderer/Renderer.h"

namespace Quark {

	struct VulkanData
	{
		std::vector<VkFence>     InFlightFences;
		std::vector<VkSemaphore> RenderFinishedSemaphores;
		std::vector<VkSemaphore> ImageAvailableSemaphores;
	};

	static VulkanData s_Data;

	void VulkanRenderer::Initialize()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		// Synchronization
		uint32_t framesInFlight = Renderer::GetFramesInFlight();
		s_Data.InFlightFences.resize(framesInFlight);
		s_Data.RenderFinishedSemaphores.resize(framesInFlight);
		s_Data.ImageAvailableSemaphores.resize(framesInFlight);

		vk::FenceCreateInfo fenceInfo;
		fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

		vk::SemaphoreCreateInfo semaphoreInfo;
		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			s_Data.InFlightFences[i] = vkDevice.createFence(fenceInfo);
			s_Data.RenderFinishedSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
			s_Data.ImageAvailableSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
		}
	}

	void VulkanRenderer::Shutdown()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		for (uint32_t i = 0; i < Renderer::GetFramesInFlight(); i++)
		{
			vkDestroyFence(vkDevice, s_Data.InFlightFences[i], nullptr);
			vkDestroySemaphore(vkDevice, s_Data.RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(vkDevice, s_Data.ImageAvailableSemaphores[i], nullptr);
		}
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
