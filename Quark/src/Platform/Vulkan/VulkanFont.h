#pragma once

#include "Quark/Renderer/Font.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanFont final : public Font
	{
	public:
		VulkanFont(VulkanDevice* device, std::string_view filepath, uint32_t fontSize);
		virtual ~VulkanFont() final override;

		virtual const void* GetHandle() const final override { return m_ImageView; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanFont(const VulkanFont&) = delete;
		VulkanFont& operator=(const VulkanFont&) = delete;

		VkImageView GetVkHandle() const { return m_ImageView; }

	private:
		VulkanDevice* m_Device;
		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_BufferMemory = nullptr;
	};
}
