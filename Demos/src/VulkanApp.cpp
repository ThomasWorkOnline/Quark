#include "VulkanApp.h"

#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

VulkanApp::VulkanApp()
{
	QK_PROFILE_FUNCTION();

	auto& device = VulkanGraphicsContext::GetCurrentDevice();
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();

	// Render pass
	vk::Format swapChainImageFormat = swapChain.GetSpecification().SurfaceFormat.format;
	m_RenderPass = CreateScope<VulkanRenderPass>(device.GetVkHandle(), swapChainImageFormat);

	// Create graphics pipeline
	{
		VulkanShader vertShader = { vk::ShaderStageFlagBits::eVertex, "../Quark/assets/shaders/version/4.50/bin/vert.spv"};
		VulkanShader fragShader = { vk::ShaderStageFlagBits::eFragment, "../Quark/assets/shaders/version/4.50/bin/frag.spv" };

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

		m_VkPipelineLayout = device.GetVkHandle().createPipelineLayout(pipelineLayoutInfo, nullptr);

		vk::PipelineShaderStageCreateInfo shaderStages[] = {
			vertShader.GetStageInfo(),
			fragShader.GetStageInfo()
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

		auto pipelineStatusPair = device.GetVkHandle().createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo, nullptr);
		QK_CORE_ASSERT(pipelineStatusPair.result == vk::Result::eSuccess, "Could not create the graphics pipeline");
		m_VkGraphicsPipeline = pipelineStatusPair.value;
	}

	// Framebuffer
	{
		uint32_t imageCount = swapChain.GetSpecification().ImageCount;
		vk::Extent2D extent = swapChain.GetSpecification().Extent;

		m_VkSwapChainFramebuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; i++)
		{
			vk::ImageView attachments[] = {
				swapChain.GetImageView(i)
			};

			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.renderPass = m_RenderPass->GetNativeHandle();
			framebufferInfo.setAttachmentCount(1);
			framebufferInfo.setPAttachments(attachments);
			framebufferInfo.setWidth(extent.width);
			framebufferInfo.setHeight(extent.height);
			framebufferInfo.setLayers(1);

			m_VkSwapChainFramebuffers[i] = device.GetVkHandle().createFramebuffer(framebufferInfo, nullptr);
		}
	}

	vk::FenceCreateInfo fenceInfo;
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	m_VkInFlightFence = device.GetVkHandle().createFence(fenceInfo, nullptr);
}

VulkanApp::~VulkanApp()
{
	QK_PROFILE_FUNCTION();

	auto& device = VulkanGraphicsContext::GetCurrentDevice();

	// Wait until all operations on the swap chain are completed
	device.GetVkHandle().waitIdle();
	vkDestroyFence(device.GetVkHandle(), m_VkInFlightFence, nullptr);

	for (auto& framebuffer : m_VkSwapChainFramebuffers)
		vkDestroyFramebuffer(device.GetVkHandle(), framebuffer, nullptr);

	m_RenderPass.reset();
	vkDestroyPipeline(device.GetVkHandle(), m_VkGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device.GetVkHandle(), m_VkPipelineLayout, nullptr);
}

void VulkanApp::OnRender()
{
	auto& device = VulkanGraphicsContext::GetCurrentDevice();
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();

	vk::Result vkRes = device.GetVkHandle().waitForFences(m_VkInFlightFence, VK_TRUE, UINT64_MAX);
	QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not wait for fences");
	device.GetVkHandle().resetFences(m_VkInFlightFence);

	uint32_t nextImageIndex = swapChain.AcquireNextImageIndex();

	device.GetCommandBuffer().reset();
	RecordCommandBuffer(nextImageIndex);

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::SubmitInfo submitInfo;
	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setPWaitSemaphores(&swapChain.GetImageAvailableSemaphore());
	submitInfo.setPWaitDstStageMask(waitStages);

	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&device.GetCommandBuffer());

	submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setPSignalSemaphores(&swapChain.GetRenderFinishedSemaphore());

	swapChain.GetPresentQueue().submit(submitInfo, m_VkInFlightFence);
}

void VulkanApp::RecordCommandBuffer(uint32_t imageIndex)
{
	auto& device = VulkanGraphicsContext::GetCurrentDevice();
	auto& swapChain = VulkanGraphicsContext::Get().GetSwapChain();
	
	auto commandBuffer = device.GetCommandBuffer();

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
