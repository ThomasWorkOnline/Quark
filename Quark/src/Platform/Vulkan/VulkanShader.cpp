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

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		: m_Device(device), m_Name(name)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertexSource.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertexSource.data());
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &m_VertexShaderModule);

		createInfo.codeSize = fragmentSource.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentSource.data());
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &m_FragmentShaderModule);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		: m_Device(device)
	{
		QK_CORE_ASSERT(false, "Compiling Vulkan shaders is not supported");
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetVkHandle(), m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetVkHandle(), m_GeometryShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetVkHandle(), m_FragmentShaderModule, nullptr);
	}
}
