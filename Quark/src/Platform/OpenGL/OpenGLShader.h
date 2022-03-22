#pragma once

#include "../../Quark/Renderer/Shader.h"

#include <unordered_map>

typedef uint32_t GLenum;
typedef int32_t GLint;

namespace Quark {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(std::string_view filepath);
		OpenGLShader(const std::string& name, std::string_view vertexSource, std::string_view fragmentSource);
		OpenGLShader(const std::string& name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
		virtual ~OpenGLShader();

		virtual void Attach() const;
		virtual void Detach() const;

		virtual void SetInt(std::string_view name, int32_t value) override;
		virtual void SetInt2(std::string_view name, const glm::ivec2& value) override;
		virtual void SetInt3(std::string_view name, const glm::ivec3& value) override;
		virtual void SetInt4(std::string_view name, const glm::ivec4& value) override;
		virtual void SetIntArray(std::string_view name, int32_t* values, uint32_t count) override;

		virtual void SetFloat(std::string_view name, float value) override;
		virtual void SetFloat2(std::string_view name, const glm::vec2& value) override;
		virtual void SetFloat3(std::string_view name, const glm::vec3& value) override;
		virtual void SetFloat4(std::string_view name, const glm::vec4& value) override;
		virtual void SetFloatArray(std::string_view name, float* values, uint32_t count) override;

		virtual void SetDouble(std::string_view name, double value) override;
		virtual void SetDouble2(std::string_view name, const glm::dvec2& value) override;
		virtual void SetDouble3(std::string_view name, const glm::dvec3& value) override;
		virtual void SetDouble4(std::string_view name, const glm::dvec4& value) override;
		virtual void SetDoubleArray(std::string_view name, double* values, uint32_t count) override;

		virtual void SetMat3(std::string_view name, const glm::mat3& matrix) override;
		virtual void SetMat4(std::string_view name, const glm::mat4& matrix) override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual bool operator==(const Shader& other) const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		}

	private:
		void UploadUniformInt(std::string_view name, int32_t value);
		void UploadUniformInt2(std::string_view name, const glm::ivec2& value);
		void UploadUniformInt3(std::string_view name, const glm::ivec3& value);
		void UploadUniformInt4(std::string_view name, const glm::ivec4& value);
		void UploadUniformIntArray(std::string_view name, int32_t* values, uint32_t count);

		void UploadUniformFloat(std::string_view name, float value);
		void UploadUniformFloat2(std::string_view name, const glm::vec2& value);
		void UploadUniformFloat3(std::string_view name, const glm::vec3& value);
		void UploadUniformFloat4(std::string_view name, const glm::vec4& value);
		void UploadUniformFloatArray(std::string_view name, float* values, uint32_t count);

		void UploadUniformDouble(std::string_view name, double value);
		void UploadUniformDouble2(std::string_view name, const glm::dvec2& value);
		void UploadUniformDouble3(std::string_view name, const glm::dvec3& value);
		void UploadUniformDouble4(std::string_view name, const glm::dvec4& value);
		void UploadUniformDoubleArray(std::string_view name, double* values, uint32_t count);

		void UploadUniformMat3(std::string_view name, const glm::mat3& matrix);
		void UploadUniformMat4(std::string_view name, const glm::mat4& matrix);

		GLint GetUniformLocation(std::string_view name);

	private:
		std::unordered_map<GLenum, std::string_view> PreProcess(std::string_view source);
		void Compile(const std::unordered_map<GLenum, std::string_view>& shaderSources);

		uint32_t m_RendererID;
		std::string m_Name;

		std::unordered_map<std::size_t, GLint> m_UniformLocations;
	};
}
