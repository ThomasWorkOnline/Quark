#include "qkpch.h"
#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <lodepng.h>

namespace Quark {

	Image::Image(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		m_Metadata.HDR = filepath.rfind(".hdr") != std::string::npos;
		if (m_Metadata.HDR)
			DecodeHDR(filepath);
		else
			DecodePNG(filepath);
	}

	Image::~Image()
	{
		QK_PROFILE_FUNCTION();

		// stbi_free calls free under the hood
		free(m_ImageData);
	}

	size_t Image::Size() const
	{
		return (size_t)m_Metadata.Width * m_Metadata.Height * m_Metadata.BPP;
	}

	Ref<Image> Image::Create(std::string_view filepath)
	{
		return CreateRef<Image>(filepath);
	}

	void Image::DecodePNG(std::string_view filepath)
	{
		LodePNGState state;
		lodepng_state_init(&state);

		size_t filesize;
		unsigned char* filedata = nullptr;
		unsigned int error = lodepng_load_file(&filedata, &filesize, filepath.data());
		QK_CORE_ASSERT(!error, "Failed to load image at path: '{0}'", filepath);

		unsigned char* imageData;
		unsigned width, height;
		error = lodepng_decode(&imageData, &width, &height, &state, filedata, filesize);
		QK_CORE_ASSERT(!error, "Failed to decode image at path: '{0}', {1}", filepath, lodepng_error_text(error));

		free(filedata);

		m_ImageData = imageData;
		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = lodepng_get_channels(&state.info_raw);
		m_Metadata.BPP = lodepng_get_bpp(&state.info_raw) >> 3;

		lodepng_state_cleanup(&state);
	}

	void Image::DecodeHDR(std::string_view filepath)
	{
		FILE* file = fopen(filepath.data(), "rb");
		QK_CORE_ASSERT(file, "Could not open file at: {0}", filepath);

		int width, height, channels;
		m_ImageData = stbi_loadf_from_file(file, &width, &height, &channels, 0);

		uint32_t bpc;
		stbi_is_16_bit_from_file(file) ? bpc = 4 : bpc = 1;

		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = channels;
		m_Metadata.BPP = channels * bpc;

		fclose(file);
		QK_CORE_ASSERT(m_ImageData, "Failed to load image at path: {0}", filepath);
	}
}
