#pragma once

#include "../Core/Core.h"

#include "Resource.h"

#include <unordered_map>

namespace Quark {

	class Shader : public Resource
	{
	public:
		virtual ~Shader() = default;

		virtual void Attach() const = 0;
		virtual void Detach() const = 0;

		virtual void SetInt(const std::string& name, int32_t value) = 0;
		virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, Ref<Shader> shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		const Ref<Shader>& Get(const std::string& name);

		bool Exists(const std::string& name) const;
		size_t Size() const { return m_Shaders.size(); }

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
