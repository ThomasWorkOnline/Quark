#include "qkpch.h"
#include "Shader.h"

#include "GraphicsAPI.h"

#include <fstream>

namespace Quark {

	Scope<Shader> Shader::Create(std::string_view filepath)
	{
		return GraphicsAPI::Instance->CreateShader(filepath);
	}

	Scope<Shader> Shader::Create(std::string_view name, SpirvSource vertexSource, SpirvSource fragmentSource)
	{
		return GraphicsAPI::Instance->CreateShader(name, vertexSource, fragmentSource);
	}

	Scope<Shader> Shader::Create(std::string_view name, SpirvSource vertexSource, SpirvSource geometrySource, SpirvSource fragmentSource)
	{
		return GraphicsAPI::Instance->CreateShader(name, vertexSource, geometrySource, fragmentSource);
	}

	std::vector<uint32_t> ReadSpirvFile(std::string_view filepath)
	{
		std::vector<uint32_t> result;
		std::ifstream in(filepath.data(), std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			QK_RUNTIME_VERIFY(size % sizeof(uint32_t) == 0, "Invalid byte alignment for file: '{0}' (SPIR-V required 4-byte alignment)", filepath);

			if (size != -1)
			{
				result.resize(size / 4);
				in.seekg(0, std::ios::beg);
				in.read((char*)result.data(), size);
				in.close();

				return result;
			}
		}

		QK_RUNTIME_ERROR("Could not open file '{0}'", filepath);
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
