#include "qkpch.h"
#include "VulkanShader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

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
		: m_Device(device)
	{
		QK_CORE_ASSERT(false, "Loading a Vulkan shader from a filepath is not supported");
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SpirvSource vertexSource, SpirvSource fragmentSource)
		: m_Device(device), m_Name(name)
	{
		m_ShaderStages.reserve(2);
		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, SpirvSource vertexSource, SpirvSource geometrySource, SpirvSource fragmentSource)
		: m_Device(device), m_Name(name)
	{
		m_ShaderStages.reserve(3);
		m_ShaderStages[VK_SHADER_STAGE_VERTEX_BIT]   = CreateShader(VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
		m_ShaderStages[VK_SHADER_STAGE_GEOMETRY_BIT] = CreateShader(VK_SHADER_STAGE_GEOMETRY_BIT, geometrySource);
		m_ShaderStages[VK_SHADER_STAGE_FRAGMENT_BIT] = CreateShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
	}

	VulkanShader::~VulkanShader()
	{
		for (auto& kv : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), kv.second, nullptr);
		}
	}

	VkShaderModule VulkanShader::CreateShader(VkShaderStageFlagBits stage, SpirvSource spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.WordCount * sizeof(uint32_t);
		createInfo.pCode = spirvSource.Data;

		VkShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule);
		Reflect(stage, spirvSource);

		return shaderModule;
	}

	void VulkanShader::Reflect(VkShaderStageFlagBits stage, SpirvSource spirvSource)
	{
		spirv_cross::Compiler compiler(spirvSource.Data, spirvSource.WordCount);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		QK_CORE_TRACE("VulkanShader::Reflect - {0} '{1}'", Utils::GetShaderStageToString(stage), m_Name);
		QK_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		QK_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		QK_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			size_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();

			QK_CORE_TRACE(" \"{0}\"", resource.name);
			QK_CORE_TRACE("    Size = {0}", bufferSize);
			QK_CORE_TRACE("    Binding = {0}", binding);
			QK_CORE_TRACE("    Members = {0}", memberCount);
		}
	}
}
