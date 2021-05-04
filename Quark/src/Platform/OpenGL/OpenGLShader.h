#pragma once

#include "../../Quark/Renderer/Shader.h"

namespace Quark {

	typedef uint32_t GLenum;

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void Attach() const;
		virtual void Detach() const;

		virtual void SetInt(const std::string& name, int32_t value) override;
		virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) override;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual bool operator==(const Resource& other) const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		}

		void UploadUniformInt(const std::string& name, int32_t value);
		void UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformFloatArray(const std::string& name, float* values, uint32_t count);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		virtual const std::string& GetName() const override { return m_Name; }
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		uint32_t m_RendererID = 0;
		std::string m_Name;
	};
}
