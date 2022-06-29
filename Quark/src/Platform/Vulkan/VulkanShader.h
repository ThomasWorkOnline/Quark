#pragma once

#include "Quark/Renderer/Shader.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanShader
	{
	public:
		VulkanShader(vk::ShaderStageFlagBits stage, std::string_view filepath);
		~VulkanShader();

		vk::PipelineShaderStageCreateInfo GetStageInfo() const { return m_StageInfo; }

	private:
		vk::ShaderModule m_ShaderModule;
		vk::PipelineShaderStageCreateInfo m_StageInfo;
	};
}
