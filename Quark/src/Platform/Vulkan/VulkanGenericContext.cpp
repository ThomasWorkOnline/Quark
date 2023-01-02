#include "qkpch.h"
#include "VulkanGenericContext.h"

#include "VulkanCommandBuffer.h"

#include <GLFW/glfw3.h>

namespace Quark {

	namespace Utils {

		static std::vector<const char*> GetRequiredVkExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
			return extensions;
		}
	}

	VulkanGenericContext::VulkanGenericContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");

		auto extensions = Utils::GetRequiredVkExtensions();
		CreateInstance("Quark App", extensions);

		glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
		m_Device = VulkanDevice::CreateDefaultForSurface(m_Instance, m_Surface);

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	VulkanGenericContext::~VulkanGenericContext()
	{
		QK_PROFILE_FUNCTION();

		m_SwapChain.reset();
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		for (auto& frame : m_Frames)
		{
			vkDestroyFence(m_Device->GetVkHandle(), frame.InFlightFence, nullptr);
			vkDestroySemaphore(m_Device->GetVkHandle(), frame.RenderFinishedSemaphore, nullptr);
			vkDestroySemaphore(m_Device->GetVkHandle(), frame.ImageAvailableSemaphore, nullptr);
		}
	}

	void VulkanGenericContext::Init(const SwapChainSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		m_SwapChain = CreateScope<VulkanSwapChain>(m_Device.get(), m_Surface, spec);

		const uint32_t framesInFlight = std::min(m_SwapChain->GetBufferCount(), 1u);
		m_Frames.resize(framesInFlight);

		// Synchronization
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (auto& frame : m_Frames)
		{
			vkCreateFence(m_Device->GetVkHandle(), &fenceInfo, nullptr, &frame.InFlightFence);
			vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &frame.RenderFinishedSemaphore);
			vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &frame.ImageAvailableSemaphore);
		}
	}

	void VulkanGenericContext::BeginFrame(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;

		VkFence waitFence = m_Frames[m_CurrentFrameIndex].InFlightFence;
		vkWaitForFences(m_Device->GetVkHandle(), 1, &waitFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetVkHandle(), 1, &waitFence);

		VkResult res = m_SwapChain->AcquireNextImage(m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore);
		m_SwapchainValid = res == VK_SUCCESS;
	}

	void VulkanGenericContext::Flush(CommandBuffer* commandBuffer)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = waitStages;

		if (m_SwapchainValid)
		{
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphores;
		}

		VkCommandBuffer vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkHandle();

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vulkanCommandBuffer;

		VkSemaphore signalSemaphore = m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		VkQueue graphicsQueue = m_Device->GetGraphicsQueue();
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_Frames[m_CurrentFrameIndex].InFlightFence);
	}

	void VulkanGenericContext::SwapBuffers()
	{
		auto presentQueue = m_Device->GetPresentQueue();
		auto renderFinishedSemaphore = m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore;

		m_SwapChain->Present(presentQueue, renderFinishedSemaphore);
	}

	void VulkanGenericContext::SetSwapInterval(int interval)
	{
		// TODO: check if swap chain supports Mailbox (unlocked)
		//m_SwapChain->SetPresentMode(interval ? SwapPresentMode::FIFO : SwapPresentMode::Mailbox);
	}

	void VulkanGenericContext::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Device->WaitUntilIdle();
		m_SwapChain->Resize(viewportWidth, viewportHeight);
	}

	SwapChain* VulkanGenericContext::GetSwapChain()
	{
		return m_SwapChain.get();
	}

	ViewportExtent VulkanGenericContext::QuerySwapExtent() const
	{
		auto& capabilities = m_Device->GetSupportDetails().Capabilities;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return { capabilities.currentExtent.width, capabilities.currentExtent.height };
		}
		else
		{
			int framebufferWidth, framebufferHeight;
			glfwGetFramebufferSize(m_WindowHandle, &framebufferWidth, &framebufferHeight);

			ViewportExtent extent = { (uint32_t)framebufferWidth, (uint32_t)framebufferHeight };
			extent.Width = std::clamp(extent.Width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			extent.Height = std::clamp(extent.Height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return extent;
		}
	}
}
