#include "qkpch.h"

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/ImGui/ImGuiBackend.h"

#include "Vulkan.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"

#include <GLFW/glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.cpp>

namespace Quark {

	class VulkanImGuiBackend final : public ImGuiBackend
	{
	public:
		VulkanImGuiBackend(VulkanDevice* device, VkRenderPass renderPass)
			: m_Device(device), m_RenderPass(renderPass)
		{
		}

		virtual void Init(void* windowHandle) final override
		{
			bool initialized = ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(windowHandle), true);
			QK_CORE_ASSERT(initialized, "ImGui_ImplGlfw_InitForVulkan() failed!");

			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.maxSets = 1;
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
			info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

			initialized = ImGui_ImplVulkan_Init(&info, m_RenderPass);
			QK_CORE_ASSERT(initialized, "ImGui_ImplVulkan_Init() failed!");

			auto* commandBuffer = Renderer::GetCommandBuffer();
			auto* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkHandle();

			vkResetCommandBuffer(vulkanCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vkBeginCommandBuffer(vulkanCommandBuffer, &beginInfo);

			initialized = ImGui_ImplVulkan_CreateFontsTexture(vulkanCommandBuffer);
			QK_CORE_ASSERT(initialized, "ImGui_ImplVulkan_CreateFontsTexture() failed!");

			vkEndCommandBuffer(vulkanCommandBuffer);
		}

		virtual ~VulkanImGuiBackend() final override
		{
			ImGui_ImplVulkan_DestroyFontUploadObjects();
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();

			vkDestroyDescriptorPool(m_Device->GetVkHandle(), m_DescriptorPool, nullptr);
		}
		
		virtual void NewFrame() final override
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}

		virtual void RenderDrawData(ImDrawData* drawData) final override
		{
			auto* commandBuffer = Renderer::GetCommandBuffer();
			auto* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkHandle();

			ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandBuffer);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

	private:
		VulkanDevice* m_Device;
		VkRenderPass m_RenderPass = nullptr;
		VkDescriptorPool m_DescriptorPool = nullptr;
	};

	Scope<ImGuiBackend> CreateImGuiVulkanBackend()
	{
		auto* ctx = VulkanContext::Get();

		auto* renderPass = Renderer::GetRenderPass();
		auto* vulkanRenderPass = static_cast<const VulkanRenderPass*>(renderPass);

		return CreateScope<VulkanImGuiBackend>(ctx->GetCurrentDevice(), vulkanRenderPass->GetVkHandle());
	}
}
