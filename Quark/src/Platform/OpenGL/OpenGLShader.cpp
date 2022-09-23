#include "qkpch.h"
#include "OpenGLShader.h"

#include "Quark/Filesystem/Filesystem.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.h>

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

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
				case GL_GEOMETRY_SHADER: return shaderc_glsl_geometry_shader;
				case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
				case GL_COMPUTE_SHADER:  return shaderc_glsl_compute_shader;

				QK_ASSERT_NO_DEFAULT("Unknown OpenGL shader stage");
			}

			return static_cast<shaderc_shader_kind>(GL_NONE);
		}

		static constexpr ShaderStage GetShaderStageFromOpenGLType(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return ShaderStage::Vertex;
				case GL_GEOMETRY_SHADER: return ShaderStage::Geometry;
				case GL_FRAGMENT_SHADER: return ShaderStage::Fragment;
				case GL_COMPUTE_SHADER:  return ShaderStage::Compute;

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
		auto shaderSources = SubstrStages(source);
		m_RendererID = CompileGLSLSources(shaderSources);
	}

	OpenGLShader::OpenGLShader(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, SpirvView> spirvSources(2);
		spirvSources[GL_VERTEX_SHADER]   = vertexSource;
		spirvSources[GL_FRAGMENT_SHADER] = fragmentSource;
		m_RendererID = CompileSPIRV(spirvSources);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : m_ShaderResources.SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, SpirvView> spirvSources(3);
		spirvSources[GL_VERTEX_SHADER]   = vertexSource;
		spirvSources[GL_GEOMETRY_SHADER] = geometrySource;
		spirvSources[GL_FRAGMENT_SHADER] = fragmentSource;
		m_RendererID = CompileSPIRV(spirvSources);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : m_ShaderResources.SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	std::unordered_map<GLenum, std::string_view> OpenGLShader::SubstrStages(std::string_view source)
	{
		std::unordered_map<GLenum, std::string_view> shaderSources;

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

	GLuint OpenGLShader::CompileGLSLSources(const std::unordered_map<GLenum, std::string_view>& shaderSources)
	{
		auto compiler = shaderc_compiler_initialize();
		auto options = shaderc_compile_options_initialize();

		shaderc_compile_options_set_source_language(options, shaderc_source_language_glsl);
		shaderc_compile_options_set_target_env(options, shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);

		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;
		for (auto&& [stage, source] : shaderSources)
		{
			auto result = shaderc_compile_into_spv(compiler, source.data(), source.size(),
				Utils::GLShaderStageToShaderC(stage), m_Name.c_str(), "main", options);

			if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(shaderc_result_get_error_message(result));
				shaderc_result_release(result);
				break;
			}

			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			size_t size = shaderc_result_get_length(result);
			const uint32_t* shaderData = (const uint32_t*)shaderc_result_get_bytes(result);

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V,
				(const void*)shaderData,
				(GLsizei)size);

			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(program, shader);

			Reflect(Utils::GetShaderStageFromOpenGLType(stage), shaderData, size / sizeof(uint32_t));
			shaderc_result_release(result);
		}

		shaderc_compiler_release(compiler);
		shaderc_compile_options_release(options);

		GLint isLinked = GL_FALSE;
		GLint isCompiled = glShaderIDIndex == shaderSources.size();

		if (isCompiled)
		{
			isLinked = LinkProgram(program);
		}
		
		for (uint32_t i = 0; i < glShaderIDIndex; i++)
		{
			glDetachShader(program, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}

		if (!isLinked || !isCompiled)
		{
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	GLuint OpenGLShader::CompileGLSLSourcesLegacy(const std::unordered_map<GLenum, std::string_view>& shaderSources)
	{
		GLuint program = glCreateProgram();

		GLint isCompiled = GL_FALSE;
		GLint isLinked = GL_FALSE;

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

			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

				QK_CORE_ERROR("Shader compilation failure (compiling: {0}):\n{1}", m_Name, (const GLchar*)infoLog);
				break;
			}

			glAttachShader(program, shader);
		}

		if (isCompiled)
		{
			isLinked = LinkProgram(program);
		}

		for (uint32_t i = 0; i < glShaderIDIndex; i++)
		{
			glDetachShader(program, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}

		if (!isCompiled || !isLinked)
		{
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	GLuint OpenGLShader::CompileSPIRV(const std::unordered_map<GLenum, SpirvView>& spirvBinaries)
	{
		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;
		for (auto& [stage, binary] : spirvBinaries)
		{
			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V,
				(const void*)binary.Data,
				(GLsizei)(binary.WordCount * sizeof(uint32_t)));

			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(program, shader);

			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary.Data, binary.WordCount);
		}

		GLint isLinked = LinkProgram(program);
		for (uint32_t i = 0; i < glShaderIDIndex; i++)
		{
			glDetachShader(program, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}

		if (!isLinked)
		{
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	GLint OpenGLShader::LinkProgram(GLuint program)
	{
		glLinkProgram(program);

		GLint isLinked = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

			QK_CORE_ERROR("Shader link failure (linking {0}):\n{1}", m_Name, (const GLchar*)infoLog);
		}

		return isLinked;
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

	bool OpenGLShader::operator==(const Shader& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
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
		if (location == -1)
		{
			QK_CORE_WARN("glGetUniformLocation returned an invalid location");
			return -1;
		}

		m_UniformLocationCache[hash] = location;
		return location;
	}
}
