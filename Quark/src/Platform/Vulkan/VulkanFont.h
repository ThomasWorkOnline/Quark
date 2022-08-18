#pragma once

#include "Quark/Renderer/Font.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanFont final : public Font
	{
	public:
		VulkanFont(VulkanDevice* device, std::string_view filepath, uint32_t fontSize);
		virtual ~VulkanFont() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual const Glyph& GetGlyph(uint8_t charcode) const override { return m_NullGlyph; }
		virtual uint32_t GetGlyphCount() const override { return 0; }

		virtual uint32_t GetFontSize() const override { return 0; }
		virtual uint32_t GetAtlasWidth() const override { return 0; }
		virtual uint32_t GetAtlasHeight() const override { return 0; }

		virtual std::string_view GetStyleName() const override { return std::string_view(); }
		virtual std::string_view GetFamilyName() const override { return std::string_view(); }

		virtual bool operator==(const Font& other) const override
		{
			return false;
		}

		// Non-Copyable
		VulkanFont(const VulkanFont&) = delete;
		VulkanFont& operator=(const VulkanFont&) = delete;

	private:
		VulkanDevice* m_Device;
		Glyph m_NullGlyph{};
	};
}
