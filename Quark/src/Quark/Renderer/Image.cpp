#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendor/stb_image/stb_image.h"

namespace Quark {

	Image::Image(const std::string& filepath, bool flipVertically)
	{
		stbi_set_flip_vertically_on_load(flipVertically);

		m_Properties.Filepath = filepath;
		m_Data = stbi_load(filepath.c_str(), &m_Properties.Width, &m_Properties.Height, &m_Properties.Channels, 0);
		QK_ASSERT(m_Data, "Failed to load image at path: " << filepath);
	}

	Image::~Image()
	{
		stbi_image_free(m_Data);
	}

	size_t Image::Size() const
	{
		return m_Properties.Width * m_Properties.Height * m_Properties.Channels;
	}
}
