#include "qkpch.h"
#include "Shader.h"

#include "GraphicsAPI.h"

// Include all supported API's shader implementations
#include "Platform/OpenGL/OpenGLShader.h"

namespace Quark {

	Ref<Shader> Shader::Create(std::string_view filepath)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLShader>(filepath);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}

	Ref<Shader> Shader::Create(const std::string& name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}

	Ref<Shader> Shader::Create(const std::string& name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLShader>(name, vertexSource, geometrySource, fragmentSource);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
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
