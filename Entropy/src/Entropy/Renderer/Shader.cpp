#include "Shader.h"

#include "RenderingAPI.h"

// Include all supported API's shader implementations
#include "../../Platform/OpenGL/OpenGLShader.h"

namespace Entropy {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, Ref<Shader> shader)
	{
		if (!Exists(name))
			m_Shaders[name] = shader;
		else
			NT_WARN("Shader already exists! It was not added");
	}

	void ShaderLibrary::Add(Ref<Shader> shader)
	{
		const std::string& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
			NT_FATAL("Shader not found!");

		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}
