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

		static vk::ShaderStageFlagBits GetShaderStageType(std::string_view filepath)
		{
			vk::ShaderStageFlagBits stage{};
			if (filepath.find("vert") != std::string::npos)
				stage = vk::ShaderStageFlagBits::eVertex;
			else if (filepath.find("frag") != std::string::npos)
				stage = vk::ShaderStageFlagBits::eFragment;
			else if (filepath.find("geometry") != std::string::npos)
				stage = vk::ShaderStageFlagBits::eGeometry;
			else if (filepath.find("compute") != std::string::npos)
				stage = vk::ShaderStageFlagBits::eCompute;

			QK_CORE_ASSERT(stage != (vk::ShaderStageFlagBits)0, "Could not find the type of shader");
			return stage;
		}
	}

	VulkanShader::VulkanShader(std::string_view filepath)
	{
		auto type = Utils::GetShaderStageType(filepath);
		auto byteCode = Utils::ReadByteCode(filepath);

		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(byteCode.size());
		createInfo.setPCode(reinterpret_cast<const uint32_t*>(byteCode.data()));

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		m_ShaderModule = vkDevice.createShaderModule(createInfo);

		m_StageInfo.setStage(type);
		m_StageInfo.setModule(m_ShaderModule);
		m_StageInfo.setPName("main");
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
		vkDevice.destroyShaderModule(m_ShaderModule);
	}
}
