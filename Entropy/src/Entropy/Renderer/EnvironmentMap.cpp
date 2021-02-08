#include "EnvironmentMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "../../Platform/OpenGL/OpenGLEnvironmentMap.h"

namespace Entropy {

	Ref<EnvironmentMap> Entropy::EnvironmentMap::Create(const std::array<std::string, 6>& filepaths)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLEnvironmentMap>(filepaths);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}
