#pragma once

#include "Quark/Renderer/UniformBuffer.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanUniformBuffer final : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(size_t size, uint32_t binding);
		virtual ~VulkanUniformBuffer() override;

		virtual void Attach(uint32_t binding = 0) const override;
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		vk::Buffer GetVkHandle() const { return m_Buffer; }

		virtual bool operator==(const UniformBuffer& other) const override
		{
			return m_Buffer == reinterpret_cast<const VulkanUniformBuffer&>(other).m_Buffer;
		}

	private:
		vk::Buffer m_Buffer;
		vk::DeviceMemory m_BufferMemory;
	};
}
