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

	enum class RenderDepthFunction
	{
		Never,
		Always,
		NotEqual,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,

		Default = Less
	};
}
