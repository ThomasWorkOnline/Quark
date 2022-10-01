#include "qkpch.h"
#include "Shader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

#include <fstream>

#include "GraphicsAPI.cpp"

namespace Quark {

	namespace Utils {

		constexpr const char* ShaderStageToString(ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::Vertex:   return "Vertex";
				case ShaderStage::Geometry: return "Geometry";
				case ShaderStage::Fragment: return "Fragment";
				case ShaderStage::Compute:  return "Compute";

				QK_ASSERT_NO_DEFAULT("Unknown shader stage");
			}

			return nullptr;
		}
	}

	Shader::Shader(std::string_view name)
		: m_Name(name)
	{
	}

	void Shader::Reflect(ShaderStage stage, const uint32_t* spirvSource, size_t words)
	{
		QK_PROFILE_FUNCTION();

		spirv_cross::Compiler compiler(spirvSource, words);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		QK_CORE_TRACE("Shader::Reflect - \"{0}\" at {1}", m_Name, Utils::ShaderStageToString(stage));
		QK_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		QK_CORE_TRACE("    {0} image samplers", resources.sampled_images.size());

		auto entryPoints = compiler.get_entry_points_and_stages();
		QK_CORE_ASSERT(entryPoints.size() == 1, "Shaders currently don't support multiple entry points");

		m_ShaderResources.BindingCount += (uint32_t)resources.uniform_buffers.size();
		m_ShaderResources.BindingCount += (uint32_t)resources.sampled_images.size();

		QK_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& type = compiler.get_type(resource.base_type_id);
			uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

			size_t bufferSize = compiler.get_declared_struct_size(type);
			size_t memberCount = type.member_types.size();

			UniformBufferResource ubResource{};
			ubResource.Size = bufferSize;
			ubResource.Stage = stage;
			ubResource.Decorators.Set = set;
			ubResource.Decorators.Binding = binding;
			ubResource.Decorators.Name = compiler.get_name(resource.id);

			m_ShaderResources.UniformBuffers.push_back(ubResource);
			m_ShaderResources.DescriptorCount++;

			QK_CORE_TRACE(" \"{0}\"", resource.name);
			QK_CORE_TRACE("    Set = {0}", set);
			QK_CORE_TRACE("    Binding = {0}", binding);
			QK_CORE_TRACE("    Size = {0} bytes", bufferSize);
			QK_CORE_TRACE("    Members = {0}", memberCount);
		}

		QK_CORE_TRACE("Image samplers:");
		for (const auto& resource : resources.sampled_images)
		{
			const auto& type = compiler.get_type(resource.type_id);
			uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

			size_t dimension = type.array.size();
			
			uint32_t samplerCount = 0;
			if (dimension > 0)
			{
				for (size_t i = 0; i < dimension; i++)
				{
					uint32_t length = type.array[i];
					samplerCount += length;
				}
			}
			else
			{
				samplerCount = 1;
			}

			SamplerArrayResource sResource{};
			sResource.SamplerCount = samplerCount;
			sResource.Stage = stage;
			sResource.Decorators.Set = set;
			sResource.Decorators.Binding = binding;
			sResource.Decorators.Name = compiler.get_name(resource.id);

			m_ShaderResources.SamplerArrays.push_back(sResource);
			m_ShaderResources.DescriptorCount += samplerCount;

			QK_CORE_TRACE(" \"{0}\"", resource.name);
			QK_CORE_TRACE("    Set = {0}", set);
			QK_CORE_TRACE("    Binding = {0}", binding);
			QK_CORE_TRACE("    Samplers = {0}", samplerCount);
;		}
	}

	Scope<Shader> Shader::Create(std::string_view filepath)
	{
		return s_GraphicsAPI->CreateShader(filepath);
	}

	Scope<Shader> Shader::Create(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSource, fragmentSource);
	}

	Scope<Shader> Shader::Create(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Shader> Shader::CreateLegacy(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return s_GraphicsAPI->CreateShaderLegacy(name, vertexSource, fragmentSource);
	}

	Scope<Shader> Shader::CreateLegacy(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return s_GraphicsAPI->CreateShaderLegacy(name, vertexSource, geometrySource, fragmentSource);
	}

	SpirvSource ReadSpirvFile(std::string_view filepath)
	{
		SpirvSource result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);
		if (in)
		{
			size_t size = in.tellg();
			Verify(size % sizeof(uint32_t) == 0, "Invalid byte alignment for file: '{0}' (SPIR-V requires 4-bytes per word)", filepath);

			if (size != -1)
			{
				result.resize(size / sizeof(uint32_t));
				in.seekg(0, std::ios::beg);
				in.read((char*)result.data(), size);
				in.close();

				return result;
			}
		}

		ThrowRuntimeError("Could not open file '{0}'", filepath);
		return result;
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	void ShaderLibrary::Add(std::string_view name, const Ref<Shader>& shader)
	{
		QK_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[GetHashedName(name)] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		Add(shader->GetName(), shader);
	}

	bool ShaderLibrary::Exists(std::string_view name) const
	{
		return m_Shaders.find(GetHashedName(name)) != m_Shaders.end();
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view name, std::string_view filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(std::string_view name) const
	{
		QK_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders.at(GetHashedName(name));
	}
}
