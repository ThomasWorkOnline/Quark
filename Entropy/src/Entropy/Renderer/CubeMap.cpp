#include "CubeMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "../../Platform/OpenGL/OpenGLCubeMap.h"

namespace Entropy {

	Ref<CubeMap> Entropy::CubeMap::Create(const std::array<std::string, 6>& filepaths)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLCubeMap>(filepaths);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}
