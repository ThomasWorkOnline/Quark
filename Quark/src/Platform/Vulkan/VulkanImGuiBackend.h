#pragma once

#include "Quark/ImGui/ImGuiBackend.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanImGuiBackend final : public ImGuiBackend
	{
	public:
		VulkanImGuiBackend(void* windowHandle);
		virtual ~VulkanImGuiBackend() final override;

		virtual void Init(RenderPass* renderPass, CommandBuffer* commandBuffer) final override;
		virtual void NewFrame() final override;
		virtual void RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData) final override;

	private:
		VulkanDevice* m_Device;
		VkDescriptorPool m_DescriptorPool = nullptr;

		GLFWwindow* m_WindowHandle;
	};
}
