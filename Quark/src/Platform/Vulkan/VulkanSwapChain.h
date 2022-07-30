#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/SwapChain.h"

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain final : public SwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* window, VkSurfaceKHR surface);
		~VulkanSwapChain();

		virtual uint32_t GetWidth() const override { return m_Format.Extent.width; }
		virtual uint32_t GetHeight() const override { return m_Format.Extent.height; }
		virtual uint32_t GetImageCount() const override { return m_Format.ImageCount; }
		virtual uint32_t GetCurrentImageIndex() const override { return m_ImageIndex; }

		virtual void AcquireNextImage() override;
		virtual void Present() override;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<FramebufferAttachment>& GetAttachment(uint32_t imageIndex) const override { return m_Attachments[imageIndex]; }

	private:
		void Invalidate();

	private:
		struct Format
		{
			uint32_t             ImageCount = 0;
			VkExtent2D           Extent{};
			VkSurfaceFormatKHR   ActualSurfaceFormat{};
			VkPresentModeKHR     ActualPresentMode{};
		};

		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<Ref<FramebufferAttachment>> m_Attachments;

		uint32_t m_ImageIndex = 0;
		Format m_Format;
	};
}
