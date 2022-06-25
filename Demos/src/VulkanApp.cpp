#include "VulkanApp.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

VulkanApp::VulkanApp()
	: m_VertShader(vk::ShaderStageFlagBits::eVertex, "../Quark/assets/shaders/version/4.50/bin/vert.spv"),
	m_FragShader(vk::ShaderStageFlagBits::eFragment, "../Quark/assets/shaders/version/4.50/bin/frag.spv")
{
	QK_PROFILE_FUNCTION();

	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	auto& device = VulkanGraphicsContext::GetCurrentDevice();

	RecreateGraphicsPipeline();
	RecreateFramebuffers();

	// Command pool and buffers
	{
		vk::CommandPoolCreateInfo poolInfo;
		poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		poolInfo.setQueueFamilyIndex(*device.GetQueueFamilyIndices().GraphicsFamily);

		m_VkCommandPool = device.GetVkHandle().createCommandPool(poolInfo);

		vk::CommandBufferAllocateInfo allocInfo;
		allocInfo.setCommandPool(m_VkCommandPool);
		allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
		allocInfo.setCommandBufferCount(g_FramesInFlight);

		m_VkCommandBuffers.resize(g_FramesInFlight);
		m_VkCommandBuffers = device.GetVkHandle().allocateCommandBuffers(allocInfo);
	}

	vk::FenceCreateInfo fenceInfo;
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

	m_VkInFlightFences.resize(g_FramesInFlight);
	for (uint32_t i = 0; i < g_FramesInFlight; i++)
		m_VkInFlightFences[i] = device.GetVkHandle().createFence(fenceInfo);
}

VulkanApp::~VulkanApp()
{
	QK_PROFILE_FUNCTION();

	auto vkDevice = VulkanGraphicsContext::GetCurrentDevice().GetVkHandle();

	{
		Profile::Timer t;
		vkDevice.waitIdle();
		QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
	}

	for (uint32_t i = 0; i < m_VkSwapChainFramebuffers.size(); i++)
		vkDevice.destroyFramebuffer(m_VkSwapChainFramebuffers[i]);

	for (uint32_t i = 0; i < m_VkInFlightFences.size(); i++)
		vkDevice.destroyFence(m_VkInFlightFences[i]);

	vkDevice.destroyPipeline(m_VkGraphicsPipeline);
	vkDevice.destroyPipelineLayout(m_VkPipelineLayout);
	vkDevice.destroyCommandPool(m_VkCommandPool);
	m_RenderPass.reset();
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));
}

void VulkanApp::OnRender()
{
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	auto vkDevice = VulkanGraphicsContext::GetCurrentDevice().GetVkHandle();

	vk::Result vkRes = vkDevice.waitForFences(m_VkInFlightFences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
	QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not wait for fences");
	vkDevice.resetFences(m_VkInFlightFences[m_CurrentFrameIndex]);

	uint32_t imageIndex = swapChain.AcquireNextImageIndex(m_CurrentFrameIndex);

	m_VkCommandBuffers[m_CurrentFrameIndex].reset();
	RecordCommandBuffer(m_VkCommandBuffers[m_CurrentFrameIndex], imageIndex);

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore waitSemaphores[] = { swapChain.GetImageAvailableSemaphore(m_CurrentFrameIndex) };

	vk::SubmitInfo submitInfo;
	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setPWaitSemaphores(waitSemaphores);
	submitInfo.setPWaitDstStageMask(waitStages);

	vk::CommandBuffer commandBuffers[] = { m_VkCommandBuffers[m_CurrentFrameIndex] };
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(commandBuffers);

	vk::Semaphore signalSemaphores[] = { swapChain.GetRenderFinishedSemaphore(m_CurrentFrameIndex) };
	submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setPSignalSemaphores(signalSemaphores);

	swapChain.GetPresentQueue().submit(submitInfo, m_VkInFlightFences[m_CurrentFrameIndex]);

	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % g_FramesInFlight;
}

bool VulkanApp::OnWindowResized(WindowResizedEvent& e)
{
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	auto vkDevice = VulkanGraphicsContext::GetCurrentDevice().GetVkHandle();

	{
		Profile::Timer t;
		vkDevice.waitIdle();
		QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
	}

	vkDevice.destroyPipeline(m_VkGraphicsPipeline);
	vkDevice.destroyPipelineLayout(m_VkPipelineLayout);

	swapChain.Resize(e.GetWidth(), e.GetHeight());
	RecreateGraphicsPipeline();
	RecreateFramebuffers();

	return false;
}

void VulkanApp::RecreateFramebuffers()
{
	QK_PROFILE_FUNCTION();

	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	auto vkDevice = VulkanGraphicsContext::GetCurrentDevice().GetVkHandle();

	uint32_t imageCount = swapChain.GetSpecification().ImageCount;
	vk::Extent2D extent = swapChain.GetSpecification().Extent;

	if (m_VkSwapChainFramebuffers.size() == 0)
	{
		m_VkSwapChainFramebuffers.resize(imageCount);
	}
	else
	{
		for (uint32_t i = 0; i < m_VkSwapChainFramebuffers.size(); i++)
			vkDevice.destroyFramebuffer(m_VkSwapChainFramebuffers[i]);
	}

	for (uint32_t i = 0; i < imageCount; i++)
	{
		vk::ImageView attachments[] = {
			swapChain.GetImageView(i)
		};

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.setRenderPass(m_RenderPass->GetNativeHandle());
		framebufferInfo.setAttachmentCount(1);
		framebufferInfo.setPAttachments(attachments);
		framebufferInfo.setWidth(extent.width);
		framebufferInfo.setHeight(extent.height);
		framebufferInfo.setLayers(1);

		m_VkSwapChainFramebuffers[i] = vkDevice.createFramebuffer(framebufferInfo);
	}
}

void VulkanApp::RecreateGraphicsPipeline()
{
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	auto vkDevice = VulkanGraphicsContext::GetCurrentDevice().GetVkHandle();

	// Render pass
	vk::Format swapChainImageFormat = swapChain.GetSpecification().SurfaceFormat.format;
	m_RenderPass = CreateScope<VulkanRenderPass>(swapChainImageFormat);

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptionCount(0);
	vertexInputInfo.setVertexAttributeDescriptionCount(0);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
	inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

	vk::Extent2D extent = swapChain.GetSpecification().Extent;

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
	/*multisampling.setMinSampleShading(1.0f);
	multisampling.setAlphaToCoverageEnable(VK_FALSE);
	multisampling.setAlphaToOneEnable(VK_FALSE);*/

	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	colorBlendAttachment.setBlendEnable(VK_FALSE);
	/*colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
	colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
	colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);*/

	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.setLogicOpEnable(VK_FALSE);
	colorBlending.setLogicOp(vk::LogicOp::eCopy);
	colorBlending.setAttachmentCount(1);
	colorBlending.setPAttachments(&colorBlendAttachment);

	/*vk::DynamicState dynamicStates[] = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eLineWidth
	};

	vk::PipelineDynamicStateCreateInfo dynamicState;
	dynamicState.setDynamicStateCount(static_cast<uint32_t>(sizeof(dynamicStates)));
	dynamicState.setPDynamicStates(dynamicStates);*/

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setSetLayoutCount(0);
	pipelineLayoutInfo.setPushConstantRangeCount(0);

	m_VkPipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutInfo);

	vk::PipelineShaderStageCreateInfo shaderStages[] = {
		m_VertShader.GetStageInfo(),
		m_FragShader.GetStageInfo()
	};

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.setStageCount(2);
	pipelineInfo.setPStages(shaderStages);

	pipelineInfo.setPVertexInputState(&vertexInputInfo);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);
	pipelineInfo.setPViewportState(&viewportState);
	pipelineInfo.setPRasterizationState(&rasterizer);
	pipelineInfo.setPMultisampleState(&multisampling);
	pipelineInfo.setPColorBlendState(&colorBlending);
	pipelineInfo.setLayout(m_VkPipelineLayout);
	pipelineInfo.setRenderPass(m_RenderPass->GetNativeHandle());
	pipelineInfo.setSubpass(0);

	auto pipelineStatusPair = vkDevice.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
	QK_CORE_ASSERT(pipelineStatusPair.result == vk::Result::eSuccess, "Could not create the graphics pipeline");
	m_VkGraphicsPipeline = pipelineStatusPair.value;
}

void VulkanApp::RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
	auto& device = VulkanGraphicsContext::GetCurrentDevice();
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();

	vk::CommandBufferBeginInfo beginInfo;
	commandBuffer.begin(beginInfo);

	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.setRenderPass(m_RenderPass->GetNativeHandle());
	renderPassInfo.setFramebuffer(m_VkSwapChainFramebuffers[imageIndex]);

	renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
	renderPassInfo.renderArea.extent = swapChain.GetSpecification().Extent;

	vk::ClearValue clearColor;
	vk::ClearColorValue colorValue;
	colorValue.setFloat32({ 1.0f, 0.0f, 0.0f, 1.0f });
	clearColor.setColor(colorValue);

	renderPassInfo.setClearValueCount(1);
	renderPassInfo.setPClearValues(&clearColor);

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_VkGraphicsPipeline);

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRenderPass();
	commandBuffer.end();
}
