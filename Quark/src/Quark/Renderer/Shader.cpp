#include "qkpch.h"
#include "Shader.h"

#include "Renderer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

#include <charconv>

#include "GraphicsAPI.cpp"

namespace Quark {

	namespace Utils {

		static constexpr ShaderStage ShaderTypeFromString(std::string_view type)
		{
			if (type == "vertex")
				return ShaderStage::Vertex;
			else if (type == "fragment" || type == "pixel")
				return ShaderStage::Fragment;
			else if (type == "geometry")
				return ShaderStage::Geometry;
			else if (type == "compute")
				return ShaderStage::Compute;

			return ShaderStage::None;
		}

		static constexpr ShaderStage ShaderStageFromExecutionModel(spv::ExecutionModel execModel)
		{
			switch (execModel)
			{
				case spv::ExecutionModelVertex:   return ShaderStage::Vertex;
				case spv::ExecutionModelGeometry: return ShaderStage::Geometry;
				case spv::ExecutionModelFragment: return ShaderStage::Fragment;
				case spv::ExecutionModelGLCompute:return ShaderStage::Compute;

				QK_ASSERT_NO_DEFAULT("Unknown shader stage");
			}

			return ShaderStage::None;
		}

		static constexpr const char* ShaderStageToString(ShaderStage stage)
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

		static void FindAndReplace(std::string_view token, std::string_view by, std::string& source)
		{
			size_t pos = 0;
			for (;;)
			{
				pos = source.find(token, token.size() + pos);
				if (pos == std::string::npos)
					break;

				source.replace(source.begin() + pos, source.begin() + token.size() + pos, by);
			}
		}
	}

	Shader::Shader(std::string_view name)
		: m_Name(name)
	{
		std::filesystem::create_directories(CacheDirectory);
	}

	void Shader::Reflect(ShaderStage stage, std::span<const uint32_t> spirvSource)
	{
		QK_PROFILE_FUNCTION();

		//                                           Number of words --v
		spirv_cross::Compiler compiler(spirvSource.data(), spirvSource.size());
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		QK_CORE_TRACE("Shader::Reflect - \"{0}\" at {1}", m_Name, Utils::ShaderStageToString(stage));
		QK_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		QK_CORE_TRACE("    {0} image samplers", resources.sampled_images.size());

		auto entryPoints = compiler.get_entry_points_and_stages();
		for (auto& entryPoint : entryPoints)
		{
			ShaderStage shaderStage = Utils::ShaderStageFromExecutionModel(entryPoint.execution_model);
			if (stage == shaderStage)
				m_Reflection.EntryPoints[stage] = entryPoint.name;
		}
		
		m_Reflection.BindingCount += (uint32_t)resources.uniform_buffers.size();
		m_Reflection.BindingCount += (uint32_t)resources.sampled_images.size();

		QK_CORE_TRACE("Push constants:");
		for (const auto& resource : resources.push_constant_buffers)
		{
			const auto& type = compiler.get_type(resource.type_id);

			size_t bufferSize = compiler.get_declared_struct_size(type);

			PushConstantResource pcResource{};
			pcResource.Size = bufferSize;
			pcResource.Stage = stage;
			pcResource.Name = compiler.get_name(resource.id);

			m_Reflection.PushConstants.push_back(pcResource);

			QK_CORE_TRACE(" \"{0} ({1})\"", resource.name, pcResource.Name);
			QK_CORE_TRACE("    Size = {0} bytes", bufferSize);
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

			m_Reflection.SamplerArrays.push_back(sResource);
			m_Reflection.DescriptorCount += samplerCount;

			QK_CORE_TRACE(" \"{0} ({1})\"", resource.name, sResource.Decorators.Name);
			QK_CORE_TRACE("    Set = {0}", set);
			QK_CORE_TRACE("    Binding = {0}", binding);
			QK_CORE_TRACE("    Samplers = {0}", samplerCount);
		}

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

			m_Reflection.UniformBuffers.push_back(ubResource);
			m_Reflection.DescriptorCount++;

			QK_CORE_TRACE(" \"{0} ({1})\"", resource.name, ubResource.Decorators.Name);
			QK_CORE_TRACE("    Set = {0}", set);
			QK_CORE_TRACE("    Binding = {0}", binding);
			QK_CORE_TRACE("    Size = {0} bytes", bufferSize);
			QK_CORE_TRACE("    Members = {0}", memberCount);
		}
	}

	std::string Shader::Parse(std::string_view source)
	{
		std::string parsedGlslSource = std::string(source);

		uint32_t maxTextureUnits = Renderer::GetCapabilities().Sampler.MaxTextureUnits;

		char buffer[4];
		std::to_chars(buffer, buffer + sizeof(buffer), maxTextureUnits);

		// Replace all occurences of "Quark.MaxTextureUnits"
		Utils::FindAndReplace("Quark.MaxTextureUnits", buffer, parsedGlslSource);

		return parsedGlslSource;
	}

	std::string_view Shader::ExtractNameFromPath(std::string_view filepath)
	{
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		return filepath.substr(lastSlash, count);
	}

	std::unordered_map<ShaderStage, std::string_view> Shader::SubstrStages(std::string_view source)
	{
		std::unordered_map<ShaderStage, std::string_view> shaderSources;

		static constexpr std::string_view typeToken = "#type";
		size_t pos = source.find(typeToken); // Start of shader type declaration line

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
			if (eol == std::string::npos)
			{
				QK_CORE_ERROR("Syntax error in shader");
				break;
			}

			size_t begin = pos + typeToken.size() + 1; // Start of shader type name (after "#type " keyword)
			std::string_view type = source.substr(begin, eol - begin);
			ShaderStage shaderType = Utils::ShaderTypeFromString(type);
			if (shaderType == ShaderStage::None)
			{
				QK_CORE_ERROR("Invalid shader type specifier");
				break;
			}

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			if (nextLinePos == std::string::npos)
			{
				QK_CORE_ERROR("Syntax error in shader");
				break;
			}

			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			shaderSources[shaderType] = (pos == std::string::npos)
				? source.substr(nextLinePos)
				: source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	Scope<Shader> Shader::Create(std::string_view filepath)
	{
		return s_GraphicsAPI->CreateShader(filepath);
	}

	Scope<Shader> Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSource, fragmentSource);
	}

	Scope<Shader> Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Shader> Shader::Create(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSpirv, fragmentSpirv);
	}

	Scope<Shader> Shader::Create(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv)
	{
		return s_GraphicsAPI->CreateShader(name, vertexSpirv, geometrySpirv, fragmentSpirv);
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
