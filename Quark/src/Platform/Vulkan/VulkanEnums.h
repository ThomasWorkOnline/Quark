#pragma once

#include "Quark/Renderer/Buffer.h"
#include "Quark/Renderer/Formats.h"
#include "Quark/Renderer/RenderModes.h"
#include "Quark/Renderer/RenderPass.h"
#include "Quark/Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace Quark {

	inline constexpr VkCullModeFlagBits s_VulkanCullModeLUT[] = {
		/*None*/            VK_CULL_MODE_NONE,
		/*Front*/           VK_CULL_MODE_FRONT_BIT,
		/*Back*/            VK_CULL_MODE_BACK_BIT,
		/*FrontAndBack*/    VK_CULL_MODE_FRONT_AND_BACK
	};

	inline constexpr VkFormat s_VulkanDataFormatLUT[] = {
		/*None*/            VK_FORMAT_UNDEFINED,

		/*RGB8*/            VK_FORMAT_R8G8B8_UNORM,
		/*RGB16*/           VK_FORMAT_R16G16B16_UNORM,
		/*RGB32*/           VK_FORMAT_R32G32B32_UINT,

		/*RGBA8*/           VK_FORMAT_R8G8B8A8_UNORM,
		/*RGBA16*/          VK_FORMAT_R16G16B16A16_UNORM,

		/*RGB8_SRGB*/       VK_FORMAT_R8G8B8_SRGB,
		/*RGBA8_SRGB*/      VK_FORMAT_R8G8B8A8_SRGB,

		/*BGR8_SRGB*/       VK_FORMAT_B8G8R8_SRGB,
		/*BGRA8_SRGB*/      VK_FORMAT_B8G8R8A8_SRGB,

		/*RGB16f*/          VK_FORMAT_R16G16B16_SFLOAT,
		/*RGB32f*/          VK_FORMAT_R32G32B32_SFLOAT,
		/*RGBA16f*/         VK_FORMAT_R16G16B16A16_SFLOAT,
		/*RGBA32f*/         VK_FORMAT_R32G32B32A32_SFLOAT,

		/*Red8*/            VK_FORMAT_R8_UNORM,

		/*Depth24*/         VK_FORMAT_D24_UNORM_S8_UINT,
		/*Depth24Stencil8*/ VK_FORMAT_D24_UNORM_S8_UINT
	};

	inline constexpr VkPipelineBindPoint s_VulkanPipelineBindPointLUT[] = {
		/*None*/            VK_PIPELINE_BIND_POINT_MAX_ENUM,
		/*Graphics*/        VK_PIPELINE_BIND_POINT_GRAPHICS,
		/*Compute*/         VK_PIPELINE_BIND_POINT_COMPUTE
	};

	inline constexpr VkFormat s_VulkanShaderDataTypeLUT[] = {
		/*None*/            VK_FORMAT_UNDEFINED,
		/*Float*/           VK_FORMAT_R32_SFLOAT,
		/*Float2*/          VK_FORMAT_R32G32_SFLOAT,
		/*Float3*/          VK_FORMAT_R32G32B32_SFLOAT,
		/*Float4*/          VK_FORMAT_R32G32B32A32_SFLOAT,

		/*Double*/          VK_FORMAT_R64_SFLOAT,
		/*Double2*/         VK_FORMAT_R64G64_SFLOAT,
		/*Double3*/         VK_FORMAT_R64G64B64_SFLOAT,
		/*Double4*/         VK_FORMAT_R64G64B64A64_SFLOAT,

		// TODO: Implement matrix data types

		/*Mat3f*/           VK_FORMAT_UNDEFINED,
		/*Mat4f*/           VK_FORMAT_UNDEFINED,
		/*Mat3d*/           VK_FORMAT_UNDEFINED,
		/*Mat4d*/           VK_FORMAT_UNDEFINED,

		/*Int*/             VK_FORMAT_R32_SINT,
		/*Int2*/            VK_FORMAT_R32G32_SINT,
		/*Int3*/            VK_FORMAT_R32G32B32_SINT,
		/*Int4*/            VK_FORMAT_R32G32B32A32_SINT,
		/*Bool*/            VK_FORMAT_R8_UINT
	};

	inline constexpr VkShaderStageFlagBits s_VulkanShaderStageLUT[] = {
		/*None*/            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM,
		/*Vertex*/          VK_SHADER_STAGE_VERTEX_BIT,
		/*Geometry*/        VK_SHADER_STAGE_GEOMETRY_BIT,
		/*Fragment*/        VK_SHADER_STAGE_FRAGMENT_BIT,
		/*Compute*/         VK_SHADER_STAGE_COMPUTE_BIT
	};

	constexpr VkCullModeFlagBits CullModeToVulkan(RenderCullMode mode)
	{
		return s_VulkanCullModeLUT[static_cast<size_t>(mode)];
	}

	constexpr VkFormat DataFormatToVulkan(ColorDataFormat format)
	{
		return s_VulkanDataFormatLUT[static_cast<size_t>(format)];
	}

	constexpr VkPipelineBindPoint PipelineBindPointToVulkan(PipelineBindPoint bindpoint)
	{
		return s_VulkanPipelineBindPointLUT[static_cast<size_t>(bindpoint)];
	}

	constexpr VkFormat ShaderDataTypeToVulkan(ShaderDataType type)
	{
		return s_VulkanShaderDataTypeLUT[static_cast<size_t>(type)];
	}

	constexpr VkShaderStageFlagBits ShaderStageToVulkan(ShaderStage	stage)
	{
		return s_VulkanShaderStageLUT[static_cast<size_t>(stage)];
	}
}
