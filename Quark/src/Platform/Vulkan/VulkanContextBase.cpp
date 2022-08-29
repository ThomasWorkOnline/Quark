#include "qkpch.h"
#include "VulkanContextBase.h"

#if defined(QK_DEBUG)
#	define CreateDebugUtilsMessengerEXT ::Quark::CreateVkDebugUtilsMessengerEXT
#	define DestroyDebugUtilsMessengerEXT ::Quark::DestroyVkDebugUtilsMessengerEXT
#else
#	define CreateDebugUtilsMessengerEXT(...) VK_SUCCESS
#	define DestroyDebugUtilsMessengerEXT(...)
#endif

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
#	define AssureDebugValidationLayerSupport ::Quark::AssureValidationLayerSupport
#else
#	define AssureDebugValidationLayerSupport(...)
#endif

#define QK_VULKAN_DEBUG_UTILS_VERBOSE 0

#if QK_VULKAN_DEBUG_UTILS_VERBOSE
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
#else
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 0
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 0
#endif

namespace Quark {

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			return VK_FALSE;

		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   QK_CORE_ASSERT(false, pCallbackData->pMessage); return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);          return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    QK_CORE_INFO(pCallbackData->pMessage);          return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage);         return VK_FALSE;

			QK_ASSERT_NO_DEFAULT("Unknown severity level in Vulkan message callback");
		}

		return VK_FALSE;
	}

	static void AssureValidationLayerSupport();

	static VkResult CreateVkDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyVkDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		QK_CORE_ASSERT(func != nullptr, "Could not load function vkDestroyDebugUtilsMessengerEXT");
		func(instance, debugMessenger, pAllocator);
	}

	static VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT()
	{
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
		messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerCreateInfo.messageSeverity =
			  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

		messengerCreateInfo.messageType =
			  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		messengerCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)VkDebugCallback;
		return messengerCreateInfo;
	}

	VulkanContextBase::~VulkanContextBase()
	{
		QK_PROFILE_FUNCTION();

		for (auto& data : m_Frames)
		{
			vkDestroyFence(m_Device->GetVkHandle(), data.InFlightFence, nullptr);
			vkDestroySemaphore(m_Device->GetVkHandle(), data.RenderFinishedSemaphore, nullptr);
			vkDestroySemaphore(m_Device->GetVkHandle(), data.ImageAvailableSemaphore, nullptr);
		}

		m_SwapChain.reset();
		m_Device.reset();

		DestroyDebugUtilsMessengerEXT(m_Instance, m_VkDebugMessenger, nullptr);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContextBase::StartFrame()
	{
		m_FrameCounterIndex = (m_FrameCounterIndex + 1) % FramesInFlight;
		m_CurrentFrameIndex = m_FrameCounterIndex;

		{
			VkFence waitFence = m_Frames[m_CurrentFrameIndex].InFlightFence;

			VkResult vkRes = vkWaitForFences(m_Device->GetVkHandle(), 1, &waitFence, VK_TRUE, UINT64_MAX);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not wait for fences");
			vkResetFences(m_Device->GetVkHandle(), 1, &waitFence);
		}
		
		m_SwapchainValid = m_SwapChain->AcquireNextImage(m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore);
	}

	void VulkanContextBase::WaitUntilIdle()
	{
		m_Device->WaitIdle();
	}

	void VulkanContextBase::Submit()
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = waitStages;

		if (m_SwapchainValid)
		{
			VkSemaphore waitSemaphores = m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphores;
		}

		VkCommandBuffer commandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer.GetVkHandle();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphore = m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_Frames[m_CurrentFrameIndex].InFlightFence);
	}

	void VulkanContextBase::SwapBuffers()
	{
		auto presentQueue = m_Device->GetPresentQueue();
		auto renderFinishedSemaphore = m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore;
		
		m_SwapChain->Present(presentQueue, renderFinishedSemaphore);
	}

	void VulkanContextBase::SetSwapInterval(int interval)
	{
	}

	void VulkanContextBase::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Device->WaitIdle();
		m_SwapChain->Resize(viewportWidth, viewportHeight);
	}

	uint32_t VulkanContextBase::GetSwapChainImageCount() const
	{
		return m_SwapChain->GetImageCount();
	}

	FramebufferAttachment* VulkanContextBase::GetColorAttachment(uint32_t index) const
	{
		return m_SwapChain->GetAttachment(index);
	}

	uint32_t VulkanContextBase::GetCurrentImageIndex() const
	{
		return m_SwapChain->GetCurrentImageIndex();
	}

	CommandBuffer* VulkanContextBase::GetCommandBuffer()
	{
		return &m_Frames[m_CurrentFrameIndex].CommandBuffer;
	}

	void VulkanContextBase::Init()
	{
		QK_PROFILE_FUNCTION();

		// Synchronization
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (auto& data : m_Frames)
		{
			vkCreateFence(m_Device->GetVkHandle(), &fenceInfo, nullptr, &data.InFlightFence);
			vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &data.RenderFinishedSemaphore);
			vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &data.ImageAvailableSemaphore);
		}

		for (uint32_t i = 0; i < FramesInFlight; i++)
		{
			m_Frames[i].CommandBuffer = { m_Device.get() };
		}

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	void VulkanContextBase::CreateInstance(VkInstanceCreateInfo& createInfo)
	{
		QK_PROFILE_FUNCTION();

		AssureDebugValidationLayerSupport();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		auto messengerCreateInfo = CreateVkDebugUtilsMessengerCreateInfoEXT();

		createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
		createInfo.ppEnabledLayerNames = g_ValidationLayers;
		createInfo.pNext = &messengerCreateInfo;
#endif
		VkResult vkRes = vkCreateInstance(&createInfo, nullptr, &m_Instance);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create the Vulkan instance");

		vkRes = CreateDebugUtilsMessengerEXT(m_Instance, &messengerCreateInfo, nullptr, &m_VkDebugMessenger);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create a Vulkan debug messenger");
	}

	void AssureValidationLayerSupport()
	{
		QK_PROFILE_FUNCTION();

		uint32_t layerCount;
		VkResult vkRes = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkRes = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		for (const char* layerName : g_ValidationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
			{
				if (std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			QK_CORE_ASSERT(layerFound, "Validation layer not found: {0}", layerName);
		}
	}
}
