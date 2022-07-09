#pragma once

#include "Quark/Renderer/Shader.h"

#include <unordered_map>

typedef unsigned int GLenum;
typedef int GLint;

namespace Quark {

	class OpenGLShader final : public Shader
	{
	public:
		OpenGLShader(std::string_view filepath);
		OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource);
		OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
		virtual ~OpenGLShader() override;

		virtual void Attach() const override;
		virtual void Detach() const override;

		virtual void SetInt(std::string_view name, int32_t value) override;
		virtual void SetInt2(std::string_view name, const Vec2i& value) override;
		virtual void SetInt3(std::string_view name, const Vec3i& value) override;
		virtual void SetInt4(std::string_view name, const Vec4i& value) override;
		virtual void SetIntArray(std::string_view name, int32_t* values, uint32_t count) override;

		virtual void SetFloat(std::string_view name, float value) override;
		virtual void SetFloat2(std::string_view name, const Vec2f& value) override;
		virtual void SetFloat3(std::string_view name, const Vec3f& value) override;
		virtual void SetFloat4(std::string_view name, const Vec4f& value) override;
		virtual void SetFloatArray(std::string_view name, float* values, uint32_t count) override;

		virtual void SetDouble(std::string_view name, double value) override;
		virtual void SetDouble2(std::string_view name, const Vec2d& value) override;
		virtual void SetDouble3(std::string_view name, const Vec3d& value) override;
		virtual void SetDouble4(std::string_view name, const Vec4d& value) override;
		virtual void SetDoubleArray(std::string_view name, double* values, uint32_t count) override;

		virtual void SetMat3(std::string_view name, const Mat3f& matrix) override;
		virtual void SetMat4(std::string_view name, const Mat4f& matrix) override;

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) override;
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) override;

		virtual std::string_view GetName() const override { return m_Name; }

		virtual bool operator==(const Shader& other) const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		}

	private:
		void UploadUniformInt(std::string_view name, int32_t value);
		void UploadUniformInt2(std::string_view name, const Vec2i& value);
		void UploadUniformInt3(std::string_view name, const Vec3i& value);
		void UploadUniformInt4(std::string_view name, const Vec4i& value);
		void UploadUniformIntArray(std::string_view name, int32_t* values, uint32_t count);

		void UploadUniformFloat(std::string_view name, float value);
		void UploadUniformFloat2(std::string_view name, const Vec2f& value);
		void UploadUniformFloat3(std::string_view name, const Vec3f& value);
		void UploadUniformFloat4(std::string_view name, const Vec4f& value);
		void UploadUniformFloatArray(std::string_view name, float* values, uint32_t count);

		void UploadUniformDouble(std::string_view name, double value);
		void UploadUniformDouble2(std::string_view name, const Vec2d& value);
		void UploadUniformDouble3(std::string_view name, const Vec3d& value);
		void UploadUniformDouble4(std::string_view name, const Vec4d& value);
		void UploadUniformDoubleArray(std::string_view name, double* values, uint32_t count);

		void UploadUniformMat3(std::string_view name, const Mat3f& matrix);
		void UploadUniformMat4(std::string_view name, const Mat4f& matrix);
		void UploadUniformMat3d(std::string_view name, const Mat3d& matrix);
		void UploadUniformMat4d(std::string_view name, const Mat4d& matrix);

		GLint GetUniformLocation(std::string_view name);

		std::unordered_map<GLenum, std::string_view> PreProcess(std::string_view source);
		void Compile(const std::unordered_map<GLenum, std::string_view>& shaderSources);

	private:
		uint32_t m_RendererID;
		std::string m_Name;

		std::unordered_map<std::size_t, GLint> m_UniformLocations;
	};
}
