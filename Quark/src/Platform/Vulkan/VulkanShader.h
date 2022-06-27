#pragma once

#include "Quark/Renderer/Shader.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanShader
	{
	public:
		VulkanShader(vk::ShaderStageFlagBits stage, std::string_view filepath);
		~VulkanShader();

		vk::PipelineShaderStageCreateInfo GetStageInfo() const { return m_VkStageInfo; }

	private:
		vk::ShaderModule m_VkShaderModule;
		vk::PipelineShaderStageCreateInfo m_VkStageInfo;
	};
}
