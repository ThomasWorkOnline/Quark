#pragma once

#include "Quark/Renderer/Font.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanFont final : public Font
	{
	public:
		VulkanFont(VulkanDevice* device, std::string_view filepath, uint32_t fontSize);
		virtual ~VulkanFont() final override = default;

		virtual const Glyph& GetGlyph(uint8_t charcode) const final override { return m_NullGlyph; }
		virtual uint32_t GetGlyphCount() const final override { return 0; }

		virtual uint32_t GetFontSize() const final override { return 0; }
		virtual uint32_t GetAtlasWidth() const final override { return 0; }
		virtual uint32_t GetAtlasHeight() const final override { return 0; }

		virtual std::string_view GetStyleName() const final override { return std::string_view(); }
		virtual std::string_view GetFamilyName() const final override { return std::string_view(); }

		virtual const void* GetHandle() const final override { return nullptr; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanFont(const VulkanFont&) = delete;
		VulkanFont& operator=(const VulkanFont&) = delete;

		VkImageView GetVkHandle() const { return nullptr; }

	private:
		VulkanDevice* m_Device;
		Glyph m_NullGlyph{};
	};
}
