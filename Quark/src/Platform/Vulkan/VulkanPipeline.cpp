#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	namespace Utils {

		static constexpr vk::Format ShaderDataTypeToVulkan(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:   return vk::Format::eR32Sfloat;
				case ShaderDataType::Float2:  return vk::Format::eR32G32Sfloat;
				case ShaderDataType::Float3:  return vk::Format::eR32G32B32Sfloat;
				case ShaderDataType::Float4:  return vk::Format::eR32G32B32A32Sfloat;
				case ShaderDataType::Double:  return vk::Format::eR64Sfloat;
				case ShaderDataType::Double2: return vk::Format::eR64G64Sfloat;
				case ShaderDataType::Double3: return vk::Format::eR64G64B64Sfloat;
				case ShaderDataType::Double4: return vk::Format::eR64G64B64A64Sfloat;

				// TODO: map matrices, int and bool types
				default:
					QK_CORE_FATAL("Unknown ShaderDataType");
					return vk::Format::eUndefined;
			}
		}

		static constexpr vk::CullModeFlagBits RenderCullModeToVulkan(RenderCullMode mode)
		{
			switch (mode)
			{
				case RenderCullMode::None:         return vk::CullModeFlagBits::eNone;
				case RenderCullMode::Front:        return vk::CullModeFlagBits::eFront;
				case RenderCullMode::Back:         return vk::CullModeFlagBits::eBack;
				case RenderCullMode::FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
				default:
					QK_CORE_FATAL("Unknown culling mode");
					return vk::CullModeFlagBits::eNone;
			}
		}

		static vk::VertexInputBindingDescription GetBindingDescription(const BufferLayout& layout)
		{
			vk::VertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 0;
			bindingDescription.stride = static_cast<uint32_t>(layout.GetStride());
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;

			return bindingDescription;
		}
	}

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec)
		: m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		m_DescriptorSets.resize(m_Spec.FramesInFlight);
		m_ImageAvailableSemaphores.resize(m_Spec.FramesInFlight);
		m_CameraUniformBuffers.resize(m_Spec.FramesInFlight);

		vk::SemaphoreCreateInfo semaphoreInfo;
		for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
		{
			m_ImageAvailableSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
			m_CameraUniformBuffers[i] = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
		}

		// Command buffers
		for (uint32_t i = 0; i < Renderer::FramesInFlight; i++)
		{
			m_CommandBuffers[i] = CommandBuffer::Create();
		}

		// Descriptor pool
		{
			vk::DescriptorPoolSize poolSize;
			poolSize.setType(vk::DescriptorType::eUniformBuffer);
			poolSize.setDescriptorCount(m_Spec.FramesInFlight);

			vk::DescriptorPoolCreateInfo poolInfo;
			poolInfo.setPoolSizeCount(1);
			poolInfo.setPPoolSizes(&poolSize);
			poolInfo.setMaxSets(m_Spec.FramesInFlight);

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
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			auto* layouts = static_cast<vk::DescriptorSetLayout*>(alloca(m_Spec.FramesInFlight * sizeof(vk::DescriptorSetLayout)));
			for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
				layouts[i] = m_DescriptorSetLayout;

 			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.setDescriptorPool(m_DescriptorPool);
			allocInfo.setDescriptorSetCount(m_Spec.FramesInFlight);
			allocInfo.setPSetLayouts(layouts);

			vkAllocateDescriptorSets(vkDevice, reinterpret_cast<VkDescriptorSetAllocateInfo*>(&allocInfo), reinterpret_cast<VkDescriptorSet*>(m_DescriptorSets.data()));

			for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
			{
				vk::DescriptorBufferInfo bufferInfo;
				bufferInfo.buffer = std::static_pointer_cast<VulkanUniformBuffer>(m_CameraUniformBuffers[i])->GetVkHandle();
				bufferInfo.offset = 0;
				bufferInfo.range = m_Spec.CameraUniformBufferSize;

				vk::WriteDescriptorSet descriptorWrite;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;

				vkDevice.updateDescriptorSets(descriptorWrite, nullptr);
			}
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
			vkDeviceWaitIdle(vkDevice);
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		for (auto& framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);

		for (auto& semaphore : m_ImageAvailableSemaphores)
			vkDestroySemaphore(vkDevice, semaphore, nullptr);

		vkDestroyPipeline(vkDevice, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vkDevice, m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(vkDevice, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(vkDevice, m_DescriptorPool, nullptr);
	}

	void VulkanPipeline::BeginFrame()
	{
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Spec.FramesInFlight;

		auto& swapChain = VulkanContext::GetSwapChain();
		m_NextImageIndex = swapChain.AcquireNextImageIndex(m_ImageAvailableSemaphores[m_CurrentFrameIndex]);

		m_ActiveCommandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(m_CommandBuffers[m_CurrentFrameIndex])->GetVkHandle();
		vkResetCommandBuffer(m_ActiveCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(m_ActiveCommandBuffer, &beginInfo);
		vkCmdBindPipeline(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		vkCmdBindDescriptorSets(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, reinterpret_cast<VkDescriptorSet*>(&m_DescriptorSets[m_CurrentFrameIndex]), 0, nullptr);
	}

	void VulkanPipeline::EndFrame()
	{
		vkEndCommandBuffer(m_ActiveCommandBuffer);
	}

	void VulkanPipeline::Submit()
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = m_ImageAvailableSemaphores[m_CurrentFrameIndex];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_ActiveCommandBuffer;

		auto& swapChain = VulkanContext::GetSwapChain();
		VkSemaphore signalSemaphore = swapChain.GetRenderFinishedSemaphore();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		vkQueueSubmit(VulkanContext::GetCurrentDevice().GetGraphicsQueue(), 1, &submitInfo, swapChain.GetFence());
	}

	void VulkanPipeline::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = std::static_pointer_cast<VulkanRenderPass>(renderPass)->GetVkHandle();
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[m_NextImageIndex];

		// must be size of framebuffer
		renderPassInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ m_Spec.ViewportWidth, m_Spec.ViewportHeight };

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_ActiveCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanPipeline::EndRenderPass()
	{
		vkCmdEndRenderPass(m_ActiveCommandBuffer);
	}

	void VulkanPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Spec.ViewportWidth = viewportWidth;
		m_Spec.ViewportHeight = viewportHeight;

		RecreateGraphicsPipeline();
		RecreateFramebuffers();
	}

	void VulkanPipeline::RecreateFramebuffers()
	{
		QK_PROFILE_FUNCTION();

		auto& swapChain = VulkanContext::GetSwapChain();
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		if (m_SwapChainFramebuffers.size() == 0)
		{
			m_SwapChainFramebuffers.resize(swapChain.GetSpecification().ImageCount);
		}
		else
		{
			for (auto& framebuffer : m_SwapChainFramebuffers)
				vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}

		for (uint32_t i = 0; i < swapChain.GetSpecification().ImageCount; i++)
		{
			vk::ImageView attachments[] = {
				swapChain.GetImageView(i)
			};

			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.setRenderPass(std::static_pointer_cast<VulkanRenderPass>(m_Spec.RenderPass)->GetVkHandle());
			framebufferInfo.setAttachmentCount(1);
			framebufferInfo.setPAttachments(attachments);
			framebufferInfo.setWidth(swapChain.GetSpecification().Width);
			framebufferInfo.setHeight(swapChain.GetSpecification().Height);
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

		// Vertex input 
		auto bindingDescription = Utils::GetBindingDescription(m_Spec.Layout);
		auto* attributeDescriptions = static_cast<vk::VertexInputAttributeDescription*>(alloca(m_Spec.Layout.GetCount() * sizeof(BufferElement)));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].format = Utils::ShaderDataTypeToVulkan(m_Spec.Layout[i].Type);
			attributeDescriptions[i].offset = static_cast<uint32_t>(m_Spec.Layout[i].Offset);
		}

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		vertexInputInfo.setVertexBindingDescriptionCount(1);
		vertexInputInfo.setVertexAttributeDescriptionCount(m_Spec.Layout.GetCount());
		vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
		vertexInputInfo.setPVertexAttributeDescriptions(attributeDescriptions);

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
		inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

		// Viewport
		vk::Viewport viewport;
		viewport.setX(0.0f);
		viewport.setY(0.0f);
		viewport.setWidth((float)m_Spec.ViewportWidth);
		viewport.setHeight((float)m_Spec.ViewportHeight);
		viewport.setMinDepth(0.0f);
		viewport.setMaxDepth(1.0f);

		vk::Rect2D scissor;
		scissor.setOffset({ 0, 0 });
		scissor.setExtent(vk::Extent2D{ m_Spec.ViewportWidth, m_Spec.ViewportHeight });

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.setViewportCount(1);
		viewportState.setPViewports(&viewport);
		viewportState.setScissorCount(1);
		viewportState.setPScissors(&scissor);

		// Rasterization
		vk::PipelineRasterizationStateCreateInfo rasterizer;
		rasterizer.setDepthClampEnable(VK_FALSE);
		rasterizer.setRasterizerDiscardEnable(VK_FALSE);
		rasterizer.setPolygonMode(vk::PolygonMode::eFill);
		rasterizer.setLineWidth(1.0f);
		rasterizer.setCullMode(Utils::RenderCullModeToVulkan(m_Spec.CullMode));
		rasterizer.setFrontFace(vk::FrontFace::eClockwise);
		rasterizer.setDepthBiasEnable(VK_FALSE);

		// Multisampling
		vk::PipelineMultisampleStateCreateInfo multisampling;
		multisampling.setSampleShadingEnable(VK_FALSE);
		multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		multisampling.setMinSampleShading(1.0f);
		multisampling.setAlphaToCoverageEnable(VK_FALSE);
		multisampling.setAlphaToOneEnable(VK_FALSE);

		// Blending
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

#if 0
		vk::DynamicState dynamicStates[] = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};

		vk::PipelineDynamicStateCreateInfo dynamicState;
		dynamicState.setDynamicStateCount(static_cast<uint32_t>(sizeof(dynamicStates)));
		dynamicState.setPDynamicStates(dynamicStates);
#endif

		// Pipeline creation
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.setSetLayoutCount(0);
		pipelineLayoutInfo.setPushConstantRangeCount(0);

		pipelineLayoutInfo.setSetLayoutCount(1);
		pipelineLayoutInfo.setPSetLayouts(&m_DescriptorSetLayout);

		m_PipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutInfo);

		vk::PipelineShaderStageCreateInfo shaderStages[] = {
			std::static_pointer_cast<VulkanShader>(m_Spec.VertexShader)->GetStageInfo(),
			std::static_pointer_cast<VulkanShader>(m_Spec.FragmentShader)->GetStageInfo()
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
		pipelineInfo.setRenderPass(std::static_pointer_cast<VulkanRenderPass>(m_Spec.RenderPass)->GetVkHandle());
		pipelineInfo.setSubpass(0);

		auto pipelineStatusPair = vkDevice.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
		QK_CORE_ASSERT(pipelineStatusPair.result == vk::Result::eSuccess, "Could not create the graphics pipeline");
		m_GraphicsPipeline = pipelineStatusPair.value;
	}
}
