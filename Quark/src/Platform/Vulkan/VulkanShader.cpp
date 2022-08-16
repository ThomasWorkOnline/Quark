#include "qkpch.h"
#include "VulkanShader.h"

namespace Quark {

	namespace Utils {

		static std::vector<char> ReadByteCode(std::string_view filename)
		{
			std::ifstream in(filename.data(), std::ios::ate | std::ios::binary);
			QK_CORE_ASSERT(in.is_open(), "Failed to open file");

			size_t fileSize = (size_t)in.tellg();
			std::vector<char> buffer(fileSize);

			in.seekg(0);
			in.read(buffer.data(), fileSize);
			in.close();

			return buffer;
		}

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
		: m_Device(device)
	{
		QK_CORE_ASSERT(false, "Loading a Vulkan shader from a filepath is not supported");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SPIRVBinary vertexSource, SPIRVBinary fragmentSource)
		: m_Device(device), m_Name(name)
	{
		m_ShaderStages.reserve(2);
		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(fragmentSource);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SPIRVBinary vertexSource, SPIRVBinary geometrySource, SPIRVBinary fragmentSource)
		: m_Device(device), m_Name(name)
	{
		m_ShaderStages.reserve(3);
		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(geometrySource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(fragmentSource);
	}

	VulkanShader::~VulkanShader()
	{
		for (auto& kv : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), kv.second, nullptr);
		}
	}

	VkShaderModule VulkanShader::CreateShader(SPIRVBinary spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(spirvSource.data());

		VkShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule);
		return shaderModule;
	}
}
