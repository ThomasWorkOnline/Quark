#pragma once

#include "../../Quark/Renderer/Shader.h"

namespace Quark {

	typedef uint32_t GLenum;

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void Attach() const;
		virtual void Detach() const;

		virtual void SetInt(const std::string& name, int32_t value) override;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) override;
		virtual void SetInt3(const std::string& name, const glm::ivec3& value) override;
		virtual void SetInt4(const std::string& name, const glm::ivec4& value) override;
		virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) override;

		virtual void SetDouble(const std::string& name, double value) override;
		virtual void SetDouble2(const std::string& name, const glm::dvec2& value) override;
		virtual void SetDouble3(const std::string& name, const glm::dvec3& value) override;
		virtual void SetDouble4(const std::string& name, const glm::dvec4& value) override;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Shader& other) const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		}

		virtual const std::string& GetName() const override { return m_Name; }

	private:
		void UploadUniformInt(const std::string& name, int32_t value);
		void UploadUniformInt2(const std::string& name, const glm::ivec2& value);
		void UploadUniformInt3(const std::string& name, const glm::ivec3& value);
		void UploadUniformInt4(const std::string& name, const glm::ivec4& value);
		void UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformFloatArray(const std::string& name, float* values, uint32_t count);

		void UploadUniformDouble(const std::string& name, double value);
		void UploadUniformDouble2(const std::string& name, const glm::dvec2& value);
		void UploadUniformDouble3(const std::string& name, const glm::dvec3& value);
		void UploadUniformDouble4(const std::string& name, const glm::dvec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		uint32_t m_RendererID;
		std::string m_Name;
	};
}
