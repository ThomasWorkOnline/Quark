#include "CubeMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "../../Platform/OpenGL/OpenGLCubeMap.h"

#include "../Core/Application.h"

namespace Quark {

	Ref<CubeMap> Quark::CubeMap::Create(const std::array<std::string, 6>& filepaths)
	{
		Ref<CubeMap> ref;

		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			ref = CreateRef<OpenGLCubeMap>(filepaths);
			break;
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}

		Application::Get().GetResourceManager().Hold(ref);
		return ref;
	}
}
