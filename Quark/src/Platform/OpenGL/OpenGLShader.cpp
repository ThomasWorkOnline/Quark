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

			return GL_NONE;
		}

		static constexpr std::string_view ExtractNameFromPath(std::string_view filepath)
		{
			// Extract name from filepath
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return filepath.substr(lastSlash, count);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view filepath)
		: m_Name(Utils::ExtractNameFromPath(filepath))
	{
		QK_PROFILE_FUNCTION();

		std::string source = Filesystem::ReadTextFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		: m_Name(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string_view> sources;
		sources[GL_VERTEX_SHADER]   = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		: m_Name(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string_view> sources;
		sources[GL_VERTEX_SHADER]   = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		sources[GL_GEOMETRY_SHADER] = geometrySource;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	std::unordered_map<GLenum, std::string_view> OpenGLShader::PreProcess(std::string_view source)
	{
		std::unordered_map<GLenum, std::string_view> shaderSources;

		static constexpr std::string_view typeToken = "#type";
		static constexpr size_t typeTokenLength = typeToken.size();
		size_t pos = source.find(typeToken); // Start of shader type declaration line

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
			if (eol == std::string::npos)
			{
				QK_CORE_ERROR("Syntax error in shader");
				break;
			}

			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			std::string_view type = source.substr(begin, eol - begin);
			GLenum shaderType = Utils::ShaderTypeFromString(type);
			if (shaderType == GL_NONE)
			{
				QK_CORE_ERROR("Invalid shader type specified");
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

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string_view>& shaderSources)
	{
		static constexpr size_t maxShaders = 3;
		QK_CORE_ASSERT(shaderSources.size() <= maxShaders, "Maximum number of shaders per program is 3");

		GLuint program = glCreateProgram();

		GLenum glShaderIDs[maxShaders]{};
		uint32_t glShaderIDIndex = 0;
		for (auto& [type, source] : shaderSources)
		{
			GLuint shader = glCreateShader(type);
			glShaderIDs[glShaderIDIndex++] = shader;

			const GLchar* sourceData = source.data();
			GLint lengths = static_cast<GLint>(source.size());
			glShaderSource(shader, 1, &sourceData, &lengths);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

				for (uint32_t i = 0; i < glShaderIDIndex; i++)
				{
					glDeleteShader(glShaderIDs[i]);
				}

				glDeleteProgram(program);

				QK_CORE_ERROR("Shader compilation failure (compiling: {0}):\n{1}", m_Name, (const GLchar*)infoLog.data());
				return;
			}

			glAttachShader(program, shader);
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

			for (uint32_t i = 0; i < glShaderIDIndex; i++)
			{
				glDetachShader(program, glShaderIDs[i]);
				glDeleteShader(glShaderIDs[i]);
			}

			glDeleteProgram(program);

			QK_CORE_ERROR("Shader link failure (linking {0}):\n{1}", m_Name, (const GLchar*)infoLog.data());
			return;
		}

		for (uint32_t i = 0; i < glShaderIDIndex; i++)
		{
			glDetachShader(program, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Attach() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Detach() const
	{
		glUseProgram(0);
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
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformInt2(std::string_view name, const Vec2i& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2i(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformInt3(std::string_view name, const Vec3i& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3i(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformInt4(std::string_view name, const Vec4i& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformIntArray(std::string_view name, const int32_t* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(std::string_view name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(std::string_view name, const Vec2f& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(std::string_view name, const Vec3f& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(std::string_view name, const Vec4f& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformFloatArray(std::string_view name, const float* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1fv(location, count, values);
	}

	void OpenGLShader::UploadUniformDouble(std::string_view name, double value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1d(location, value);
	}

	void OpenGLShader::UploadUniformDouble2(std::string_view name, const Vec2d& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2d(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformDouble3(std::string_view name, const Vec3d& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3d(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformDouble4(std::string_view name, const Vec4d& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4d(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformDoubleArray(std::string_view name, const double* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1dv(location, count, values);
	}

	void OpenGLShader::UploadUniformMat3f(std::string_view name, const Mat3f& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4f(std::string_view name, const Mat4f& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3d(std::string_view name, const Mat3d& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4d(std::string_view name, const Mat4d& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(matrix));
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
