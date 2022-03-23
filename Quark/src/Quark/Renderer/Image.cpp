#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Quark {

	Image::Image(std::string_view filepath, bool flipVertically)
	{
		stbi_set_flip_vertically_on_load(flipVertically);

		m_Data = stbi_load(filepath.data(), &m_Properties.Width, &m_Properties.Height, &m_Properties.Channels, 0);
		QK_CORE_ASSERT(m_Data, "Failed to load image at path: " << filepath);
	}

	Image::~Image()
	{
		stbi_image_free(m_Data);
	}

	size_t Image::Size() const
	{
		return (size_t)m_Properties.Width * m_Properties.Height * m_Properties.Channels;
	}
}
