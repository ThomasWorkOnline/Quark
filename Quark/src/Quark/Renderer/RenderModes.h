#pragma once

namespace Quark {

	enum class RenderCullMode
	{
		None,
		Front,
		Back,
		FrontAndBack,

		Default = Back
	};

	enum class DepthCompareFunction
	{
		Never,
		Always,
		NotEqual,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,

		Default = Less
	};
}
