#include "qkpch.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

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

	VulkanShader::VulkanShader(std::string_view filepath)
	{
		auto type = Utils::GetShaderStageType(filepath);
		auto byteCode = Utils::ReadByteCode(filepath);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = byteCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.data());

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkCreateShaderModule(vkDevice, &createInfo, nullptr, &m_ShaderModule);

		m_StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_StageInfo.stage = type;
		m_StageInfo.module = m_ShaderModule;
		m_StageInfo.pName = "main";
	}

	VulkanShader::VulkanShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		QK_CORE_FATAL("Compiling Vulkan shaders is not yet implemented");
	}

	VulkanShader::VulkanShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		QK_CORE_FATAL("Compiling Vulkan shaders is not yet implemented");
	}

	VulkanShader::~VulkanShader()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDestroyShaderModule(vkDevice, m_ShaderModule, nullptr);
	}
}
