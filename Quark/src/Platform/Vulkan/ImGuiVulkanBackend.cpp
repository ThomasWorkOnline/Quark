#include "qkpch.h"

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/ImGui/ImGuiBackend.h"

#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_vulkan.cpp>

namespace Quark {

	ImGuiBackend CreateImGuiVulkanBackend()
	{
		ImGuiBackend backend;
		backend.Init = [](void* windowHandle)
		{
			bool initialized = ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(windowHandle), true);
			QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

			auto* ctx = VulkanContext::Get();

			ImGui_ImplVulkan_InitInfo info{};
			info.Instance       = ctx->GetInstance();
			info.PhysicalDevice = ctx->GetCurrentDevice()->GetPhysicalDevice();
			info.Device         = ctx->GetCurrentDevice()->GetVkHandle();
			info.QueueFamily    = ctx->GetCurrentDevice()->GetQueueFamilyIndices().GraphicsFamily.value();
			info.Queue          = ctx->GetCurrentDevice()->GetGraphicsQueue();
			info.MinImageCount  = ctx->GetSwapChain()->GetImageCount();
			info.ImageCount     = info.MinImageCount;
			info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

			auto* renderPass = Renderer::GetRenderPass();
			auto* vulkanRenderPass = static_cast<const VulkanRenderPass*>(renderPass);

			initialized = ImGui_ImplVulkan_Init(&info, vulkanRenderPass->GetVkHandle());
			QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
		};

		backend.Shutdown = []()
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		};

		backend.NewFrame = []()
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		};

		backend.RenderDrawData = [](ImDrawData* drawData)
		{
			auto* commandBuffer = Renderer::GetCommandBuffer();
			auto* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

			ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandBuffer->GetVkHandle());

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		};

		return backend;
	}
}
