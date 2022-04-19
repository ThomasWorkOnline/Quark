#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Quark {

	Image::Image(std::string_view filepath, const ImageDescriptor& descriptor)
	{
		QK_SCOPE_TIMER(Image::Image);

		stbi_set_flip_vertically_on_load_thread(descriptor.FlipVertically);

		FILE* file = fopen(filepath.data(), "rb");
		QK_CORE_ASSERT(file, "Could not open file at: {0}", filepath);

		stbi_is_16_bit_from_file(file) ? m_BPC = 4 : m_BPC = 1;
		bool hdr = stbi_is_hdr_from_file(file);

		if (hdr)
			m_ExtFormat = ImageExtensionFormat::HDR;
		else
			m_ExtFormat = ImageExtensionFormat::Int;

		switch (m_ExtFormat)
		{
			case ImageExtensionFormat::Int:
				m_Data = stbi_load_from_file(file, &m_Width, &m_Height, &m_Channels, 0);
				break;
			case ImageExtensionFormat::HDR:
				m_Data = stbi_loadf_from_file(file, &m_Width, &m_Height, &m_Channels, 0);
				break;
			default:
				QK_CORE_ASSERT(false, "Invalid image type");
				break;
		}

		fclose(file);
		QK_CORE_ASSERT(m_Data, "Failed to load image at path: {0}", filepath);
	}

	Image::~Image()
	{
		QK_SCOPE_TIMER(Image::~Image);

		stbi_image_free(m_Data);
	}

	size_t Image::Size() const
	{
		return (size_t)m_Width * m_Height * m_Channels * m_BPC;
	}

	bool Image::IsHDR() const
	{
		return m_ExtFormat == ImageExtensionFormat::HDR;
	}
}
