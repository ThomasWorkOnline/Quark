#include "qkpch.h"
#include "VulkanPipeline.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	VulkanPipeline::VulkanPipeline()
		: m_VertShader(vk::ShaderStageFlagBits::eVertex, "../Quark/assets/shaders/version/4.50/bin/vert.spv"),
		m_FragShader(vk::ShaderStageFlagBits::eFragment, "../Quark/assets/shaders/version/4.50/bin/frag.spv")
	{
		QK_PROFILE_FUNCTION();

		auto& swapChain = VulkanContext::GetSwapChain();
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		m_ViewportWidth = swapChain.GetSpecification().Extent.width;
		m_ViewportHeight = swapChain.GetSpecification().Extent.height;

		m_InFlightFences.resize(g_FramesInFlight);
		m_ImageAvailableSemaphores.resize(g_FramesInFlight);
		m_CameraUniformBuffers.resize(g_FramesInFlight);

		vk::FenceCreateInfo fenceInfo;
		fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

		vk::SemaphoreCreateInfo semaphoreInfo;

		for (uint32_t i = 0; i < g_FramesInFlight; i++)
		{
			m_InFlightFences[i] = vkDevice.createFence(fenceInfo);
			m_ImageAvailableSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
			m_CameraUniformBuffers[i] = UniformBuffer::Create(sizeof(CameraBufferData), 0);
		}

		// Render pass
		{
			QK_PROFILE_FUNCTION();

			vk::Format swapChainImageFormat = swapChain.GetSpecification().SurfaceFormat.format;

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

			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			m_RenderPass = vkDevice.createRenderPass(renderPassInfo);
		}

		// Descriptor pool
		{
			vk::DescriptorPoolSize poolSize;
			poolSize.setType(vk::DescriptorType::eUniformBuffer);
			poolSize.setDescriptorCount(g_FramesInFlight);

			vk::DescriptorPoolCreateInfo poolInfo;
			poolInfo.setPoolSizeCount(1);
			poolInfo.setPPoolSizes(&poolSize);
			poolInfo.setMaxSets(g_FramesInFlight);

			m_DescriptorPool = vkDevice.createDescriptorPool(poolInfo);
		}

		// Descriptor set layout
		{
			vk::DescriptorSetLayoutBinding uboLayoutBinding;
			uboLayoutBinding.setBinding(0);
			uboLayoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
			uboLayoutBinding.setDescriptorCount(1);
			uboLayoutBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);

			vk::DescriptorSetLayoutCreateInfo layoutInfo;
			layoutInfo.setBindingCount(1);
			layoutInfo.setPBindings(&uboLayoutBinding);

			m_DescriptorSetLayout = vkDevice.createDescriptorSetLayout(layoutInfo);

			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.setDescriptorPool(m_DescriptorPool);
			allocInfo.setDescriptorSetCount(1);
			allocInfo.setPSetLayouts(&m_DescriptorSetLayout);

			m_DescriptorSet = vkDevice.allocateDescriptorSets(allocInfo)[0];
		}

		RecreateGraphicsPipeline();
		RecreateFramebuffers();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		{
			Timer t;
			vkDevice.waitIdle();
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		for (uint32_t i = 0; i < m_SwapChainFramebuffers.size(); i++)
		{
			vkDevice.destroyFramebuffer(m_SwapChainFramebuffers[i]);
		}

		for (uint32_t i = 0; i < g_FramesInFlight; i++)
		{
			vkDevice.destroyFence(m_InFlightFences[i]);
			vkDevice.destroySemaphore(m_ImageAvailableSemaphores[i]);
		}

		vkDevice.destroyPipeline(m_GraphicsPipeline);
		vkDevice.destroyPipelineLayout(m_PipelineLayout);
		vkDevice.destroyDescriptorSetLayout(m_DescriptorSetLayout);
		vkDevice.destroyDescriptorPool(m_DescriptorPool);

		vkDevice.destroyRenderPass(m_RenderPass);
	}

	void VulkanPipeline::Begin()
	{
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % g_FramesInFlight;

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		// Wait for last frame to be complete
		{
			vk::Result vkRes = vkDevice.waitForFences(m_InFlightFences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not wait for fences");
			vkDevice.resetFences(m_InFlightFences[m_CurrentFrameIndex]);
		}

		auto& swapChain = VulkanContext::GetSwapChain();
		m_NextImageIndex = swapChain.AcquireNextImageIndex(m_ImageAvailableSemaphores[m_CurrentFrameIndex]);

		m_ActiveCommandBuffer = VulkanContext::GetCurrentDevice().SwitchCommandBuffer(m_CurrentFrameIndex);
		m_ActiveCommandBuffer.reset();

		m_ActiveCameraUniformBuffer = m_CameraUniformBuffers[m_CurrentFrameIndex].get();

		vk::CommandBufferBeginInfo beginInfo;
		m_ActiveCommandBuffer.begin(beginInfo);
		m_ActiveCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline);
	}

	void VulkanPipeline::End()
	{
		m_ActiveCommandBuffer.end();
	}

	void VulkanPipeline::Submit()
	{
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::Semaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrameIndex] };

		vk::SubmitInfo submitInfo;
		submitInfo.setWaitSemaphoreCount(1);
		submitInfo.setPWaitSemaphores(waitSemaphores);
		submitInfo.setPWaitDstStageMask(waitStages);

		submitInfo.setCommandBufferCount(1);
		submitInfo.setPCommandBuffers(&m_ActiveCommandBuffer);

		auto& swapChain = VulkanContext::GetSwapChain();
		vk::Semaphore signalSemaphores[] = { swapChain.GetRenderFinishedSemaphore(m_CurrentFrameIndex) };
		submitInfo.setSignalSemaphoreCount(1);
		submitInfo.setPSignalSemaphores(signalSemaphores);

		VulkanContext::GetCurrentDevice().GetGraphicsQueue().submit(submitInfo, m_InFlightFences[m_CurrentFrameIndex]);
	}

	void VulkanPipeline::BeginRenderPass()
	{
		vk::RenderPassBeginInfo renderPassInfo;
		renderPassInfo.setRenderPass(m_RenderPass);
		renderPassInfo.setFramebuffer(m_SwapChainFramebuffers[m_NextImageIndex]);

		renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = vk::Extent2D{ m_ViewportWidth, m_ViewportHeight };

		vk::ClearValue clearColor;
		vk::ClearColorValue colorValue;
		colorValue.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
		clearColor.setColor(colorValue);

		renderPassInfo.setClearValueCount(1);
		renderPassInfo.setPClearValues(&clearColor);

		m_ActiveCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void VulkanPipeline::EndRenderPass()
	{
		m_ActiveCommandBuffer.endRenderPass();
	}

	void VulkanPipeline::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_ViewportWidth = viewportWidth;
		m_ViewportHeight = viewportHeight;

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		{
			Timer t;
			vkDevice.waitIdle();
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		VulkanContext::GetSwapChain().Resize(viewportWidth, viewportHeight);

		RecreateGraphicsPipeline();
		RecreateFramebuffers();
	}

	void VulkanPipeline::RecreateFramebuffers()
	{
		QK_PROFILE_FUNCTION();

		auto& swapChain = VulkanContext::GetSwapChain();
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		uint32_t imageCount = swapChain.GetSpecification().ImageCount;
		vk::Extent2D extent = swapChain.GetSpecification().Extent;

		if (m_SwapChainFramebuffers.size() == 0)
		{
			m_SwapChainFramebuffers.resize(imageCount);
		}
		else
		{
			for (uint32_t i = 0; i < m_SwapChainFramebuffers.size(); i++)
				vkDevice.destroyFramebuffer(m_SwapChainFramebuffers[i]);
		}

		for (uint32_t i = 0; i < imageCount; i++)
		{
			vk::ImageView attachments[] = {
				swapChain.GetImageView(i)
			};

			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.setRenderPass(m_RenderPass);
			framebufferInfo.setAttachmentCount(1);
			framebufferInfo.setPAttachments(attachments);
			framebufferInfo.setWidth(extent.width);
			framebufferInfo.setHeight(extent.height);
			framebufferInfo.setLayers(1);

			m_SwapChainFramebuffers[i] = vkDevice.createFramebuffer(framebufferInfo);
		}
	}

	void VulkanPipeline::RecreateGraphicsPipeline()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		if (m_GraphicsPipeline)
		{
			vkDevice.destroyPipeline(m_GraphicsPipeline);
			vkDevice.destroyPipelineLayout(m_PipelineLayout);
		}

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		vertexInputInfo.setVertexBindingDescriptionCount(1);
		vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
		vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
		vertexInputInfo.setPVertexAttributeDescriptions(attributeDescriptions.data());

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
		inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

		auto& swapChain = VulkanContext::GetSwapChain();
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

		pipelineLayoutInfo.setSetLayoutCount(1);
		pipelineLayoutInfo.setPSetLayouts(&m_DescriptorSetLayout);

		m_PipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutInfo);

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
		pipelineInfo.setLayout(m_PipelineLayout);
		pipelineInfo.setRenderPass(m_RenderPass);
		pipelineInfo.setSubpass(0);

		auto pipelineStatusPair = vkDevice.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
		QK_CORE_ASSERT(pipelineStatusPair.result == vk::Result::eSuccess, "Could not create the graphics pipeline");
		m_GraphicsPipeline = pipelineStatusPair.value;
	}
}
