#include "qkpch.h"
#include "VulkanGraphicsContext.h"

#include <fstream>
#include <set>
#include <sstream>

#include "VulkanShader.h" // TODO: remove

namespace Quark {

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage); return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:	  QK_CORE_INFO(pCallbackData->pMessage);  return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);  return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:	  QK_CORE_ERROR(pCallbackData->pMessage); return VK_FALSE;
		}

		QK_CORE_FATAL("Unknown severity level in Vulkan message callback");
		return VK_FALSE;
	}

	namespace Utils {

		static vk::Result CreateVkDebugUtilsMessengerEXT(
			vk::Instance instance,
			const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const vk::AllocationCallbacks* pAllocator,
			vk::DebugUtilsMessengerEXT* pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");

			if (func != nullptr)
				return (vk::Result)func((VkInstance)instance, (const VkDebugUtilsMessengerCreateInfoEXT*)pCreateInfo, (const VkAllocationCallbacks*)pAllocator, (VkDebugUtilsMessengerEXT*)pDebugMessenger);
			else
				return vk::Result::eErrorExtensionNotPresent;
		}

		static void DestroyVkDebugUtilsMessengerEXT(vk::Instance instance,
			vk::DebugUtilsMessengerEXT debugMessenger,
			const vk::AllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");

			QK_CORE_ASSERT(func != nullptr, "Could not load function vkDestroyDebugUtilsMessengerEXT");
			func((VkInstance)instance, (VkDebugUtilsMessengerEXT)debugMessenger, (const VkAllocationCallbacks*)pAllocator);
		}

		static vk::DebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT()
		{
			vk::DebugUtilsMessengerCreateInfoEXT vkMessengerCreateInfo;
			vkMessengerCreateInfo.setMessageSeverity(
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
#if QK_VULKAN_VERBOSE_LOG
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
#endif
			);

			vkMessengerCreateInfo.setMessageType(
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
			);

			vkMessengerCreateInfo.setPfnUserCallback(VkDebugCallback);
			return vkMessengerCreateInfo;
		}
	}

	VulkanGraphicsContext::VulkanGraphicsContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		m_SwapChain.reset();
		vkDestroyCommandPool(m_VkDevice, m_VkCommandPool, nullptr);

		for (auto& framebuffer : m_VkSwapChainFramebuffers)
			vkDestroyFramebuffer(m_VkDevice, framebuffer, nullptr);

		vkDestroyPipeline(m_VkDevice, m_VkGraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_VkDevice, m_VkPipelineLayout, nullptr);
		vkDestroyRenderPass(m_VkDevice, m_VkRenderPass, nullptr);

		m_VkDevice.destroy();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		Utils::DestroyVkDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);
#endif
		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);

		m_VkInstance.destroy();
	}

	void VulkanGraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Instance creation
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::Instance);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			QK_CORE_ASSERT(Utils::CheckVkValidationLayerSupport(), "Some Vulkan validation layers are not supported");
#endif

			vk::ApplicationInfo vkAppInfo;
			vkAppInfo.setPApplicationName("Quark App");
			vkAppInfo.setPEngineName("Quark Engine");

			vk::InstanceCreateInfo vkCreateInfo;
			vkCreateInfo.setPApplicationInfo(&vkAppInfo);

			auto extensions = Utils::GetRequiredVkExtensions();
			vkCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
			vkCreateInfo.setPpEnabledExtensionNames(extensions.data());

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			auto vkMessengerCreateInfo = Utils::CreateVkDebugUtilsMessengerCreateInfoEXT();

			vkCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(g_ValidationLayers.size()));
			vkCreateInfo.setPpEnabledLayerNames(g_ValidationLayers.data());
			vkCreateInfo.setPNext(&vkMessengerCreateInfo);
#endif

			m_VkInstance = vk::createInstance(vkCreateInfo);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			vk::Result vkRes = Utils::CreateVkDebugUtilsMessengerEXT(m_VkInstance, &vkMessengerCreateInfo, nullptr, &m_VkDebugMessenger);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Failed to create a Vulkan debug messenger");
#endif
		}

		// Window surface creation
		glfwCreateWindowSurface(m_VkInstance, m_WindowHandle, nullptr, (vk::SurfaceKHR::CType*)&m_VkSurface);

		// Device creation
		Utils::QueueFamilyIndices queueFamilyIndices;

		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::Device);

			uint32_t deviceCount = 0;
			vk::Result vkRes = m_VkInstance.enumeratePhysicalDevices(&deviceCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");
			QK_CORE_ASSERT(deviceCount, "No physical graphics devices found");

			std::vector<vk::PhysicalDevice> devices(deviceCount);
			vkRes = m_VkInstance.enumeratePhysicalDevices(&deviceCount, devices.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");

			for (const auto& device : devices)
			{
				auto indices = Utils::FindVkQueueFamilies(device, m_VkSurface);
				if (Utils::IsVkSwapChainSuitable(device, m_VkSurface, indices))
				{
					m_VkPhysicalDevice = device;
					queueFamilyIndices = indices;
					break;
				}
			}

			QK_CORE_ASSERT(m_VkPhysicalDevice, "Failed to find a suitable graphics device");

			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = {
				*queueFamilyIndices.GraphicsFamily,
				*queueFamilyIndices.PresentFamily
			};

			float queuePriorities = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				vk::DeviceQueueCreateInfo queueCreateInfo;
				queueCreateInfo.setQueueFamilyIndex(queueFamily);
				queueCreateInfo.setQueueCount(1);
				queueCreateInfo.setPQueuePriorities(&queuePriorities);
				queueCreateInfos.push_back(queueCreateInfo);
			}

			vk::PhysicalDeviceFeatures deviceFeatures;

			vk::DeviceCreateInfo createInfo;
			createInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
			createInfo.setPQueueCreateInfos(queueCreateInfos.data());
			createInfo.setPEnabledFeatures(&deviceFeatures);

			createInfo.setEnabledExtensionCount(static_cast<uint32_t>(g_DeviceExtensions.size()));
			createInfo.setPpEnabledExtensionNames(g_DeviceExtensions.data());

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			createInfo.setEnabledLayerCount(static_cast<uint32_t>(g_ValidationLayers.size()));
			createInfo.setPpEnabledLayerNames(g_ValidationLayers.data());
#endif
			m_VkDevice = m_VkPhysicalDevice.createDevice(createInfo, nullptr);
		}

		// Swap chain creation
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::SwapChain);

			Utils::SwapChainSupportDetails swapChainSupport = Utils::QuerySwapChainSupport(m_VkPhysicalDevice, m_VkSurface);

			uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
			if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
				imageCount = swapChainSupport.Capabilities.maxImageCount;

			VulkanSwapChainSpecification scSpec;
			scSpec.Extent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, m_WindowHandle);
			scSpec.SurfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
			scSpec.PresentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
			scSpec.FamilyIndices = queueFamilyIndices;
			scSpec.ImageCount = imageCount;

			m_SwapChain = CreateScope<VulkanSwapChain>(m_VkDevice, m_VkSurface, scSpec);
			m_VkPresentQueue = m_VkDevice.getQueue(*queueFamilyIndices.PresentFamily, 0);
		}

		// Render pass
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::RenderPass);

			vk::Format swapChainImageFormat = m_SwapChain->GetSpecification().SurfaceFormat.format;

			vk::AttachmentDescription colorAttachment;
			colorAttachment.setFormat(swapChainImageFormat);
			colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
			colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
			colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

			colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
			colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

			vk::AttachmentReference colorAttachmentRef;
			colorAttachmentRef.setAttachment(0);
			colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

			vk::SubpassDescription subpass;
			subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
			subpass.setColorAttachmentCount(1);
			subpass.setPColorAttachments(&colorAttachmentRef);

			vk::SubpassDependency dependency;
			dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
			dependency.setDstSubpass(0);
			dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
			dependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
			dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
			dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

			vk::RenderPassCreateInfo renderPassInfo;
			renderPassInfo.setAttachmentCount(1);
			renderPassInfo.setPAttachments(&colorAttachment);
			renderPassInfo.setSubpassCount(1);
			renderPassInfo.setPSubpasses(&subpass);
			renderPassInfo.setDependencyCount(1);
			renderPassInfo.setPDependencies(&dependency);

			m_VkRenderPass = m_VkDevice.createRenderPass(renderPassInfo, nullptr);
		}

		// Create graphics pipeline
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::Pipeline);

			VulkanShader vertShader = { m_VkDevice, vk::ShaderStageFlagBits::eVertex, "../Quark/assets/shaders/version/4.50/bin/vert.spv" };
			VulkanShader fragShader = { m_VkDevice, vk::ShaderStageFlagBits::eFragment, "../Quark/assets/shaders/version/4.50/bin/frag.spv" };

			vk::PipelineShaderStageCreateInfo shaderStages[] = {
				vertShader.GetStageInfo(),
				fragShader.GetStageInfo()
			};

			vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
			vertexInputInfo.setVertexBindingDescriptionCount(0);
			vertexInputInfo.setVertexAttributeDescriptionCount(0);

			vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
			inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
			inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

			vk::Extent2D extent = m_SwapChain->GetSpecification().Extent;

			vk::Viewport viewport;
			viewport.setX(0.0f);
			viewport.setY(0.0f);
			viewport.setWidth((float)extent.width);
			viewport.setHeight((float)extent.height);
			viewport.setMinDepth(0.0f);
			viewport.setMaxDepth(1.0f);

			vk::Rect2D scissor;
			scissor.setOffset({ 0, 0 });
			scissor.setExtent(extent);

			vk::PipelineViewportStateCreateInfo viewportState;
			viewportState.setViewportCount(1);
			viewportState.setPViewports(&viewport);
			viewportState.setScissorCount(1);
			viewportState.setPScissors(&scissor);

			vk::PipelineRasterizationStateCreateInfo rasterizer;
			rasterizer.setDepthClampEnable(VK_FALSE);
			rasterizer.setRasterizerDiscardEnable(VK_FALSE);
			rasterizer.setPolygonMode(vk::PolygonMode::eFill);
			rasterizer.setLineWidth(1.0f);
			rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
			rasterizer.setFrontFace(vk::FrontFace::eClockwise);
			rasterizer.setDepthBiasEnable(VK_FALSE);

			vk::PipelineMultisampleStateCreateInfo multisampling;
			multisampling.setSampleShadingEnable(VK_FALSE);
			multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
			multisampling.setMinSampleShading(1.0f);
			multisampling.setAlphaToCoverageEnable(VK_FALSE);
			multisampling.setAlphaToOneEnable(VK_FALSE);

			vk::PipelineColorBlendAttachmentState colorBlendAttachment;
			colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
			colorBlendAttachment.setBlendEnable(VK_TRUE);
			colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
			colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
			colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
			colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
			colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
			colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

			vk::PipelineColorBlendStateCreateInfo colorBlending;
			colorBlending.setLogicOpEnable(VK_FALSE);
			colorBlending.setLogicOp(vk::LogicOp::eCopy);
			colorBlending.setAttachmentCount(1);
			colorBlending.setPAttachments(&colorBlendAttachment);

			vk::DynamicState dynamicStates[] = {
				vk::DynamicState::eViewport,
				vk::DynamicState::eLineWidth
			};

			vk::PipelineDynamicStateCreateInfo dynamicState;
			dynamicState.setDynamicStateCount(static_cast<uint32_t>(sizeof(dynamicStates)));
			dynamicState.setPDynamicStates(dynamicStates);

			vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
			pipelineLayoutInfo.setSetLayoutCount(0);
			pipelineLayoutInfo.setPSetLayouts(nullptr);
			pipelineLayoutInfo.setPushConstantRangeCount(0);
			pipelineLayoutInfo.setPPushConstantRanges(nullptr);

			m_VkPipelineLayout = m_VkDevice.createPipelineLayout(pipelineLayoutInfo, nullptr);

			vk::GraphicsPipelineCreateInfo pipelineInfo;
			pipelineInfo.setStageCount(2);
			pipelineInfo.setPStages(shaderStages);

			pipelineInfo.setPVertexInputState(&vertexInputInfo);
			pipelineInfo.setPInputAssemblyState(&inputAssembly);
			pipelineInfo.setPViewportState(&viewportState);
			pipelineInfo.setPRasterizationState(&rasterizer);
			pipelineInfo.setPMultisampleState(&multisampling);
			pipelineInfo.setPDepthStencilState(nullptr);
			pipelineInfo.setPColorBlendState(&colorBlending);
			pipelineInfo.setPDynamicState(nullptr);

			pipelineInfo.setLayout(m_VkPipelineLayout);

			pipelineInfo.setRenderPass(m_VkRenderPass);
			pipelineInfo.setSubpass(0);

			auto pipelineStatusPair = m_VkDevice.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo, nullptr);
			QK_CORE_ASSERT(pipelineStatusPair.result == vk::Result::eSuccess, "Could not create the graphics pipeline");
			m_VkGraphicsPipeline = pipelineStatusPair.value;
		}

		// Framebuffer
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::Framebuffer);

			uint32_t imageCount = m_SwapChain->GetSpecification().ImageCount;
			vk::Extent2D extent = m_SwapChain->GetSpecification().Extent;

			m_VkSwapChainFramebuffers.resize(imageCount);
			for (size_t i = 0; i < imageCount; i++)
			{
				vk::ImageView attachments[] = {
					m_SwapChain->GetImageView(i)
				};

				vk::FramebufferCreateInfo framebufferInfo;
				framebufferInfo.renderPass = m_VkRenderPass;
				framebufferInfo.setAttachmentCount(1);
				framebufferInfo.setPAttachments(attachments);
				framebufferInfo.setWidth(extent.width);
				framebufferInfo.setHeight(extent.height);
				framebufferInfo.setLayers(1);

				m_VkSwapChainFramebuffers[i] = m_VkDevice.createFramebuffer(framebufferInfo, nullptr);
			}
		}

		// Command Pools and Buffers
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::CommandBuffer);

			vk::CommandPoolCreateInfo poolInfo;
			poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
			poolInfo.setQueueFamilyIndex(*queueFamilyIndices.GraphicsFamily);

			m_VkCommandPool = m_VkDevice.createCommandPool(poolInfo, nullptr);

			vk::CommandBufferAllocateInfo allocInfo;
			allocInfo.setCommandPool(m_VkCommandPool);
			allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
			allocInfo.setCommandBufferCount(1);

			m_VkCommandBuffer = m_VkDevice.allocateCommandBuffers(allocInfo)[0];
		}

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	void VulkanGraphicsContext::SwapBuffers()
	{
		m_SwapChain->SwapBuffers();

		uint32_t imageIndex = m_SwapChain->GetNextImageIndex();
		m_VkCommandBuffer.reset();
		RecordCommandBuffer(m_VkCommandBuffer, imageIndex);

		{
			vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

			vk::SubmitInfo submitInfo;
			submitInfo.setWaitSemaphoreCount(1);
			submitInfo.setPWaitSemaphores(&m_SwapChain->GetImageAvailableSemaphore());
			submitInfo.setPWaitDstStageMask(waitStages);

			submitInfo.setCommandBufferCount(1);
			submitInfo.setPCommandBuffers(&m_VkCommandBuffer);

			submitInfo.setSignalSemaphoreCount(1);
			submitInfo.setPSignalSemaphores(&m_SwapChain->GetRenderFinishedSemaphore());

			m_VkPresentQueue.submit(submitInfo, m_SwapChain->GetInFlightFence());
			m_SwapChain->Present(m_VkPresentQueue);
		}
	}

	void VulkanGraphicsContext::RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
	{
		vk::CommandBufferBeginInfo beginInfo;
		commandBuffer.begin(beginInfo);

		vk::RenderPassBeginInfo renderPassInfo;
		renderPassInfo.setRenderPass(m_VkRenderPass);
		renderPassInfo.setFramebuffer(m_VkSwapChainFramebuffers[imageIndex]);

		renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->GetSpecification().Extent;

		vk::ClearValue clearColor;
		vk::ClearColorValue colorValue;
		colorValue.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
		clearColor.setColor(colorValue);

		renderPassInfo.setClearValueCount(1);
		renderPassInfo.setPClearValues(&clearColor);

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_VkGraphicsPipeline);

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRenderPass();
		commandBuffer.end();
	}
}
