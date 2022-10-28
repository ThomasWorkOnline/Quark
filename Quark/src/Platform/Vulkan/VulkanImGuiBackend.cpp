#include "qkpch.h"
#include "VulkanImGuiBackend.h"

#include "VulkanContext.h"
#include "VulkanEnums.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"

#include <GLFW/glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.cpp>

namespace Quark {

	VulkanImGuiBackend::VulkanImGuiBackend(void* windowHandle)
		: m_Device(VulkanContext::Get()->GetCurrentDevice()), m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
	}

	VulkanImGuiBackend::~VulkanImGuiBackend()
	{
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		vkDestroyDescriptorPool(m_Device->GetVkHandle(), m_DescriptorPool, nullptr);
	}

	void VulkanImGuiBackend::Init(RenderPass* renderPass, CommandBuffer* commandBuffer)
	{
		bool initialized = ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
		QK_CORE_ASSERT(initialized, "ImGui_ImplGlfw_InitForVulkan() failed!");

		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.maxSets = 1;
		createInfo.poolSizeCount = 1;
		createInfo.pPoolSizes = &poolSize;
		vkCreateDescriptorPool(m_Device->GetVkHandle(), &createInfo, nullptr, &m_DescriptorPool);

		auto* ctx = VulkanContext::Get();

		ImGui_ImplVulkan_InitInfo info{};
		info.Instance       = ctx->GetInstance();
		info.PhysicalDevice = m_Device->GetPhysicalDevice();
		info.Device         = m_Device->GetVkHandle();
		info.QueueFamily    = m_Device->GetQueueFamilyIndices().GraphicsFamily.value();
		info.Queue          = m_Device->GetGraphicsQueue();
		info.DescriptorPool = m_DescriptorPool;
		info.MinImageCount  = ctx->GetSwapChain()->GetImageCount();
		info.ImageCount     = info.MinImageCount;
		info.MSAASamples    = SampleCountToVulkan(renderPass->GetSpecification().Samples);

		auto vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass)->GetVkHandle();
		initialized = ImGui_ImplVulkan_Init(&info, vulkanRenderPass);
		QK_CORE_ASSERT(initialized, "ImGui_ImplVulkan_Init() failed!");

		auto vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkHandle();
		vkResetCommandBuffer(vulkanCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(vulkanCommandBuffer, &beginInfo);

		initialized = ImGui_ImplVulkan_CreateFontsTexture(vulkanCommandBuffer);
		QK_CORE_ASSERT(initialized, "ImGui_ImplVulkan_CreateFontsTexture() failed!");

		vkEndCommandBuffer(vulkanCommandBuffer);
	}
		
	void VulkanImGuiBackend::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void VulkanImGuiBackend::RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData)
	{
		auto vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkHandle();
		ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandBuffer);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}
