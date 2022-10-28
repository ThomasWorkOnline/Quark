#include "qkpch.h"
#include "VulkanFont.h"
#include "VulkanUtils.h"

#include "Quark/Renderer/Renderer.h"

#pragma warning(push, 0)
#	include <ft2build.h>
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
#pragma warning(pop)

namespace Quark {

	VulkanFont::VulkanFont(VulkanDevice* device, std::string_view filepath, uint32_t fontSize)
		: Font(filepath, fontSize), m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_AtlasWidth <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_AtlasHeight <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Font atlas dimensions too large: see Renderer::GetCapabilities() for more info");

		VkExtent3D atlasExtent{};
		atlasExtent.width = m_AtlasWidth;
		atlasExtent.height = m_AtlasHeight;
		atlasExtent.depth = 1;

		Utils::AllocateImage(m_Device, atlasExtent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8_UNORM,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Image, &m_BufferMemory);

		// Image view allocation
		{
			VkComponentMapping componentsSwizzle{};
			componentsSwizzle.r = VK_COMPONENT_SWIZZLE_ONE;
			componentsSwizzle.g = VK_COMPONENT_SWIZZLE_ONE;
			componentsSwizzle.b = VK_COMPONENT_SWIZZLE_ONE;
			componentsSwizzle.a = VK_COMPONENT_SWIZZLE_R;

			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_Image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = VK_FORMAT_R8_UNORM;
			info.components = componentsSwizzle;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;

			vkCreateImageView(m_Device->GetVkHandle(), &info, nullptr, &m_ImageView);
		}

		FT_GlyphSlot g = m_Face->glyph;
		for (uint8_t i = 0; i < GlyphCount; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != FT_Err_Ok &&
				g->bitmap.width == 0 || g->bitmap.rows == 0)
				continue;

			// Staging buffer
			VkDeviceSize size = g->bitmap.width * g->bitmap.rows * 1;
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			VkMemoryRequirements req = Utils::AllocateBuffer(m_Device, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				&stagingBuffer, &stagingBufferMemory
			);

			// Upload
			{
				void* mappedMemory;
				vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, req.size, 0, &mappedMemory);

				std::memcpy(mappedMemory, g->bitmap.buffer, size);

				VkMappedMemoryRange range{};
				range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				range.memory = stagingBufferMemory;
				range.size = req.size;

				vkFlushMappedMemoryRanges(m_Device->GetVkHandle(), 1, &range);
				vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
			}

			const Glyph& glyph = m_Glyphs[i];

			VkExtent3D glyphExtent{};
			glyphExtent.width = g->bitmap.width;
			glyphExtent.height = g->bitmap.rows;
			glyphExtent.depth = 1;

			VkOffset3D glyphOffset{};
			glyphOffset.x = glyph.OffsetX;

			Utils::CopyBufferToImage(m_Device, stagingBuffer, 0,
				m_Image, glyphExtent, glyphOffset, 1, 0, 1);

			vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
			vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
		}
	}

	VulkanFont::~VulkanFont()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
		vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	bool VulkanFont::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_ImageView == o->m_ImageView;

		return false;
	}
}
