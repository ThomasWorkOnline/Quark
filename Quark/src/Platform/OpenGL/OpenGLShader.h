#pragma once

#include "Quark/Renderer/Shader.h"

#include <unordered_map>
#include <vector>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;

namespace Quark {

	class OpenGLShader final : public Shader
	{
	public:
		OpenGLShader(std::string_view filepath);
		OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource);
		OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
		OpenGLShader(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv);
		OpenGLShader(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv);

		virtual ~OpenGLShader() final override;

		virtual void SetInt(std::string_view name, int32_t value) final override;
		virtual void SetVec2i(std::string_view name, const Vec2i& value) final override;
		virtual void SetVec3i(std::string_view name, const Vec3i& value) final override;
		virtual void SetVec4i(std::string_view name, const Vec4i& value) final override;
		virtual void SetIntArray(std::string_view name, const int32_t* values, uint32_t count) final override;

		virtual void SetFloat(std::string_view name, float value) final override;
		virtual void SetVec2f(std::string_view name, const Vec2f& value) final override;
		virtual void SetVec3f(std::string_view name, const Vec3f& value) final override;
		virtual void SetVec4f(std::string_view name, const Vec4f& value) final override;
		virtual void SetFloatArray(std::string_view name, const float* values, uint32_t count) final override;

		virtual void SetDouble(std::string_view name, double value) final override;
		virtual void SetVec2d(std::string_view name, const Vec2d& value) final override;
		virtual void SetVec3d(std::string_view name, const Vec3d& value) final override;
		virtual void SetVec4d(std::string_view name, const Vec4d& value) final override;
		virtual void SetDoubleArray(std::string_view name, const double* values, uint32_t count) final override;

		virtual void SetMat3f(std::string_view name, const Mat3f& matrix) final override;
		virtual void SetMat4f(std::string_view name, const Mat4f& matrix) final override;

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) final override;
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) final override;

		virtual bool operator==(const Shader& other) const final override;

		// Non-Copyable
		OpenGLShader(const OpenGLShader&) = delete;
		OpenGLShader& operator=(const OpenGLShader&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }
		
	private:
		void UploadUniformInt(std::string_view name, int32_t value);
		void UploadUniformInt2(std::string_view name, const Vec2i& value);
		void UploadUniformInt3(std::string_view name, const Vec3i& value);
		void UploadUniformInt4(std::string_view name, const Vec4i& value);
		void UploadUniformIntArray(std::string_view name, const int32_t* values, uint32_t count);

		void UploadUniformFloat(std::string_view name, float value);
		void UploadUniformFloat2(std::string_view name, const Vec2f& value);
		void UploadUniformFloat3(std::string_view name, const Vec3f& value);
		void UploadUniformFloat4(std::string_view name, const Vec4f& value);
		void UploadUniformFloatArray(std::string_view name, const float* values, uint32_t count);

		void UploadUniformDouble(std::string_view name, double value);
		void UploadUniformDouble2(std::string_view name, const Vec2d& value);
		void UploadUniformDouble3(std::string_view name, const Vec3d& value);
		void UploadUniformDouble4(std::string_view name, const Vec4d& value);
		void UploadUniformDoubleArray(std::string_view name, const double* values, uint32_t count);

		void UploadUniformMat3f(std::string_view name, const Mat3f& matrix);
		void UploadUniformMat4f(std::string_view name, const Mat4f& matrix);
		void UploadUniformMat3d(std::string_view name, const Mat3d& matrix);
		void UploadUniformMat4d(std::string_view name, const Mat4d& matrix);

		void CompileOrReadFromCache(const std::unordered_map<GLenum, std::string_view>& shaderSources);

		GLuint CompileVulkanSources(const std::unordered_map<GLenum, std::string>& shaderSources, uint32_t glslVersion = GetGLSLVersion());
		GLuint CompileVulkanSourcesLegacy(const std::unordered_map<GLenum, std::string>& shaderSources, uint32_t glslVersion = GetGLSLVersion());
		GLuint CompileVulkanSpirv(const std::unordered_map<GLenum, std::span<const uint32_t>>& spirvBinaries, uint32_t glslVersion = GetGLSLVersion());
		
		GLuint CreateProgram(const std::unordered_map<GLenum, std::vector<uint32_t>>& spirvBinaries);
		GLuint CreateProgram(const std::unordered_map<GLenum, std::string>& shaderSources);

		GLint LinkProgram(GLuint program);
		GLint GetUniformLocation(std::string_view name) const;

		static bool IsLegacyCompiler();
		static uint32_t GetGLSLVersion();

	private:
		GLuint m_RendererID = 0;
		mutable std::unordered_map<size_t, GLint> m_UniformLocationCache;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSpirv;
	};
}
