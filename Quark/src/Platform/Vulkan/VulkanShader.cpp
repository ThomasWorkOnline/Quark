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
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view filepath)
		: Shader(filepath), m_Device(device)
	{
		QK_CORE_ASSERT(false, "Loading a Vulkan shader from a filepath is not supported");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SpirvView vertexSource, SpirvView fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(2)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::Vertex, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::Fragment, fragmentSource);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(3)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::Vertex, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(ShaderStage::Geometry, geometrySource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::Fragment, fragmentSource);
	}

	VulkanShader::~VulkanShader()
	{
		QK_PROFILE_FUNCTION();

		for (auto& kv : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), kv.second.Module, nullptr);
		}
	}

	bool VulkanShader::operator==(const Shader& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Name == o->m_Name;

		return false;
	}

	VulkanShaderModule VulkanShader::CreateShader(ShaderStage stage, SpirvView spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.WordCount * sizeof(uint32_t);
		createInfo.pCode = spirvSource.Data;

		VulkanShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule.Module);
		Reflect(stage, spirvSource.Data, spirvSource.WordCount);

		// TODO: get entry point from reflection
		shaderModule.EntryPoint = "main";

		return shaderModule;
	}
}
