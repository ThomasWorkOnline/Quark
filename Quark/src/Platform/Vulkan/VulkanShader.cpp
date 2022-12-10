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

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		: Shader(name), m_Device(device)
	{
		QK_CORE_ASSERT(false, "Source shaders in Vulkan are not supported!");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		: Shader(name), m_Device(device)
	{
		QK_CORE_ASSERT(false, "Source shaders in Vulkan are not supported!");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name), m_Device(device), m_ShaderStages(2)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::Vertex, vertexSpirv);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::Fragment, fragmentSpirv);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name), m_Device(device), m_ShaderStages(3)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(ShaderStage::Vertex, vertexSpirv);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(ShaderStage::Geometry, geometrySpirv);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(ShaderStage::Fragment, fragmentSpirv);
	}

	VulkanShader::~VulkanShader()
	{
		QK_PROFILE_FUNCTION();

		for (auto& [stage, module] : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), module, nullptr);
		}
	}

	bool VulkanShader::operator==(const Shader& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return GetName() == o->GetName();

		return false;
	}

	VkShaderModule VulkanShader::CreateShader(ShaderStage stage, std::span<const uint32_t> spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.size_bytes();
		createInfo.pCode = spirvSource.data();

		VkShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule);

		Reflect(stage, spirvSource);

		return shaderModule;
	}
}
