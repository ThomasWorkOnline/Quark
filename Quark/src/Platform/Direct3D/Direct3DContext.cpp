#include "qkpch.h"
#include "Direct3DContext.h"

#include <d3d12.h>
#include <d3d12SDKLayers.h>

#include <dxgi.h>
#include <dxgi1_4.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Quark {

	static constexpr bool s_UseWarpDevice = false;

	void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);

	Direct3D12Context::Direct3D12Context(void* windowHandle)
		: m_WindowHandle(glfwGetWin32Window(static_cast<GLFWwindow*>(windowHandle)))
		, m_Device(nullptr)
		, m_CommandQueue(nullptr)
		, m_SwapChain(nullptr)
	{
		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");
	}

	Direct3D12Context::~Direct3D12Context()
	{
		QK_PROFILE_FUNCTION();

		m_SwapChain->Release();
		m_CommandQueue->Release();
		m_Device->Release();
	}

	void Direct3D12Context::Init()
	{
		QK_PROFILE_FUNCTION();

		HINSTANCE hInstance = GetModuleHandle(NULL);

#if QK_ASSERT_API_VALIDATION_ERRORS
		// Enable the D3D12 debug layer.
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
#endif

		IDXGIFactory4* factory;
		CreateDXGIFactory1(IID_PPV_ARGS(&factory));

		if constexpr (s_UseWarpDevice)
		{
			IDXGIAdapter* warpAdapter;
			factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

			QK_CORE_ASSERT(SUCCEEDED(D3D12CreateDevice(
				warpAdapter,
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_Device)
			)), "Could not create Direct3D device!");

			warpAdapter->Release();
		}
		else
		{
			IDXGIAdapter1* hardwareAdapter;
			GetHardwareAdapter(factory, &hardwareAdapter);

			QK_CORE_ASSERT(SUCCEEDED(D3D12CreateDevice(
				hardwareAdapter,
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_Device)
			)), "Could not create Direct3D device!");

			hardwareAdapter->Release();
		}

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		QK_CORE_ASSERT(SUCCEEDED(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue))),
			"Could not create Direct3D command queue!");

		RECT extent{};
		GetClientRect(m_WindowHandle, &extent);

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 3;
		swapChainDesc.BufferDesc.Width = extent.right - extent.left;
		swapChainDesc.BufferDesc.Height = extent.bottom - extent.top;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.OutputWindow = m_WindowHandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE;

		QK_CORE_ASSERT(SUCCEEDED(factory->CreateSwapChain(
			m_CommandQueue,        // Swap chain needs the queue so that it can force a flush on it.
			&swapChainDesc,
			&m_SwapChain
		)), "Could not create Direct3D swap chain!");

		factory->Release();
	}

	void Direct3D12Context::WaitUntilDeviceIdle()
	{
	}

	void Direct3D12Context::BeginFrame(uint32_t frameIndex)
	{
	}

	void Direct3D12Context::Flush(CommandBuffer* commandBuffer, uint32_t frameIndex)
	{
	}

	void Direct3D12Context::SwapBuffers()
	{
	}

	void Direct3D12Context::SetSwapInterval(int interval)
	{
	}

	void Direct3D12Context::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
	}

	uint32_t Direct3D12Context::GetCurrentImageIndex() const
	{
		return 0;
	}

	uint32_t Direct3D12Context::GetSwapChainImageCount() const
	{
		return 3;
	}

	FramebufferAttachment* Direct3D12Context::GetColorAttachment(uint32_t index) const
	{
		return nullptr;
	}

	void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter1* pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
			{
				// No more adapters to enumerate.
				break;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				*ppAdapter = pAdapter;
				return;
			}
			pAdapter->Release();
		}
	}
}
