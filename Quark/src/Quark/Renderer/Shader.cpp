#include "Shader.h"

#include "RenderingAPI.h"
#include "DeferredObjectDeleter.h"

// Include all supported API's shader implementations
#include "../../Platform/OpenGL/OpenGLShader.h"

#include <xhash>
#include <filesystem>

namespace Quark {

	Ref<Shader> Shader::Create(std::string_view filepath)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLShader, DeferredObjectDeleter>(filepath);
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLShader, DeferredObjectDeleter>(name, vertexSource, fragmentSource);
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLShader, DeferredObjectDeleter>(name, vertexSource, geometrySource, fragmentSource);
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}
		return nullptr;
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	void ShaderLibrary::Add(std::string_view name, const Ref<Shader>& shader)
	{
		if (!Exists(name))
			m_Shaders[GetHashedName(name)] = shader;
		else
			QK_CORE_WARN("Shader already exists! It was not added");
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
		if (!Exists(name))
			QK_FATAL("Shader not found!");

		return m_Shaders.at(GetHashedName(name));
	}

	bool ShaderLibrary::Exists(std::string_view name) const
	{
		return m_Shaders.find(GetHashedName(name)) != m_Shaders.end();
	}
}
