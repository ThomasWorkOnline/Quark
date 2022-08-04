#pragma once

#include "Quark/Renderer/Shader.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(VulkanDevice* device, std::string_view filepath);
		VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view fragmentSource);
		VulkanShader(VulkanDevice* device, std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource);
		virtual ~VulkanShader() override;

		virtual void Attach() const override {}
		virtual void Detach() const override {}

		virtual void SetInt(std::string_view name, int32_t value) override {}
		virtual void SetInt2(std::string_view name, const Vec2i& value) override {}
		virtual void SetInt3(std::string_view name, const Vec3i& value) override {}
		virtual void SetInt4(std::string_view name, const Vec4i& value) override {}
		virtual void SetIntArray(std::string_view name, const int32_t* values, uint32_t count) override {}

		virtual void SetFloat(std::string_view name, float value) override {}
		virtual void SetVec2f(std::string_view name, const Vec2f& value) override {}
		virtual void SetVec3f(std::string_view name, const Vec3f& value) override {}
		virtual void SetVec4f(std::string_view name, const Vec4f& value) override {}
		virtual void SetFloatArray(std::string_view name, const float* values, uint32_t count) override {}

		virtual void SetDouble(std::string_view name, double value) override {}
		virtual void SetVec2d(std::string_view name, const Vec2d& value) override {}
		virtual void SetVec3d(std::string_view name, const Vec3d& value) override {}
		virtual void SetVec4d(std::string_view name, const Vec4d& value) override {}
		virtual void SetDoubleArray(std::string_view name, const double* values, uint32_t count) override {}

		virtual void SetMat3f(std::string_view name, const Mat3f& matrix) override {}
		virtual void SetMat4f(std::string_view name, const Mat4f& matrix) override {}

		virtual void SetMat3d(std::string_view name, const Mat3d& matrix) override {}
		virtual void SetMat4d(std::string_view name, const Mat4d& matrix) override {}

		virtual std::string_view GetName() const override { return m_Name; };

		virtual bool operator==(const Shader& other) const override
		{
			return m_Name == reinterpret_cast<const VulkanShader&>(other).m_Name;
		}

		VkShaderModule GetVertexVkHandle() const { return m_VertexShaderModule; }
		VkShaderModule GetGeometryVkHandle() const { return m_VertexShaderModule; }
		VkShaderModule GetFragmentVkHandle() const { return m_FragmentShaderModule; }

	private:
		VulkanDevice* m_Device;

		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_GeometryShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;
		std::string m_Name;
	};
}
