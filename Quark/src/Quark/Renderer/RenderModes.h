#pragma once

namespace Quark {

	enum class RenderCullMode
	{
		None = 0,
		Front,
		Back,
		FrontAndBack,

		Default = Back
	};

	enum class DepthCompareFunction
	{
		Never = 0,
		Always,
		NotEqual,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,

		Default = Less
	};
}
