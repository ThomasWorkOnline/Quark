#include "qkpch.h"
#include "OpenGLShader.h"

#include "OpenGLEnums.h"

#include "Quark/Filesystem/Filesystem.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Quark {

	namespace Utils {

		static constexpr shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
				case GL_GEOMETRY_SHADER: return shaderc_glsl_geometry_shader;
				case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
				case GL_COMPUTE_SHADER:  return shaderc_glsl_compute_shader;

				QK_ASSERT_NO_DEFAULT("Unknown OpenGL shader stage");
			}

			return static_cast<shaderc_shader_kind>(0);
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

		static constexpr std::string GetCacheExtension(const std::string& name, GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return name + ".opengl.vert.spv";
				case GL_GEOMETRY_SHADER: return name + ".opengl.geo.spv";
				case GL_FRAGMENT_SHADER: return name + ".opengl.frag.spv";
				case GL_COMPUTE_SHADER:  return name + ".opengl.comp.spv";

				QK_ASSERT_NO_DEFAULT("Unknown OpenGL shader stage");
			}

			return {};
		}
	}

	static constexpr size_t s_MaxShaders = 3;

	OpenGLShader::OpenGLShader(std::string_view filepath)
		: Shader(ExtractNameFromPath(filepath))
	{
		QK_PROFILE_FUNCTION();

		std::string source = Filesystem::ReadTextFile(filepath);
		auto shaderSources = SubstrStages(source);

		std::unordered_map<GLenum, std::string_view> glslSources;
		for (auto&& [stage, source] : shaderSources)
			glslSources[ShaderStageToOpenGL(stage)] = source;

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_OpenGLSpirv)
			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : GetReflection().SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string_view> glslSources = {
			{ GL_VERTEX_SHADER,   vertexSource   },
			{ GL_FRAGMENT_SHADER, fragmentSource }
		};

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_OpenGLSpirv)
			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : GetReflection().SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string_view> glslSources = {
			{ GL_VERTEX_SHADER,   vertexSource   },
			{ GL_VERTEX_SHADER,   geometrySource },
			{ GL_FRAGMENT_SHADER, fragmentSource }
		};

		CompileOrReadFromCache(glslSources);

		for (auto&& [stage, binary] : m_OpenGLSpirv)
			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : GetReflection().SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::span<const uint32_t>> spirvSources = {
			{ GL_VERTEX_SHADER,   vertexSpirv   },
			{ GL_FRAGMENT_SHADER, fragmentSpirv }
		};

		m_RendererID = IsLegacyCompiler()
			? 0
			: CompileVulkanSpirv(spirvSources);

		for (auto&& [stage, binary] : m_OpenGLSpirv)
			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : GetReflection().SamplerArrays)
		{
			AutoRelease<int32_t> samplerIndexes = StackAlloc(samplerArray.SamplerCount * sizeof(int32_t));
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
				samplerIndexes[i] = (int32_t)i;

			UploadUniformIntArray(samplerArray.Decorators.Name, samplerIndexes, samplerArray.SamplerCount);
		}
	}

	OpenGLShader::OpenGLShader(std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv)
		: Shader(name)
	{
		QK_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::span<const uint32_t>> spirvSources = {
			{ GL_VERTEX_SHADER,   vertexSpirv   },
			{ GL_GEOMETRY_SHADER, geometrySpirv },
			{ GL_FRAGMENT_SHADER, fragmentSpirv }
		};

		m_RendererID = IsLegacyCompiler()
			? 0
			: CompileVulkanSpirv(spirvSources);

		for (auto&& [stage, binary] : m_OpenGLSpirv)
			Reflect(Utils::GetShaderStageFromOpenGLType(stage), binary);

		// Set uniform locations for samplers automatically (parity with other APIs)
		for (auto& samplerArray : GetReflection().SamplerArrays)
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

	void OpenGLShader::CompileOrReadFromCache(const std::unordered_map<GLenum, std::string_view>& shaderSources)
	{
		std::unordered_map<GLenum, std::string> parsedGlslSources;

		// Get shaders from cache
		for (auto&& [stage, shaderSource] : shaderSources)
		{
			std::filesystem::path filename = CacheDirectory / Utils::GetCacheExtension(m_Name, stage);
			if (std::filesystem::exists(filename))
			{
				m_OpenGLSpirv[stage] = Filesystem::ReadSpirvFile(filename.string());
			}
			else
			{
				parsedGlslSources[stage] = Parse(shaderSource);
			}
		}

		if (parsedGlslSources.empty())
		{
			// Create program from cache
			m_RendererID = CreateProgram(m_OpenGLSpirv);
		}
		else
		{
			// Compile not-found shaders
			m_RendererID = IsLegacyCompiler()
				? CompileVulkanSourcesLegacy(parsedGlslSources)
				: CompileVulkanSources(parsedGlslSources);
		}

		// Add new compiled sources to cache
		for (auto&& [stage, source] : parsedGlslSources)
		{
			auto it = m_OpenGLSpirv.find(stage);
			if (it != m_OpenGLSpirv.end())
			{
				std::filesystem::path filename = CacheDirectory / Utils::GetCacheExtension(m_Name, stage);
				std::ofstream out(filename, std::ios::out | std::ios::binary);

				auto&& [stage, binary] = *it;
				out.write((const char*)binary.data(), binary.size() * sizeof(uint32_t));
				out.close();
			}
		}
	}

	GLuint OpenGLShader::CompileVulkanSources(const std::unordered_map<GLenum, std::string>& shaderSources, uint32_t glslVersion)
	{
		static constexpr bool optimize = false;

		shaderc::Compiler vulkanCompiler;
		shaderc::CompileOptions vulkanOptions;

		vulkanOptions.SetSourceLanguage(shaderc_source_language_glsl);
		vulkanOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		if constexpr (optimize)
			vulkanOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		shaderc::Compiler openglCompiler;
		shaderc::CompileOptions openglOptions;
		openglOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		openglOptions.SetAutoMapLocations(true);
		openglOptions.SetAutoBindUniforms(true);

		if constexpr (optimize)
			openglOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		for (auto&& [stage, vulkanGlslSource] : shaderSources)
		{
			// Compile Vulkan GLSL sources to SPIR-V
			auto result = vulkanCompiler.CompileGlslToSpv(vulkanGlslSource, Utils::GLShaderStageToShaderC(stage), GetName().data(), "main", vulkanOptions);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(result.GetErrorMessage());
				break;
			}

			// Recompile SPIR-V to OpenGL GLSL sources
			spirv_cross::CompilerGLSL glslCompiler(result.cbegin(), result.cend() - result.cbegin());
			spirv_cross::CompilerGLSL::Options glslOptions;
			glslOptions.version = glslVersion;
			glslOptions.emit_push_constant_as_uniform_buffer = true;
			glslCompiler.set_common_options(glslOptions);

			std::string openglGlslSource = glslCompiler.compile();

			// Compile GLSL sources back into OpenGL compatible SPIR-V
			auto openglResult = openglCompiler.CompileGlslToSpv(openglGlslSource, Utils::GLShaderStageToShaderC(stage), GetName().data(), "main", openglOptions);

			if (openglResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(openglResult.GetErrorMessage());
				break;
			}

			m_OpenGLSpirv[stage] = { openglResult.cbegin(), openglResult.cend() };
		}

		bool isCompiled = m_OpenGLSpirv.size() == shaderSources.size();

		if (!isCompiled)
			return 0;

		return CreateProgram(m_OpenGLSpirv);
	}

	// TODO: deprecate
	GLuint OpenGLShader::CompileVulkanSourcesLegacy(const std::unordered_map<GLenum, std::string>& shaderSources, uint32_t glslVersion)
	{
		static constexpr bool optimize = false;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetSourceLanguage(shaderc_source_language_glsl);
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		if constexpr (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::unordered_map<GLenum, std::string> glslShaderSources;

		for (auto&& [stage, vulkanGlslSource] : shaderSources)
		{
			// Compile Vulkan GLSL sources to SPIR-V
			auto result = compiler.CompileGlslToSpv(vulkanGlslSource, Utils::GLShaderStageToShaderC(stage), GetName().data(), "main", options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(result.GetErrorMessage());
				break;
			}

			// Recompile SPIR-V to OpenGL GLSL sources
			spirv_cross::CompilerGLSL glslCompiler(result.cbegin(), result.cend() - result.cbegin());
			spirv_cross::CompilerGLSL::Options glslOptions;
			glslOptions.version = glslVersion;
			glslOptions.emit_push_constant_as_uniform_buffer = true;
			glslCompiler.set_common_options(glslOptions);

			glslShaderSources[stage] = glslCompiler.compile();
		}

		return CreateProgram(glslShaderSources);
	}

	GLuint OpenGLShader::CompileVulkanSpirv(const std::unordered_map<GLenum, std::span<const uint32_t>>& spirvBinaries, uint32_t glslVersion)
	{
		static constexpr bool optimize = false;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetSourceLanguage(shaderc_source_language_glsl);
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

		if constexpr (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto&& [stage, vulkanBinary] : spirvBinaries)
		{
			spirv_cross::CompilerGLSL glslCompiler(vulkanBinary.data(), vulkanBinary.size());
			spirv_cross::CompilerGLSL::Options glslOptions;
			glslOptions.version = glslVersion;
			glslOptions.emit_push_constant_as_uniform_buffer = true;
			glslCompiler.set_common_options(glslOptions);

			std::string source = glslCompiler.compile();
			auto result = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), GetName().data(), "main", options);

			size_t warnings = result.GetNumWarnings();
			QK_WARN("{0} warnings generated", warnings);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				QK_CORE_ERROR(result.GetErrorMessage());
				break;
			}

			m_OpenGLSpirv[stage] = { result.cbegin(), result.cend() };
		}

		bool isCompiled = m_OpenGLSpirv.size() == spirvBinaries.size();

		if (!isCompiled)
			return 0;

		return CreateProgram(m_OpenGLSpirv);
	}

	GLuint OpenGLShader::CreateProgram(const std::unordered_map<GLenum, std::vector<uint32_t>>& spirvBinaries)
	{
		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;

		for (auto&& [stage, binary] : spirvBinaries)
		{
			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V,
				(const void*)binary.data(),
				(GLsizei)(binary.size() * sizeof(uint32_t)));

			glSpecializeShader(shader, "main", 0, NULL, NULL);
			glAttachShader(program, shader);
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

	GLuint OpenGLShader::CreateProgram(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		GLenum glShaderIDs[s_MaxShaders]{};
		uint32_t glShaderIDIndex = 0;

		for (auto&& [stage, source] : shaderSources)
		{
			GLuint shader = glCreateShader(stage);
			glShaderIDs[glShaderIDIndex++] = shader;

			const GLchar* sourceData = source.c_str();
			const GLint length = static_cast<GLint>(source.size());

			glShaderSource(shader, 1, &sourceData, &length);
			glCompileShader(shader);

			GLint isCompiled;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

				QK_CORE_ERROR("Shader compilation failure (compiling: {0}):\n{1}", GetName(), (const GLchar*)infoLog);
			}
			else
			{
				glAttachShader(program, shader);
			}
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

		if (!isLinked)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			AutoRelease<GLchar> infoLog = StackAlloc(maxLength * sizeof(GLchar));
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

			QK_CORE_ERROR("Shader link failure (linking {0}):\n{1}", GetName(), (const GLchar*)infoLog);
		}

		return isLinked;
	}

	GLint OpenGLShader::GetUniformLocation(std::string_view name) const
	{
		size_t hash = std::hash<std::string_view>()(name);
		auto it = m_UniformLocationCache.find(hash);
		if (it != m_UniformLocationCache.end())
			return it->second;

		// Insert uniform location into cache
		const GLchar* nameCstr = name.data();
		GLint location = glGetUniformLocation(m_RendererID, nameCstr);
		if (location == -1)
		{
			QK_CORE_WARN("glGetUniformLocation returned an invalid location");
			return -1;
		}

		m_UniformLocationCache[hash] = location;
		return location;
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

	bool OpenGLShader::IsLegacyCompiler()
	{
		// Fallback to legacy compiler if this function is missing
		return !glSpecializeShader;
	}

	uint32_t OpenGLShader::GetGLSLVersion()
	{
		return GLVersion.major * 100 + GLVersion.minor * 10;
	}
}
