#include "CubeMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "../../Platform/OpenGL/OpenGLCubeMap.h"

namespace Quark {

	Ref<CubeMap> Quark::CubeMap::Create(const std::array<std::string, 6>& filepaths)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLCubeMap, DeferredObjectDeleter>(filepaths);
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}
		return nullptr;
	}
}
