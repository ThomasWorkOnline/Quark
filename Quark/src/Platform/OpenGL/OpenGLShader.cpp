#include "qkpch.h"
#include "OpenGLShader.h"

#include "Quark/Filesystem/Filesystem.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_map>
#include <sstream>

namespace Quark {

	namespace Utils {

		static constexpr GLenum ShaderTypeFromString(std::string_view type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			else if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			else if (type == "geometry")
				return GL_GEOMETRY_SHADER;
			else if (type == "compute")
				return GL_COMPUTE_SHADER;

			return GL_NONE;
		}

		static constexpr ShaderStage GetShaderStageFromOpenGLType(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return ShaderStage::VertexStage;
				case GL_GEOMETRY_SHADER: return ShaderStage::GeometryStage;
				case GL_FRAGMENT_SHADER: return ShaderStage::FragmentStage;
				case GL_COMPUTE_SHADER:  return ShaderStage::ComputeStage;

				QK_ASSERT_NO_DEFAULT("Unknown OpenGL shader stage");
			}

			return ShaderStage::None;
		}

		static constexpr std::string_view ExtractNameFromPath(std::string_view filepath)
		{
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return filepath.substr(lastSlash, count);
		}
	}

	static constexpr size_t s_MaxShaders = 3;

	OpenGLShader::OpenGLShader(std::string_view filepath)
		: Shader(Utils::ExtractNameFromPath(filepath))
	{
		QK_PROFILE_FUNCTION();

		std::string source = Filesystem::ReadTextFile(filepath);
		auto shaderSources = PreProcess(source);
		m_RendererID = CompileSources(shaderSources);
	}

	OpenGLShader::OpenGLShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource)
		: Shader(name), m_SpirvSources(2)
	{
		QK_PROFILE_FUNCTION();

		m_SpirvSources[GL_VERTEX_SHADER]   = vertexSource;
		m_SpirvSources[GL_FRAGMENT_SHADER] = fragmentSource;
		m_RendererID = CompileSPIRV(m_SpirvSources);
	}

	OpenGLShader::OpenGLShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource)
		: Shader(name), m_SpirvSources(3)
	{
		QK_PROFILE_FUNCTION();

		m_SpirvSources[GL_VERTEX_SHADER]   = vertexSource;
		m_SpirvSources[GL_FRAGMENT_SHADER] = fragmentSource;
		m_SpirvSources[GL_GEOMETRY_SHADER] = geometrySource;
		m_RendererID = CompileSPIRV(m_SpirvSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(std::string_view source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		static constexpr std::string_view typeToken = "#type";
		size_t pos = source.find(typeToken); // Start of shader type declaration line

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
			if (eol == std::string::npos)
			{
				QK_CORE_ERROR("Syntax error in shader");
				break;
			}

			size_t begin = pos + typeToken.size() + 1; // Start of shader type name (after "#type " keyword)
			std::string_view type = source.substr(begin, eol - begin);
			GLenum shaderType = Utils::ShaderTypeFromString(type);
			if (shaderType == GL_NONE)
			{
				QK_CORE_ERROR("Invalid shader type specifier");
				break;
			}

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			if (nextLinePos == std::string::npos)
			{
				QK_CORE_ERROR("Syntax error in shader");
				break;
			}

			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			shaderSources[shaderType] = (pos == std::string::npos)
				? source.substr(nextLinePos)
				: source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	GLuint OpenGLShader::CompileSources(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		QK_CORE_ASSERT(shaderSources.size() <= s_MaxShaders, "Maximum number of shaders per program is 3");

		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;
		for (auto& [stage, source] : shaderSources)
		{
			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			const GLchar* sourceData = source.data();
			const GLint length = static_cast<GLint>(source.size());
			glShaderSource(shader, 1, &sourceData, &length);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

				for (uint32_t i = 0; i < glShaderIDIndex; i++)
				{
					glDeleteShader(glShaderIDs[i]);
				}

				glDeleteProgram(program);

				QK_CORE_ERROR("Shader compilation failure (compiling: {0}):\n{1}", m_Name, (const GLchar*)infoLog);
				return 0;
			}

			glAttachShader(program, shader);
		}

		LinkProgram(program, glShaderIDs, glShaderIDIndex);
		return program;
	}

	GLuint OpenGLShader::CompileSPIRV(const std::unordered_map<GLenum, SpirvSource>& spirvBinaries)
	{
		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;
		for (auto& [stage, binary] : spirvBinaries)
		{
			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V,
				(const void*)binary.data(),
				(GLsizei)(binary.size() * sizeof(uint32_t)));

			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(program, shader);

			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);
		}

		LinkProgram(program, glShaderIDs, glShaderIDIndex);
		return program;
	}

	void OpenGLShader::LinkProgram(GLuint program, const GLenum* glShaderIDs, uint32_t shaderCount)
	{
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

			for (uint32_t i = 0; i < shaderCount; i++)
			{
				glDetachShader(program, glShaderIDs[i]);
				glDeleteShader(glShaderIDs[i]);
			}

			glDeleteProgram(program);

			QK_CORE_ERROR("Shader link failure (linking {0}):\n{1}", m_Name, (const GLchar*)infoLog);
			return;
		}

		for (uint32_t i = 0; i < shaderCount; i++)
		{
			glDetachShader(program, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}
	}

	void OpenGLShader::SetInt(std::string_view name, int32_t value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetVec2i(std::string_view name, const Vec2i& value)
	{
		UploadUniformInt2(name, value);
	}

	void OpenGLShader::SetVec3i(std::string_view name, const Vec3i& value)
	{
		UploadUniformInt3(name, value);
	}

	void OpenGLShader::SetVec4i(std::string_view name, const Vec4i& value)
	{
		UploadUniformInt4(name, value);
	}

	void OpenGLShader::SetIntArray(std::string_view name, const int32_t* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(std::string_view name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetVec2f(std::string_view name, const Vec2f& value)
	{
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetVec3f(std::string_view name, const Vec3f& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetVec4f(std::string_view name, const Vec4f& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetFloatArray(std::string_view name, const float* values, uint32_t count)
	{
		UploadUniformFloatArray(name, values, count);
	}

	void OpenGLShader::SetDouble(std::string_view name, double value)
	{
		UploadUniformDouble(name, value);
	}

	void OpenGLShader::SetVec2d(std::string_view name, const Vec2d& value)
	{
		UploadUniformDouble2(name, value);
	}

	void OpenGLShader::SetVec3d(std::string_view name, const Vec3d& value)
	{
		UploadUniformDouble3(name, value);
	}

	void OpenGLShader::SetVec4d(std::string_view name, const Vec4d& value)
	{
		UploadUniformDouble4(name, value);
	}

	void OpenGLShader::SetDoubleArray(std::string_view name, const double* values, uint32_t count)
	{
		UploadUniformDoubleArray(name, values, count);
	}

	void OpenGLShader::SetMat3f(std::string_view name, const Mat3f& matrix)
	{
		UploadUniformMat3f(name, matrix);
	}

	void OpenGLShader::SetMat4f(std::string_view name, const Mat4f& matrix)
	{
		UploadUniformMat4f(name, matrix);
	}

	void OpenGLShader::SetMat3d(std::string_view name, const Mat3d& matrix)
	{
		UploadUniformMat3d(name, matrix);
	}

	void OpenGLShader::SetMat4d(std::string_view name, const Mat4d& matrix)
	{
		UploadUniformMat4d(name, matrix);
	}

	void OpenGLShader::UploadUniformInt(std::string_view name, int32_t value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1i(m_RendererID, location, value);
	}

	void OpenGLShader::UploadUniformInt2(std::string_view name, const Vec2i& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform2i(m_RendererID, location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformInt3(std::string_view name, const Vec3i& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform3i(m_RendererID, location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformInt4(std::string_view name, const Vec4i& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform4i(m_RendererID, location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformIntArray(std::string_view name, const int32_t* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1iv(m_RendererID, location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(std::string_view name, float value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1f(m_RendererID, location, value);
	}

	void OpenGLShader::UploadUniformFloat2(std::string_view name, const Vec2f& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform2f(m_RendererID, location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(std::string_view name, const Vec3f& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform3f(m_RendererID, location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(std::string_view name, const Vec4f& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform4f(m_RendererID, location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformFloatArray(std::string_view name, const float* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1fv(m_RendererID, location, count, values);
	}

	void OpenGLShader::UploadUniformDouble(std::string_view name, double value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1d(m_RendererID, location, value);
	}

	void OpenGLShader::UploadUniformDouble2(std::string_view name, const Vec2d& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform2d(m_RendererID, location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformDouble3(std::string_view name, const Vec3d& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform3d(m_RendererID, location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformDouble4(std::string_view name, const Vec4d& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform4d(m_RendererID, location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformDoubleArray(std::string_view name, const double* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1dv(m_RendererID, location, count, values);
	}

	void OpenGLShader::UploadUniformMat3f(std::string_view name, const Mat3f& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix3fv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4f(std::string_view name, const Mat4f& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix4fv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3d(std::string_view name, const Mat3d& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix3dv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4d(std::string_view name, const Mat4d& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix4dv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	GLint OpenGLShader::GetUniformLocation(std::string_view name) const
	{
		size_t hash = std::hash<std::string_view>()(name);
		auto it = m_UniformLocationCache.find(hash);
		if (it != m_UniformLocationCache.end())
			return it->second;

		// Insert uniform location into cache
		GLint location = glGetUniformLocation(m_RendererID, (const GLchar*)name.data());
		m_UniformLocationCache[hash] = location;

		return location;
	}
}
