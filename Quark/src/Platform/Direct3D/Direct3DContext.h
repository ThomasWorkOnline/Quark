#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct HWND__* HWND;

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

		virtual void Init(const SwapChainSpecification& spec) final override;
		virtual void WaitUntilDeviceIdle() final override;

		virtual void BeginFrame(uint32_t frameIndex) final override;
		virtual void Flush(CommandBuffer* commandBuffer) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual SwapChain*        GetSwapChain() final override;
		virtual uint32_t          QuerySwapChainImageCount() const final override;

		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override;
		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const final override;
		virtual ViewportExtent    ChooseSwapExtent(uint32_t width, uint32_t height) const final override;

	private:
		HWND m_WindowHandle;
		ID3D12Device* m_Device;
		IDXGIFactory4* m_Factory;
		ID3D12CommandQueue* m_CommandQueue;
		IDXGISwapChain* m_SwapChain;
	};
}
