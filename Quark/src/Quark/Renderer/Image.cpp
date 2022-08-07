#include "qkpch.h"
#include "Image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <lodepng.h>

#include <filesystem>

namespace Quark {

	//                                           Number of bits per pixel ---v
	static constexpr ColorDataFormat GetDataFormat(uint8_t channels, uint8_t bpp, bool srgb, bool fp)
	{
		switch (channels)
		{
			case 1:
			switch (bpp)
			{
				case 8: return ColorDataFormat::Red8;
			}
			break;

			case 3:
			switch (bpp)
			{
				case 24: return srgb ? ColorDataFormat::RGB8_SRGB : ColorDataFormat::RGB8;
				case 48: return fp ? ColorDataFormat::RGB16f : ColorDataFormat::RGB16;
				case 96: return fp ? ColorDataFormat::RGB32f : ColorDataFormat::RGB32;
			}
			break;

			case 4:
			switch (bpp)
			{
				case 32: return srgb ? ColorDataFormat::RGBA8_SRGB : ColorDataFormat::RGBA8;
				case 64: return fp ? ColorDataFormat::RGBA16f : ColorDataFormat::RGBA16;
			}
			break;
		}

		QK_CORE_ASSERT(false, "Unknown texture internal format");
		return ColorDataFormat::None;
	}

	Image::Image(std::string_view filepath)
	{
		QK_CORE_ASSERT(std::filesystem::exists(filepath), "No file exists at filepath: '{0}'", filepath);

		if (stbi_is_hdr(filepath.data()))
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
		return (size_t)m_Metadata.Width * m_Metadata.Height * (m_Metadata.BitsPerPixel >> 3);
	}

	Ref<Image> Image::Shared(std::string_view filepath)
	{
		return CreateRef<Image>(filepath);
	}

	void Image::DecodePNG(std::string_view filepath)
	{	
		QK_PROFILE_FUNCTION();

		LodePNGState state;
		lodepng_state_init(&state);

		size_t filesize;
		unsigned char* filedata;
		unsigned int error = lodepng_load_file(&filedata, &filesize, filepath.data());
		QK_CORE_ASSERT(!error, "Failed to load image at path: '{0}'", filepath);

		unsigned char* imageData;
		unsigned width, height;
		error = lodepng_decode(&imageData, &width, &height, &state, filedata, filesize);
		QK_CORE_ASSERT(!error, "Failed to decode image at path: '{0}', {1}", filepath, lodepng_error_text(error));

		auto bpp = lodepng_get_bpp(&state.info_raw);
		auto channels = lodepng_get_channels(&state.info_raw);

		m_ImageData = imageData;
		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = channels;
		m_Metadata.BitsPerPixel = bpp;
		m_Metadata.DataFormat = GetDataFormat(channels, bpp, state.info_png.srgb_defined, false);

		free(filedata);
		lodepng_state_cleanup(&state);
	}

	void Image::DecodeHDR(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		FILE* file = fopen(filepath.data(), "rb");
		QK_CORE_ASSERT(file, "Could not open file at: {0}", filepath);

		int width, height, channels;
		m_ImageData = stbi_loadf_from_file(file, &width, &height, &channels, 0);
		QK_CORE_ASSERT(m_ImageData, "Failed to load image at path: {0}", filepath);

		uint8_t bpc;
		stbi_is_16_bit_from_file(file) ? bpc = 16 : bpc = 32;

		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = channels;
		m_Metadata.BitsPerPixel = bpc * channels;
		m_Metadata.DataFormat = GetDataFormat(channels, bpc * channels, false, true);

		fclose(file);
	}
}
