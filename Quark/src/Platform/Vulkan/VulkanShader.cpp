#include "qkpch.h"
#include "VulkanShader.h"

#include "VulkanEnums.h"

#include "Quark/Filesystem/Filesystem.h"

#include <shaderc/shaderc.hpp>

namespace Quark {

	namespace Utils {

		static constexpr VkShaderStageFlags GetShaderStageType(std::string_view filepath)
		{
			VkShaderStageFlags stage{};
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

		static constexpr shaderc_shader_kind VulkanShaderStageToShaderC(VkShaderStageFlags stage)
		{
			switch (stage)
			{
				case VK_SHADER_STAGE_VERTEX_BIT:   return shaderc_glsl_vertex_shader;
				case VK_SHADER_STAGE_GEOMETRY_BIT: return shaderc_glsl_geometry_shader;
				case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_glsl_fragment_shader;
				case VK_SHADER_STAGE_COMPUTE_BIT:  return shaderc_glsl_compute_shader;

				QK_ASSERT_NO_DEFAULT("Unknown Vulkan shader stage");
			}

			return static_cast<shaderc_shader_kind>(0);
		}

		static constexpr ShaderStage GetShaderStageFromVulkanType(VkShaderStageFlags stage)
		{
			switch (stage)
			{
				case VK_SHADER_STAGE_VERTEX_BIT:   return ShaderStage::Vertex;
				case VK_SHADER_STAGE_GEOMETRY_BIT: return ShaderStage::Geometry;
				case VK_SHADER_STAGE_FRAGMENT_BIT: return ShaderStage::Fragment;
				case VK_SHADER_STAGE_COMPUTE_BIT:  return ShaderStage::Compute;

				QK_ASSERT_NO_DEFAULT("Unknown Vulkan shader stage");
			}

			return ShaderStage::None;
		}

		static constexpr std::string GetCacheExtension(const std::string& name, VkShaderStageFlags stage)
		{
			switch (stage)
			{
				case VK_SHADER_STAGE_VERTEX_BIT:   return name + ".vulkan.vert.spv";
				case VK_SHADER_STAGE_GEOMETRY_BIT: return name + ".vulkan.geo.spv";
				case VK_SHADER_STAGE_FRAGMENT_BIT: return name + ".vulkan.frag.spv";
				case VK_SHADER_STAGE_COMPUTE_BIT:  return name + ".vulkan.comp.spv";

				QK_ASSERT_NO_DEFAULT("Unknown Vulkan shader stage");
			}

			return {};
		}
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view filepath)
		: Shader(ExtractNameFromPath(filepath)), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		std::string source = Filesystem::ReadTextFile(filepath);
		auto shaderSources = SubstrStages(source);

		std::unordered_map<VkShaderStageFlags, std::string_view> glslSources;
		for (auto&& [stage, source] : shaderSources)
			glslSources[ShaderStageToVulkan(stage)] = source;

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_VulkanSpirv)
			Reflect(Utils::GetShaderStageFromVulkanType(stage), binary);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		: Shader(name), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<VkShaderStageFlags, std::string_view> glslSources = {
			{ VK_SHADER_STAGE_VERTEX_BIT,   vertexSource   },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource }
		};

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_VulkanSpirv)
			Reflect(Utils::GetShaderStageFromVulkanType(stage), binary);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		: Shader(name), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<VkShaderStageFlags, std::string_view> glslSources = {
			{ VK_SHADER_STAGE_VERTEX_BIT,   vertexSource   },
			{ VK_SHADER_STAGE_GEOMETRY_BIT, geometrySource },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource }
		};

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_VulkanSpirv)
			Reflect(Utils::GetShaderStageFromVulkanType(stage), binary);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderModules = {
			{ VK_SHADER_STAGE_VERTEX_BIT,   CreateShader(vertexSpirv)   },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, CreateShader(fragmentSpirv) }
		};

		for (auto&& [stage, binary] : m_VulkanSpirv)
			Reflect(Utils::GetShaderStageFromVulkanType(stage), binary);
	}

	VulkanShader::VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		m_ShaderModules = {
			{ VK_SHADER_STAGE_VERTEX_BIT,   CreateShader(vertexSpirv)   },
			{ VK_SHADER_STAGE_GEOMETRY_BIT, CreateShader(geometrySpirv) },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, CreateShader(fragmentSpirv) }
		};

		for (auto&& [stage, binary] : m_VulkanSpirv)
			Reflect(Utils::GetShaderStageFromVulkanType(stage), binary);
	}

	VulkanShader::~VulkanShader()
	{
		QK_PROFILE_FUNCTION();

		for (auto& [stage, module] : m_ShaderModules)
		{
			vkDestroyShaderModule(m_Device->GetVkHandle(), module, nullptr);
		}
	}

	bool VulkanShader::operator==(const Shader& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Name == o->m_Name;

		return false;
	}

	VkShaderModule VulkanShader::CreateShader(std::span<const uint32_t> spirvSource)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirvSource.size_bytes();
		createInfo.pCode    = spirvSource.data();

		VkShaderModule shaderModule;
		vkCreateShaderModule(m_Device->GetVkHandle(), &createInfo, nullptr, &shaderModule);

		return shaderModule;
	}

	void VulkanShader::CompileOrReadFromCache(const std::unordered_map<VkShaderStageFlags, std::string_view>& shaderSources)
	{
		std::unordered_map<VkShaderStageFlags, std::string> parsedGlslSources;

		// Get shaders from cache
		for (auto&& [stage, shaderSource] : shaderSources)
		{
			std::filesystem::path filename = CacheDirectory / Utils::GetCacheExtension(m_Name, stage);
			if (std::filesystem::exists(filename))
			{
				m_VulkanSpirv[stage] = Filesystem::ReadSpirvFile(filename.string());
			}
			else
			{
				parsedGlslSources[stage] = Parse(shaderSource);
			}
		}

		if (parsedGlslSources.empty())
		{
			// Create program from cache
			for (auto&& [stage, binary] : m_VulkanSpirv)
				m_ShaderModules[stage] = CreateShader(binary);
		}
		else
		{
			// Compile not-found shaders
			CompileVulkanSources(parsedGlslSources);
		}

		// Add new compiled sources to cache
		for (auto&& [stage, source] : parsedGlslSources)
		{
			auto it = m_VulkanSpirv.find(stage);
			if (it != m_VulkanSpirv.end())
			{
				std::filesystem::path filename = CacheDirectory / Utils::GetCacheExtension(m_Name, stage);
				std::ofstream out(filename, std::ios::out | std::ios::binary);

				auto&& [stage, binary] = *it;
				out.write((const char*)binary.data(), binary.size() * sizeof(uint32_t));
				out.close();
			}
		}
	}

	void VulkanShader::CompileVulkanSources(const std::unordered_map<VkShaderStageFlags, std::string>& shaderSources)
	{
		static constexpr bool optimize = true;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetSourceLanguage(shaderc_source_language_glsl);
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		if constexpr (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto&& [stage, shaderSource] : shaderSources)
		{
			auto result = compiler.CompileGlslToSpv(shaderSource, Utils::VulkanShaderStageToShaderC(stage), GetName().data(), "main", options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(result.GetErrorMessage());
				break;
			}

			m_VulkanSpirv[stage] = { result.cbegin(), result.cend() };
			m_ShaderModules[stage] = CreateShader(result);
		}
	}
}
