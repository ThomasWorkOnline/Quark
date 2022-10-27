#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include <Windows.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGISwapChain;
struct IDXGIFactory4;

namespace Quark {

	class Direct3D12Context final : public GraphicsContext
	{
	public:
		Direct3D12Context(void* windowHandle);
		virtual ~Direct3D12Context() final override;

		virtual void Init() final override;
		virtual void CreateSwapChain(const RenderPass* renderPass) final override;
		virtual void WaitUntilDeviceIdle() final override;

		virtual void BeginFrame(uint32_t frameIndex) final override;
		virtual void Flush(CommandBuffer* commandBuffer, uint32_t frameIndex) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual Framebuffer* GetFramebuffer() const final override;
		virtual ViewportExtent GetViewportExtent() const final override;

	private:
		HWND m_WindowHandle;
		ID3D12Device* m_Device;
		IDXGIFactory4* m_Factory;
		ID3D12CommandQueue* m_CommandQueue;
		IDXGISwapChain* m_SwapChain;
	};
}
