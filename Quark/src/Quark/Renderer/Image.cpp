#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Quark {

	Image::Image(std::string_view filepath, const ImageDescriptor& descriptor)
	{
		stbi_set_flip_vertically_on_load(descriptor.FlipVertically);

		m_Data = stbi_load(filepath.data(), &m_Width, &m_Height, &m_Channels, 0);
		QK_CORE_ASSERT(m_Data, "Failed to load image at path: {0}", filepath);
	}

	Image::~Image()
	{
		stbi_image_free(m_Data);
	}

	size_t Image::Size() const
	{
		return (size_t)m_Width * m_Height * m_Channels;
	}

	HDRImage::HDRImage(std::string_view filepath, const ImageDescriptor& descriptor)
	{
		stbi_set_flip_vertically_on_load(descriptor.FlipVertically);

		m_Data = stbi_loadf(filepath.data(), &m_Width, &m_Height, &m_Channels, 0);
		QK_CORE_ASSERT(m_Data, "Failed to load image at path: {0}", filepath);
	}

	HDRImage::~HDRImage()
	{
		stbi_image_free(m_Data);
	}

	size_t HDRImage::Size() const
	{
		return (size_t)m_Width * m_Height * m_Channels;
	}
}
