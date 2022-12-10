#pragma once

#include "Quark/Renderer/Shader.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

#include <unordered_map>

namespace Quark {

	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(VulkanDevice* device, std::string_view filepath);
		VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view fragmentSource);
		VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
		VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> fragmentSpirv);
		VulkanShader(VulkanDevice* device, std::string_view name, std::span<const uint32_t> vertexSpirv, std::span<const uint32_t> geometrySpirv, std::span<const uint32_t> fragmentSpirv);

		virtual ~VulkanShader() final override;

		virtual void SetInt(std::string_view name, int32_t value) final override {}
		virtual void SetVec2i(std::string_view name, const Vec2i& value) final override {}
		virtual void SetVec3i(std::string_view name, const Vec3i& value) final override {}
		virtual void SetVec4i(std::string_view name, const Vec4i& value) final override {}
		virtual void SetIntArray(std::string_view name, const int32_t* values, uint32_t count) final override {}

		virtual void SetFloat(std::string_view name, float value) final override {}
		virtual void SetVec2f(std::string_view name, const Vec2f& value) final override {}
		virtual void SetVec3f(std::string_view name, const Vec3f& value) final override {}
		virtual void SetVec4f(std::string_view name, const Vec4f& value) final override {}
		virtual void SetFloatArray(std::string_view name, const float* values, uint32_t count) final override {}

		virtual void SetDouble(std::string_view name, double value) final override {}
		virtual void SetVec2d(std::string_view name, const Vec2d& value) final override {}
		virtual void SetVec3d(std::string_view name, const Vec3d& value) final override {}
		virtual void SetVec4d(std::string_view name, const Vec4d& value) final override {}
		virtual void SetDoubleArray(std::string_view name, const double* values, uint32_t count) final override {}

		virtual void SetMat3f(std::string_view name, const Mat3f& matrix) final override {}
		virtual void SetMat4f(std::string_view name, const Mat4f& matrix) final override {}

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) final override {}
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) final override {}

		virtual bool operator==(const Shader& other) const final override;

		// Non-Copyable
		VulkanShader(const VulkanShader&) = delete;
		VulkanShader& operator=(const VulkanShader&) = delete;

		const std::unordered_map<VkShaderStageFlagBits, VkShaderModule>& GetShaderStages() const { return m_ShaderStages; }

	private:
		VkShaderModule CreateShader(ShaderStage stage, std::span<const uint32_t> spirvSource);

	private:
		VulkanDevice* m_Device;
		std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderStages;
	};
}
