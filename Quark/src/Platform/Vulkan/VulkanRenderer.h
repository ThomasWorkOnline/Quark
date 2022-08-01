#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Renderer.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanRenderer final : public BackendRenderer
	{
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
		
		virtual void BeginFrame() override;
		virtual void Submit() override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

		static uint32_t      GetCurrentFrameIndex();
		static VkFence       GetInFlightFence();
		static VkSemaphore   GetRenderFinishedSemaphore();
		static VkSemaphore   GetImageAvailableSemaphore();
		static VkFramebuffer GetActiveFramebuffer();
	};
}
