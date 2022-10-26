#include "qkpch.h"
#include "VulkanContextBase.h"

#include "VulkanUtils.h"
#include "VulkanCommandBuffer.h"

///////////////////////////////////////
/// Vulkan API customizations
///
#define QK_VULKAN_DEBUG_UTILS_VERBOSE 0


#if QK_ASSERT_API_VALIDATION_ERRORS
#	define QK_VULKAN_ERROR_CALLBACK(message) QK_CORE_ASSERT(false, message)
#else
#	define QK_VULKAN_ERROR_CALLBACK(message) QK_CORE_ERROR(message)
#endif

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
#	define AssureDebugValidationLayerSupport ::Quark::AssureValidationLayerSupport
#	define CreateDebugUtilsMessengerEXT ::Quark::CreateVkDebugUtilsMessengerEXT
#	define DestroyDebugUtilsMessengerEXT ::Quark::DestroyVkDebugUtilsMessengerEXT
#else
#	define AssureDebugValidationLayerSupport(...)
#	define CreateDebugUtilsMessengerEXT(...) VK_SUCCESS
#	define DestroyDebugUtilsMessengerEXT(...)
#endif

#if QK_VULKAN_DEBUG_UTILS_VERBOSE
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
#else
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 0
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 0
#endif

namespace Quark {

	static void AssureValidationLayerSupport()
	{
		QK_PROFILE_FUNCTION();

		uint32_t layerCount;
		VkResult vkRes = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		AutoRelease<VkLayerProperties> availableLayers = StackAlloc(layerCount * sizeof(VkLayerProperties));
		vkRes = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		for (const char* layerName : g_ValidationLayers)
		{
			bool layerFound = false;
			for (size_t i = 0; i < layerCount; i++)
			{
				if (std::strcmp(layerName, availableLayers[i].layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			QK_CORE_ASSERT(layerFound, "Validation layer not found: {0}", layerName);
		}
	}

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
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   QK_VULKAN_ERROR_CALLBACK(pCallbackData->pMessage); return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);             return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    QK_CORE_INFO(pCallbackData->pMessage);             return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage);            return VK_FALSE;

			QK_ASSERT_NO_DEFAULT("Unknown severity level in Vulkan message callback");
		}

		return VK_FALSE;
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

		messengerCreateInfo.pfnUserCallback = VkDebugCallback;
		return messengerCreateInfo;
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

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	VulkanContextBase::VulkanContextBase()
		: m_Frames(Renderer::GetFramesInFlight())
	{
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
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		m_Device.reset();

		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContextBase::BeginFrame(uint32_t frameIndex)
	{
		VkFence waitFence = m_Frames[frameIndex].InFlightFence;
		vkWaitForFences(m_Device->GetVkHandle(), 1, &waitFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetVkHandle(), 1, &waitFence);

		VkResult res = m_SwapChain->AcquireNextImage(m_Frames[frameIndex].ImageAvailableSemaphore);
		m_SwapchainValid = res == VK_SUCCESS;
	}

	void VulkanContextBase::WaitUntilDeviceIdle()
	{
		m_Device->WaitUntilIdle();
	}

	void VulkanContextBase::Flush(CommandBuffer* commandBuffer, uint32_t frameIndex)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = m_Frames[frameIndex].ImageAvailableSemaphore;

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

		VkSemaphore signalSemaphore = m_Frames[frameIndex].RenderFinishedSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		VkQueue graphicsQueue = m_Device->GetGraphicsQueue();
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_Frames[frameIndex].InFlightFence);
	}

	void VulkanContextBase::SwapBuffers()
	{
		auto presentQueue = m_Device->GetPresentQueue();
		auto renderFinishedSemaphore = m_Frames[Renderer::GetCurrentFrameIndex()].RenderFinishedSemaphore;
		
		m_SwapChain->Present(presentQueue, renderFinishedSemaphore);
	}

	void VulkanContextBase::SetSwapInterval(int interval)
	{
	}

	void VulkanContextBase::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Device->WaitUntilIdle();
		m_SwapChain->Resize(viewportWidth, viewportHeight);
	}

	uint32_t VulkanContextBase::GetSwapChainImageCount() const
	{
		return m_SwapChain->GetImageCount();
	}

	FramebufferAttachment* VulkanContextBase::GetColorAttachment(uint32_t index) const
	{
		return m_SwapChain->GetColorAttachment(index);
	}

	uint32_t VulkanContextBase::GetCurrentImageIndex() const
	{
		return m_SwapChain->GetCurrentImageIndex();
	}

	void VulkanContextBase::CreateInstance(const char* appName, const std::vector<const char*>& extensions)
	{
		QK_PROFILE_FUNCTION();

		Utils::EnumerateVkExtensions();
		AssureDebugValidationLayerSupport();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Quark Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Quark Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		auto messengerCreateInfo = CreateVkDebugUtilsMessengerCreateInfoEXT();

		createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
		createInfo.ppEnabledLayerNames = g_ValidationLayers;
		createInfo.pNext = &messengerCreateInfo;
#endif
		VkResult vkRes = vkCreateInstance(&createInfo, nullptr, &m_Instance);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create the Vulkan instance (code: {0})", vkRes);

		vkRes = CreateDebugUtilsMessengerEXT(m_Instance, &messengerCreateInfo, nullptr, &m_DebugMessenger);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create a Vulkan debug messenger (code: {0})", vkRes);
	}

	VkSurfaceFormatKHR VulkanContextBase::ChooseSwapSurfaceFormat(VkSurfaceFormatKHR preferred)
	{
		auto& availableFormats = m_Device->GetSupportDetails().Formats;
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == preferred.format && availableFormat.colorSpace == preferred.colorSpace)
				return availableFormat;
		}

		QK_CORE_WARN("Swap surface format not found: fallback to default format 0");
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanContextBase::ChooseSwapPresentMode(VkPresentModeKHR preferred)
	{
		auto& availablePresentModes = m_Device->GetSupportDetails().PresentModes;
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == preferred)
				return availablePresentMode;
		}

		QK_CORE_WARN("Swap present mode not found: fallback to default FIFO present mode");
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanContextBase::ChooseSwapExtent(uint32_t width, uint32_t height)
	{
		auto& capabilities = m_Device->GetSupportDetails().Capabilities;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D extent = { width, height };
			extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return extent;
		}
	}

	uint32_t VulkanContextBase::GetSwapChainImageCount()
	{
		auto& supportDetails = m_Device->GetSupportDetails();
		uint32_t imageCount = supportDetails.Capabilities.minImageCount + 1;
		if (imageCount > supportDetails.Capabilities.maxImageCount)
			imageCount = supportDetails.Capabilities.maxImageCount;

		return imageCount;
	}
}
