#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	enum class SwapChainPresentMode
	{
		Immediate   = 0,
		Mailbox     = 1,
		Fifo        = 2,
		FifoRelaxed = 3,

		Preferred = Mailbox
	};

	enum class SwapChainImageFormat
	{

	};

	enum class ColorSpace
	{

	};

	struct SwapChainSpecification
	{
		uint32_t             Width = 0, Height = 0;
		uint32_t             ImageCount = 0;
		SwapChainPresentMode PresentMode = SwapChainPresentMode::Preferred;
		SwapChainImageFormat SurfaceFormat{};
		ColorSpace           ColorSpace{};
	};

	class SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;
	};
}
