#pragma once

#include "Formats.h"
#include "RenderPass.h"

#include <cstdint>

namespace Quark {

	enum class ColorSpace
	{
		SRGBNonLinear         = 0,
		DisplayP3NonLinear    = 1000104001,
		ExtendedSRGBLinear    = 1000104002,
		DisplayP3Linear       = 1000104003,
		DCIP3NonLinear        = 1000104004,
		BT709Linear           = 1000104005,
		BT709NonLinear        = 1000104006,
		BT2020Linear          = 1000104007,
		HDR10ST2084           = 1000104008,
		DolbyVision           = 1000104009,
		HDR10HLG              = 1000104010,
		ADOBERGBLinear        = 1000104011,
		ADOBERGBNonLinear     = 1000104012,
		PassThrough           = 1000104013,
		ExtendedSRGBNonLinear = 1000104014,
		DisplayNativeAMD      = 1000213000,
	};

	enum class SwapPresentMode
	{
		Immediate               = 0,
		Mailbox                 = 1,
		FIFO                    = 2,
		FIFORelaxed             = 3,
		SharedDemandRefresh     = 1000111000,
		SharedContinuousRefresh = 1000111001
	};

	struct SwapExtent
	{
		uint32_t Width = 0, Height = 0;
	};

	struct SwapSurfaceFormat
	{
		ColorFormat Format;
		ColorSpace  ColorSpace;
	};

	struct SwapChainSpecification
	{
		uint32_t          MinImageCount = 1;
		SwapExtent        Extent;
		SwapSurfaceFormat SurfaceFormat{};
		SwapPresentMode   PresentMode{};
		const RenderPass* RenderPass = nullptr;
	};

	class SwapChain
	{
	public:
		virtual ~SwapChain() = default;
	};
}
