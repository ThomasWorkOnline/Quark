#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include <Windows.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGISwapChain;

namespace Quark {

	class Direct3D12Context final : public GraphicsContext
	{
	public:
		Direct3D12Context(void* windowHandle);
		virtual ~Direct3D12Context() final override;

		virtual void Init() final override;
		virtual void WaitUntilDeviceIdle() final override;

		virtual void BeginFrame() final override;
		virtual void Flush() final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetCurrentImageIndex() const final override;
		virtual uint32_t GetSwapChainImageCount() const final override;
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;

		virtual CommandBuffer* GetCommandBuffer() final override;

	private:
		HWND m_WindowHandle;
		ID3D12Device* m_Device;
		ID3D12CommandQueue* m_CommandQueue;
		IDXGISwapChain* m_SwapChain;
	};
}
