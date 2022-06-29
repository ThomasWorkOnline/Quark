#pragma once

#include "Quark/Core/Core.h"

#include <unordered_map>

namespace Quark {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Attach() const = 0;
		virtual void Detach() const = 0;

		virtual void SetInt(std::string_view name, int32_t value) = 0;
		virtual void SetInt2(std::string_view name, const Vec2i& value) = 0;
		virtual void SetInt3(std::string_view name, const Vec3i& value) = 0;
		virtual void SetInt4(std::string_view name, const Vec4i& value) = 0;
		virtual void SetIntArray(std::string_view name, int32_t* values, uint32_t count) = 0;

		virtual void SetFloat(std::string_view name, float value) = 0;
		virtual void SetFloat2(std::string_view name, const Vec2f& value) = 0;
		virtual void SetFloat3(std::string_view name, const Vec3f& value) = 0;
		virtual void SetFloat4(std::string_view name, const Vec4f& value) = 0;
		virtual void SetFloatArray(std::string_view name, float* values, uint32_t count) = 0;

		virtual void SetDouble(std::string_view name, double value) = 0;
		virtual void SetDouble2(std::string_view name, const Vec2d& value) = 0;
		virtual void SetDouble3(std::string_view name, const Vec3d& value) = 0;
		virtual void SetDouble4(std::string_view name, const Vec4d& value) = 0;
		virtual void SetDoubleArray(std::string_view name, double* values, uint32_t count) = 0;

		virtual void SetMat3(std::string_view name, const Mat3f& matrix) = 0;
		virtual void SetMat4(std::string_view name, const Mat4f& matrix) = 0;

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) = 0;
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual std::string_view GetName() const = 0;

		virtual bool operator==(const Shader& other) const = 0;

		static Ref<Shader> Create(std::string_view filepath);
		static Ref<Shader> Create(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource);
		static Ref<Shader> Create(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
	};

	class ShaderLibrary
	{
	public:
		void Add(std::string_view name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);

		Ref<Shader> Load(std::string_view filepath);
		Ref<Shader> Load(std::string_view name, std::string_view filepath);

		const Ref<Shader>& Get(std::string_view name) const;

		bool Exists(std::string_view name) const;
		size_t Size() const { return m_Shaders.size(); }

	private:
		std::unordered_map<size_t, Ref<Shader>> m_Shaders;
	};
}
