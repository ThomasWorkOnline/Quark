#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"

namespace Quark {

	struct ImageMetadata
	{
		uint32_t Width    = 0, Height = 0;
		uint8_t  Channels = 0;
		uint8_t  BitsPerPixel = 0;

		ColorDataFormat DataFormat{};
	};

	class Image
	{
	public:
		Image(std::string_view filepath);
		~Image();

		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		uint32_t Width() const { return m_Metadata.Width; }
		uint32_t Height() const { return m_Metadata.Height; }
		uint8_t Channels() const { return m_Metadata.Channels; }

		const ImageMetadata& GetMetadata() const { return m_Metadata; }

		void* Data() const { return m_ImageData; }
		void* operator*() const { return m_ImageData; }

		size_t Size() const;

		static Ref<Image> Shared(std::string_view filepath);

	private:
		void DecodePNG(std::string_view filepath);
		void DecodeHDR(std::string_view filepath);

	private:
		ImageMetadata m_Metadata;
		void* m_ImageData = nullptr;
	};
}
