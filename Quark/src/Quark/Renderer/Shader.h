#pragma once

#include "Quark/Core/Core.h"

#include <unordered_map>

namespace Quark {

	using SpirvSource = std::vector<uint32_t>;

	enum class ShaderStage
	{
		None = 0,
		VertexStage,
		GeometryStage,
		FragmentStage,
		ComputeStage
	};

	struct ResourceDecorators
	{
		uint32_t Set;
		uint32_t Binding;
	};

	struct UniformBufferResource
	{
		size_t      Size;
		ShaderStage Stage;
		ResourceDecorators Decorators;
	};

	struct SamplerArrayResource
	{
		uint32_t    SamplerCount;
		ShaderStage Stage;
		ResourceDecorators Decorators;
	};

	struct ShaderResources
	{
		std::vector<UniformBufferResource> UniformBuffers;
		std::vector<SamplerArrayResource>  SamplerArrays;
	};

	class Shader
	{
	public:
		Shader(std::string_view name);
		virtual ~Shader() = default;

		virtual void SetInt(std::string_view name, int32_t value) = 0;
		virtual void SetVec2i(std::string_view name, const Vec2i& value) = 0;
		virtual void SetVec3i(std::string_view name, const Vec3i& value) = 0;
		virtual void SetVec4i(std::string_view name, const Vec4i& value) = 0;
		virtual void SetIntArray(std::string_view name, const int32_t* values, uint32_t count) = 0;

		virtual void SetFloat(std::string_view name, float value) = 0;
		virtual void SetVec2f(std::string_view name, const Vec2f& value) = 0;
		virtual void SetVec3f(std::string_view name, const Vec3f& value) = 0;
		virtual void SetVec4f(std::string_view name, const Vec4f& value) = 0;
		virtual void SetFloatArray(std::string_view name, const float* values, uint32_t count) = 0;

		virtual void SetDouble(std::string_view name, double value) = 0;
		virtual void SetVec2d(std::string_view name, const Vec2d& value) = 0;
		virtual void SetVec3d(std::string_view name, const Vec3d& value) = 0;
		virtual void SetVec4d(std::string_view name, const Vec4d& value) = 0;
		virtual void SetDoubleArray(std::string_view name, const double* values, uint32_t count) = 0;

		virtual void SetMat3f(std::string_view name, const Mat3f& matrix) = 0;
		virtual void SetMat4f(std::string_view name, const Mat4f& matrix) = 0;

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) = 0;
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) = 0;

		virtual bool operator==(const Shader& other) const = 0;

		std::string_view GetName() const { return m_Name; }

		const ShaderResources& GetShaderResources() const { return m_ShaderResources; }
		uint32_t GetDescriptorCount() const { return m_DescriptorCount; }
		uint32_t GetBindingCount() const { return m_BindingCount; }

		static Scope<Shader> Create(std::string_view filepath);
		static Scope<Shader> Create(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource);
		static Scope<Shader> Create(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource);

	protected:
		void Reflect(ShaderStage stage, const SpirvSource& spirvSource);

	protected:
		std::string m_Name;
		ShaderResources m_ShaderResources;
		uint32_t m_DescriptorCount = 0;
		uint32_t m_BindingCount = 0;
	};

	SpirvSource ReadSpirvFile(std::string_view filepath);

	class ShaderLibrary
	{
	public:
		void Add(std::string_view name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		bool Exists(std::string_view name) const;

		Ref<Shader> Load(std::string_view filepath);
		Ref<Shader> Load(std::string_view name, std::string_view filepath);
		Ref<Shader> Get(std::string_view name) const;

		size_t Size() const { return m_Shaders.size(); }

	private:
		std::unordered_map<size_t, Ref<Shader>> m_Shaders;
	};
}
