#include "qkpch.h"
#include "Shader.h"

#include "GraphicsAPI.h"

namespace Quark {

	Shader* Shader::Create(std::string_view filepath)
	{
		return GraphicsAPI::Instance->CreateShader(filepath);
	}

	Shader* Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return GraphicsAPI::Instance->CreateShader(name, vertexSource, fragmentSource);
	}

	Shader* Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return GraphicsAPI::Instance->CreateShader(name, vertexSource, geometrySource, fragmentSource);
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	void ShaderLibrary::Add(std::string_view name, Ref<Shader> shader)
	{
		QK_CORE_ASSERT(!Exists(name), "Shader already exists! It was not added");
		m_Shaders[GetHashedName(name)] = std::move(shader);
	}

	void ShaderLibrary::Add(Ref<Shader> shader)
	{
		Add(shader->GetName(), std::move(shader));
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view filepath)
	{
		Ref<Shader> shader(Shader::Create(filepath));
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view name, std::string_view filepath)
	{
		Ref<Shader> shader(Shader::Create(filepath));
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(std::string_view name) const
	{
		QK_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders.at(GetHashedName(name));
	}

	bool ShaderLibrary::Exists(std::string_view name) const
	{
		return m_Shaders.find(GetHashedName(name)) != m_Shaders.end();
	}
}
