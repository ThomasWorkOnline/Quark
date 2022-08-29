#include "qkpch.h"
#include "VulkanTexture.h"

#include "Quark/Renderer/Image.h"

#include "VulkanUtils.h"
#include "VulkanFormats.h"

namespace Quark {

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec)
		: Texture2D(spec)
		, m_Device(device)
	{
	}

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, std::string_view filepath, const TextureRenderModes& renderModes)
		: m_Device(device)
	{
		Image image = filepath;
		auto& metadata = image.GetMetadata();

		m_Spec.Width = metadata.Width;
		m_Spec.Height = metadata.Height;
		m_Spec.DataFormat = metadata.DataFormat;
		m_Spec.DataFormat = metadata.DataFormat;
		m_Spec.RenderModes = renderModes;

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);

		m_Image = Utils::AllocateImage(m_Device, imageExtent, format,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

		// Image view allocation
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_Image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = format;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;

			vkCreateImageView(m_Device->GetVkHandle(), &info, nullptr, &m_ImageView);
		}

		// Staging buffer
		size_t alignedSize;
		VkDeviceSize size = metadata.Size;

		VkDeviceMemory stagingBufferMemory;
		VkBuffer stagingBuffer = Utils::AllocateBuffer(m_Device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			stagingBufferMemory, &alignedSize
		);

		// Upload
		{
			void* mappedMemory;
			vkMapMemory(m_Device->GetVkHandle(), stagingBufferMemory, 0, size, 0, &mappedMemory);
			std::memcpy(mappedMemory, image.GetData(), size);

			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = stagingBufferMemory;
			range.size = alignedSize;

			vkFlushMappedMemoryRanges(m_Device->GetVkHandle(), 1, &range);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		Utils::CopyBufferToImage(m_Device, m_Image, stagingBuffer, imageExtent);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
		vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	bool VulkanTexture2D::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<const VulkanTexture2D*>(&other))
			return m_Image == o->m_Image;

		return false;
	}
}
