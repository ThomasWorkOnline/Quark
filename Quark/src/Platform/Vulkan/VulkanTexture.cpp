#include "qkpch.h"
#include "VulkanTexture.h"

#include "Quark/Renderer/Image.h"
#include "Quark/Renderer/Renderer.h"

#include "VulkanEnums.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec)
		: Texture2D(spec)
		, m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Height <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Levels = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : 1;

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);
		Utils::AllocateImage(m_Device, imageExtent, 1, m_Spec.Levels, SampleCountToVulkan(m_Spec.Samples), format,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | (m_Spec.Levels > 1 ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Image, &m_BufferMemory);

		Utils::AllocateImageView(m_Device, m_Image, format, 1, m_Spec.Levels, &m_ImageView);
	}

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, std::string_view filepath)
		: m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		Image image = filepath;
		auto& metadata = image.GetMetadata();

		QK_CORE_ASSERT(metadata.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& metadata.Height <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Width      = metadata.Width;
		m_Spec.Height     = metadata.Height;
		m_Spec.DataFormat = metadata.DataFormat;
		m_Spec.Levels     = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : 1;

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);
		Utils::AllocateImage(m_Device, imageExtent, 1, m_Spec.Levels, SampleCountToVulkan(m_Spec.Samples), format,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | (m_Spec.Levels > 1 ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Image, &m_BufferMemory);

		Utils::AllocateImageView(m_Device, m_Image, format, 1, m_Spec.Levels, &m_ImageView);

		// Staging buffer
		VkDeviceSize size = metadata.Size;
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
			std::memcpy(mappedMemory, image.GetData(), size);

			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = stagingBufferMemory;
			range.size = req.size;

			vkFlushMappedMemoryRanges(m_Device->GetVkHandle(), 1, &range);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		VkOffset3D imageOffset{};
		Utils::CopyBufferToImage(m_Device, stagingBuffer, 0,
			m_Image, imageExtent, imageOffset, 1, 0, m_Spec.Levels);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);

		if (m_Spec.Levels > 1)
		{
			Utils::TransitionImageLayout(m_Device, m_Image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Spec.Levels);
			Utils::GenerateMipmaps(m_Device, m_Image, m_Spec.Width, m_Spec.Height, m_Spec.Levels);
		}
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
		vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	void VulkanTexture2D::SetData(const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		// Staging buffer
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
			std::memcpy(mappedMemory, data, size);

			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = stagingBufferMemory;
			range.size = req.size;

			vkFlushMappedMemoryRanges(m_Device->GetVkHandle(), 1, &range);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkOffset3D imageOffset{};
		Utils::CopyBufferToImage(m_Device, stagingBuffer, 0,
			m_Image, imageExtent, imageOffset, 1, 0, m_Spec.Levels);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	void VulkanTexture2D::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Levels > 1, "Invalid texture specification for mipmaps");

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);

		Utils::TransitionImageLayout(m_Device, m_Image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Spec.Levels);
		Utils::GenerateMipmaps(m_Device, m_Image, m_Spec.Width, m_Spec.Height, m_Spec.Levels);
	}

	bool VulkanTexture2D::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_Image == o->m_Image;

		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// VulkanTexture2DArray
	//

	VulkanTexture2DArray::VulkanTexture2DArray(VulkanDevice* device, const Texture2DArraySpecification& spec)
		: Texture2DArray(spec)
		, m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Height <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Layers <= Renderer::GetCapabilities().Texture.MaxArrayLayers,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Levels = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : 1;

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);
		Utils::AllocateImage(m_Device, imageExtent, m_Spec.Layers, m_Spec.Levels, SampleCountToVulkan(m_Spec.Samples), format,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Image, &m_BufferMemory);

		// Image view allocation
		{
			VkComponentMapping componentsSwizzle{};
			componentsSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			componentsSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			componentsSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			componentsSwizzle.a = VK_COMPONENT_SWIZZLE_A;

			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_Image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			info.format = format;
			info.components = componentsSwizzle;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = m_Spec.Levels;
			info.subresourceRange.layerCount = m_Spec.Layers;

			vkCreateImageView(m_Device->GetVkHandle(), &info, nullptr, &m_ImageView);
		}
	}

	void VulkanTexture2DArray::SetData(const void* data, size_t size, uint32_t layer)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		// Staging buffer
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
			std::memcpy(mappedMemory, data, size);

			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = stagingBufferMemory;
			range.size = req.size;

			vkFlushMappedMemoryRanges(m_Device->GetVkHandle(), 1, &range);
			vkUnmapMemory(m_Device->GetVkHandle(), stagingBufferMemory);
		}

		VkExtent3D imageExtent{};
		imageExtent.width = m_Spec.Width;
		imageExtent.height = m_Spec.Height;
		imageExtent.depth = 1;

		VkOffset3D imageOffset{};
		Utils::CopyBufferToImage(m_Device, stagingBuffer, 0,
			m_Image, imageExtent, imageOffset, 1, layer, m_Spec.Levels);

		vkDestroyBuffer(m_Device->GetVkHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), stagingBufferMemory, nullptr);
	}

	VulkanTexture2DArray::~VulkanTexture2DArray()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
		vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
		vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
	}

	bool VulkanTexture2DArray::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_Image == o->m_Image;

		return false;
	}
}
