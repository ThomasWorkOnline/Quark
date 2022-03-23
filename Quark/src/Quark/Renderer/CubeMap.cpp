#include "CubeMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "Platform/OpenGL/OpenGLCubeMap.h"

namespace Quark {

	Ref<CubeMap> Quark::CubeMap::Create(const std::array<std::string_view, 6>& filepaths)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLCubeMap>(filepaths);
		}

		QK_FATAL("Rendering API not supported");
		return nullptr;
	}
}
