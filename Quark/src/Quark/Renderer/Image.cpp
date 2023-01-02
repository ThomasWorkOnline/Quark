#include "qkpch.h"
#include "Image.h"

#include <lodepng/lodepng.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdexcept>

namespace Quark {

	namespace Utils {

		static bool IsImageHDR(FILE* in)
		{
			return stbi_is_hdr_from_file(in);
		}

		//                                         Number of bits per pixel ---v
		static constexpr ColorFormat GetDataFormat(uint32_t channels, uint32_t bpp, bool srgb, bool fp)
		{
			switch (channels)
			{
				case 1:
				switch (bpp)
				{
					case 8: return ColorFormat::Red8;

				} break;

				case 3:
				switch (bpp)
				{
					case 24: return srgb ? ColorFormat::RGB8SRGB : ColorFormat::RGB8;
					case 48: return fp ? ColorFormat::RGB16f : ColorFormat::RGB16;
					case 96: return fp ? ColorFormat::RGB32f : ColorFormat::RGB32UInt;

				} break;

				case 4:
				switch (bpp)
				{
					case 32: return srgb ? ColorFormat::RGBA8SRGB : ColorFormat::RGBA8;
					case 64: return fp ? ColorFormat::RGBA16f : ColorFormat::RGBA16;

				} break;

				QK_ASSERT_NO_DEFAULT("Unknown texture internal format");
			}

			return ColorFormat::None;
		}
	}

	Image::Image(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		FILE* in = std::fopen(filepath.data(), "rb");
		Verify(in, "Could not open image: '{0}'", filepath);

		try
		{
			if (Utils::IsImageHDR(in))
			{
				DecodeHDR(in);
			}
			else
			{
				DecodePNG(in);
			}
		}
		catch (std::exception& e)
		{
			std::fclose(in);
			throw e;
		}

		std::fclose(in);
	}

	Image::Image(Image&& other) noexcept
		: m_Spec(other.m_Spec)
		, m_ImageData(other.m_ImageData)
	{
		other.m_Spec = {};
		other.m_ImageData = nullptr;
	}

	Image::~Image() noexcept
	{
		free(m_ImageData);
	}

	Image& Image::operator=(Image&& other) noexcept
	{
		std::swap(other, *this);
		return *this;
	}

	size_t Image::GetSize() const
	{
		return (size_t)m_Spec.Width * m_Spec.Height * (m_Spec.BitsPerPixel >> 3);
	}

	void Image::DecodePNG(FILE* in)
	{	
		LodePNGState state;
		lodepng_state_init(&state);

		unsigned char* imageData;
		unsigned int width, height;
		{
			std::fseek(in, 0, SEEK_END);
			size_t fileSize = std::ftell(in);
			std::fseek(in, 0, SEEK_SET);

			std::vector<uint8_t> fileData;
			fileData.resize(fileSize);
			size_t readSize = std::fread(fileData.data(), 1, fileSize, in);
			fileData.resize(readSize);

			Verify(readSize == fileSize, "Expected size and read size do not match");
			unsigned int error = lodepng_decode(&imageData, &width, &height, &state, fileData.data(), fileSize);

			if (error)
			{
				lodepng_state_cleanup(&state);
				ThrowRuntimeError("Failed to decode PNG image: {0}", lodepng_error_text(error));
			}
		}

		auto bpp = lodepng_get_bpp(&state.info_raw);
		auto channels = lodepng_get_channels(&state.info_raw);

		m_Spec.Width = width;
		m_Spec.Height = height;
		m_Spec.Channels = channels;
		m_Spec.BitsPerPixel = bpp;
		m_Spec.DataFormat = Utils::GetDataFormat(channels, bpp, state.info_png.srgb_defined, false);
		m_ImageData = imageData;

		lodepng_state_cleanup(&state);
	}

	void Image::DecodeHDR(FILE* in)
	{
		int width, height, channels;
		float* imageData = stbi_loadf_from_file(in, &width, &height, &channels, 0);

		Verify(imageData, "Failed to load HDR image data");

		uint32_t bitsPerChannel;
		stbi_is_16_bit_from_file(in)
			? bitsPerChannel = 16
			: bitsPerChannel = 32;

		uint32_t bpp = bitsPerChannel * channels;

		m_Spec.Width = width;
		m_Spec.Height = height;
		m_Spec.Channels = channels;
		m_Spec.BitsPerPixel = bpp;
		m_Spec.DataFormat = Utils::GetDataFormat(channels, bpp, false, true);
		m_ImageData = imageData;
	}
}
