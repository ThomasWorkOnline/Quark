#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Quark {

	Image::Image(std::string_view filepath, const ImageDescriptor& descriptor)
	{
		QK_SCOPE_TIMER(Image::Image);

		stbi_set_flip_vertically_on_load(descriptor.FlipVertically);

		size_t extensionStart = filepath.find_last_of('.');
		std::string_view extension = filepath.substr(extensionStart);

		if (extension == ".hdr")
			m_ExtFormat = ImageExtensionFormat::HDR;
		else
			m_ExtFormat = ImageExtensionFormat::SRGB;

		switch (m_ExtFormat)
		{
			case ImageExtensionFormat::Linear:
			case ImageExtensionFormat::SRGB:
				m_Data = stbi_load(filepath.data(), &m_Width, &m_Height, &m_Channels, 0);
				break;
			case ImageExtensionFormat::HDR:
				m_Data = stbi_loadf(filepath.data(), &m_Width, &m_Height, &m_Channels, 0);
				break;
			default:
				QK_CORE_ASSERT(false, "Invalid image type");
				break;
		}
		
		QK_CORE_ASSERT(m_Data, "Failed to load image at path: {0}", filepath);
	}

	Image::~Image()
	{
		QK_SCOPE_TIMER(Image::~Image);

		stbi_image_free(m_Data);
	}

	size_t Image::Size() const
	{
		return (size_t)m_Width * m_Height * m_Channels;
	}

	bool Image::IsHDR() const
	{
		return m_ExtFormat == ImageExtensionFormat::HDR;
	}
}
