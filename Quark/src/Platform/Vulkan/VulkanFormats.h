#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/ColorFormats.h"
#include "Quark/Renderer/RenderModes.h"
#include "Quark/Renderer/Buffer.h"
#include "Quark/Renderer/TextureFormats.h"

#include <vulkan/vulkan.h>

namespace Quark {

	constexpr VkFormat InternalFormatToVulkan(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB8:            return VK_FORMAT_R8G8B8_UINT;
			case ColorDataFormat::RGB16:           return VK_FORMAT_R16G16B16_UINT;
			case ColorDataFormat::RGB32:           return VK_FORMAT_R32G32B32_UINT;
			case ColorDataFormat::RGBA8:           return VK_FORMAT_R8G8B8A8_UINT;
			case ColorDataFormat::RGBA16:          return VK_FORMAT_R16G16B16A16_UINT;
			case ColorDataFormat::RGB8_SRGB:       return VK_FORMAT_R8G8B8_SRGB;
			case ColorDataFormat::RGBA8_SRGB:      return VK_FORMAT_R8G8B8A8_SRGB;
			case ColorDataFormat::BGR8_SRGB:       return VK_FORMAT_B8G8R8_SRGB;
			case ColorDataFormat::BGRA8_SRGB:      return VK_FORMAT_B8G8R8A8_SRGB;
			case ColorDataFormat::Red8:            return VK_FORMAT_R8_UINT;
			case ColorDataFormat::RGB16f:          return VK_FORMAT_R16G16B16_SFLOAT;
			case ColorDataFormat::RGB32f:          return VK_FORMAT_R32G32B32_SFLOAT;
			case ColorDataFormat::RGBA16f:         return VK_FORMAT_R16G16B16A16_SFLOAT;
			case ColorDataFormat::RGBA32f:         return VK_FORMAT_R32G32B32A32_SFLOAT;

			QK_ASSERT_NO_DEFAULT("Invalid internal color format");
		}

		return VK_FORMAT_UNDEFINED;
	}

	constexpr VkFormat ShaderDataTypeToVulkan(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:   return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2:  return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3:  return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Double:  return VK_FORMAT_R64_SFLOAT;
			case ShaderDataType::Double2: return VK_FORMAT_R64G64_SFLOAT;
			case ShaderDataType::Double3: return VK_FORMAT_R64G64B64_SFLOAT;
			case ShaderDataType::Double4: return VK_FORMAT_R64G64B64A64_SFLOAT;

			// TODO: map matrices, int and bool types

			QK_ASSERT_NO_DEFAULT("Unknown ShaderDataType");
		}

		return VK_FORMAT_UNDEFINED;
	}

	constexpr VkCullModeFlagBits RenderCullModeToVulkan(RenderCullMode mode)
	{
		switch (mode)
		{
			case RenderCullMode::None:         return VK_CULL_MODE_NONE;
			case RenderCullMode::Front:        return VK_CULL_MODE_FRONT_BIT;
			case RenderCullMode::Back:         return VK_CULL_MODE_BACK_BIT;
			case RenderCullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;

			QK_ASSERT_NO_DEFAULT("Unknown culling mode");
		}

		return VK_CULL_MODE_NONE;
	}
}
