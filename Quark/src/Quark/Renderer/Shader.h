#pragma once

#include "Quark/Core/Core.h"
#include <unordered_map>

namespace Quark {

	class Shader
	{
	public:
		using SPIRVBinary = std::string_view;

		virtual ~Shader() = default;

		QK_DEPRECATED virtual void Attach() const = 0;
		QK_DEPRECATED virtual void Detach() const = 0;

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

		virtual std::string_view GetName() const = 0;

		virtual bool operator==(const Shader& other) const = 0;

		static Scope<Shader> Create(std::string_view filepath);
		static Scope<Shader> Create(std::string_view name, SPIRVBinary vertexSource, SPIRVBinary fragmentSource);
		static Scope<Shader> Create(std::string_view name, SPIRVBinary vertexSource, SPIRVBinary geometrySource, SPIRVBinary fragmentSource);
	};

	class ShaderLibrary
	{
	public:
		void Add(std::string_view name, Ref<Shader> shader);
		void Add(Ref<Shader> shader);
		bool Exists(std::string_view name) const;

		Ref<Shader> Load(std::string_view filepath);
		Ref<Shader> Load(std::string_view name, std::string_view filepath);
		Ref<Shader> Get(std::string_view name) const;

		size_t Size() const { return m_Shaders.size(); }

	private:
		std::unordered_map<size_t, Ref<Shader>> m_Shaders;
	};
}
