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

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(2)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::VertexStage, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::FragmentStage, fragmentSource);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource)
		: Shader(name), m_Device(device), m_ShaderStages(3)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::VertexStage, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(ShaderStage::GeometryStage, geometrySource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::FragmentStage, fragmentSource);
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

	VulkanShaderModule VulkanShader::CreateShader(ShaderStage stage, const SpirvSource& spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.size() * sizeof(uint32_t);
		createInfo.pCode = spirvSource.data();

		VulkanShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule.Module);

		// TODO: get entry point from reflection
		shaderModule.VulkanSpirv = spirvSource;
		shaderModule.EntryPoint = "main";
		Reflect(stage, spirvSource);

		return shaderModule;
	}
}
