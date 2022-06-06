#include "qkpch.h"
#include "Shader.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<Shader> Shader::Create(std::string_view filepath)
	{
		return RenderCommand::CreateShader(filepath);
	}

	Ref<Shader> Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return RenderCommand::CreateShader(name, vertexSource, fragmentSource);
	}

	Ref<Shader> Shader::Create(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return RenderCommand::CreateShader(name, vertexSource, geometrySource, fragmentSource);
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	void ShaderLibrary::Add(std::string_view name, const Ref<Shader>& shader)
	{
		QK_ASSERT(!Exists(name), "Shader already exists! It was not added");
		m_Shaders[GetHashedName(name)] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		Add(shader->GetName(), shader);
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(std::string_view name, std::string_view filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	const Ref<Shader>& ShaderLibrary::Get(std::string_view name) const
	{
		QK_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders.at(GetHashedName(name));
	}

	bool ShaderLibrary::Exists(std::string_view name) const
	{
		return m_Shaders.find(GetHashedName(name)) != m_Shaders.end();
	}
}
