#include "qkpch.h"
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"

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
	}

	VulkanShader::VulkanShader(vk::ShaderStageFlagBits stage, std::string_view filepath)
		: m_VkDevice(VulkanGraphicsContext::GetCurrentDevice().GetVkHandle())
	{
		auto byteCode = Utils::ReadByteCode(filepath);

		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(byteCode.size());
		createInfo.setPCode(reinterpret_cast<const uint32_t*>(byteCode.data()));

		m_VkShaderModule = m_VkDevice.createShaderModule(createInfo);

		m_VkStageInfo.setStage(stage);
		m_VkStageInfo.setModule(m_VkShaderModule);
		m_VkStageInfo.setPName("main");
	}

	VulkanShader::~VulkanShader()
	{
		m_VkDevice.destroyShaderModule(m_VkShaderModule);
	}
}
