#include "qkpch.h"
#include "MetalGraphicsAPI.h"

// We need to define these once to link the static symbols inside Metal
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace Quark {

	void MetalGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();
	}

	void MetalGraphicsAPI::SetClearColor(const glm::vec4& rgba)
	{
		
	}

	void MetalGraphicsAPI::Clear()
	{
		
	}
}
