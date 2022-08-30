#include "qkpch.h"
#include "Image.h"

#include <fpng/fpng.h>
#include <fpng/fpng.cpp>
#include <lodepng/lodepng.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Quark {

	namespace Utils {

		static bool IsImageHDR(const FileStream& in)
		{
			return stbi_is_hdr_from_file(in.GetHandle());
		}

		//                                           Number of bits per pixel ---v
		static constexpr ColorDataFormat GetDataFormat(uint8_t channels, uint8_t bpp, bool srgb, bool fp)
		{
			switch (channels)
			{
				case 1:
				switch (bpp)
				{
					case 8: return ColorDataFormat::Red8;

				} break;

				case 3:
				switch (bpp)
				{
					case 24: return srgb ? ColorDataFormat::RGB8_SRGB : ColorDataFormat::RGB8;
					case 48: return fp ? ColorDataFormat::RGB16f : ColorDataFormat::RGB16;
					case 96: return fp ? ColorDataFormat::RGB32f : ColorDataFormat::RGB32;

				} break;

				case 4:
				switch (bpp)
				{
					case 32: return srgb ? ColorDataFormat::RGBA8_SRGB : ColorDataFormat::RGBA8;
					case 64: return fp ? ColorDataFormat::RGBA16f : ColorDataFormat::RGBA16;

				} break;

				QK_ASSERT_NO_DEFAULT("Unknown texture internal format");
			}

			return ColorDataFormat::None;
		}
	}

	Image::Image(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		FileStream in = filepath;

		if (Utils::IsImageHDR(in))
		{
			DecodeHDR(in);
		}
		else
		{
			DecodePNG(in);
		}
	}

	Image::~Image()
	{
		QK_PROFILE_FUNCTION();

		free(m_ImageData);
	}

	void Image::DecodePNG(FileStream& in)
	{	
		QK_PROFILE_FUNCTION();

		LodePNGState state;
		lodepng_state_init(&state);

		unsigned char* imageData;
		unsigned width, height;
		{
			size_t fileSize = in.GetFileSize();

			std::vector<uint8_t> fileData;
			fileData.reserve(fileSize);

			size_t readSize = std::fread(fileData.data(), 1, fileSize, in.GetHandle());
			QK_RUNTIME_VERIFY(readSize == fileSize, "Expected size and read size do not match");
			
			unsigned int error = lodepng_decode(&imageData, &width, &height, &state, fileData.data(), fileSize);

			if (error)
			{
				lodepng_state_cleanup(&state);
				QK_RUNTIME_ERROR("Failed to decode image: {0}", lodepng_error_text(error));
			}
		}

		auto bpp = lodepng_get_bpp(&state.info_raw);
		auto channels = lodepng_get_channels(&state.info_raw);

		m_ImageData = imageData;
		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = channels;
		m_Metadata.BitsPerPixel = bpp;
		m_Metadata.DataFormat = Utils::GetDataFormat(channels, bpp, state.info_png.srgb_defined, false);
		m_Metadata.Size = (size_t)width * height * (bpp >> 3);

		lodepng_state_cleanup(&state);
	}

	void Image::DecodeHDR(FileStream& in)
	{
		QK_PROFILE_FUNCTION();

		int width, height, channels;
		float* imageData = stbi_loadf_from_file(in.GetHandle(), &width, &height, &channels, 0);

		QK_RUNTIME_VERIFY(imageData, "Failed to load image data");

		uint8_t bitsPerChannel;
		stbi_is_16_bit_from_file(in.GetHandle())
			? bitsPerChannel = 16
			: bitsPerChannel = 32;

		uint8_t bpp = bitsPerChannel * channels;

		m_ImageData = imageData;
		m_Metadata.Width = width;
		m_Metadata.Height = height;
		m_Metadata.Channels = channels;
		m_Metadata.BitsPerPixel = bpp;
		m_Metadata.DataFormat = Utils::GetDataFormat(channels, bpp, false, true);
		m_Metadata.Size = (size_t)width * height * (bpp >> 3);
	}
}
