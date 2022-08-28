#include "qkpch.h"
#include "VulkanShader.h"

namespace Quark {

	namespace Utils {

		static constexpr VkShaderStageFlagBits GetShaderStageType(std::string_view filepath)
		{
			VkShaderStageFlagBits stage{};
			if (filepath.find("vert") != std::string::npos)
				stage = VK_SHADER_STAGE_VERTEX_BIT;
			else if (filepath.find("frag") != std::string::npos)
				stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (filepath.find("geometry") != std::string::npos)
				stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			else if (filepath.find("compute") != std::string::npos)
				stage = VK_SHADER_STAGE_COMPUTE_BIT;

			QK_CORE_ASSERT(stage, "Could not find the type of shader");
			return stage;
		}

		static constexpr const char* GetShaderStageToString(VkShaderStageFlags stage)
		{
			switch (stage)
			{
				case VK_SHADER_STAGE_VERTEX_BIT:   return "VK_SHADER_STAGE_VERTEX_BIT";
				case VK_SHADER_STAGE_FRAGMENT_BIT: return "VK_SHADER_STAGE_FRAGMENT_BIT";
				case VK_SHADER_STAGE_GEOMETRY_BIT: return "VK_SHADER_STAGE_GEOMETRY_BIT";
				case VK_SHADER_STAGE_COMPUTE_BIT:  return "VK_SHADER_STAGE_COMPUTE_BIT";

				QK_ASSERT_NO_DEFAULT("Unknown shader stage");
			}

			return nullptr;
		}
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view filepath)
		: Shader(filepath), m_Device(device)
	{
		QK_CORE_ASSERT(false, "Loading a Vulkan shader from a filepath is not supported");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(2)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(3)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(VK_SHADER_STAGE_GEOMETRY_BIT, geometrySource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
	}

	VulkanShader::~VulkanShader()
	{
		QK_PROFILE_FUNCTION();

		for (auto& kv : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), kv.second.Module, nullptr);
		}
	}

	VulkanShaderModule VulkanShader::CreateShader(VkShaderStageFlagBits stage, const SpirvSource& spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.size() * sizeof(uint32_t);
		createInfo.pCode = spirvSource.data();

		VulkanShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule.Module);

		shaderModule.VulkanSpirv = spirvSource;
		shaderModule.EntryPoint = "main";
		Reflect(Utils::GetShaderStageToString(stage), spirvSource);

		return shaderModule;
	}
}
