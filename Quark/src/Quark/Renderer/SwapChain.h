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
		uint32_t             FramesInFlight = 0;
		SwapChainPresentMode PresentMode = SwapChainPresentMode::Preferred;
		SwapChainImageFormat SurfaceFormat{};
		ColorSpace           ColorSpace{};
	};

	class SwapChain
	{
	public:
		SwapChain(const SwapChainSpecification& spec)
			: m_Spec(spec) {}

		virtual ~SwapChain() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetImageCount() const = 0;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		const SwapChainSpecification& GetSpecification() const { return m_Spec; }

	protected:
		SwapChainSpecification m_Spec;
	};
}
