#pragma once

#include "Quark/Core/Core.h"
#include "TextureFormats.h"

namespace Quark {

	struct ImageMetadata
	{
		uint32_t Width    = 0, Height = 0;
		uint32_t Channels = 0;
		uint32_t BPP      = 0; // Number of bytes per pixel

		bool HDR = false;
	};

	class Image
	{
	public:
		Image(std::string_view filepath);
		~Image();

		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		int32_t Width() const { return m_Metadata.Width; }
		int32_t Height() const { return m_Metadata.Height; }
		int32_t Channels() const { return m_Metadata.Channels; }

		void* Data() const { return m_ImageData; }
		void* operator*() const { return m_ImageData; }

		size_t Size() const;
		bool IsHDR() const { return m_Metadata.HDR; }

		static Ref<Image> Create(std::string_view filepath);

	private:
		void DecodePNG(std::string_view filepath);
		void DecodeHDR(std::string_view filepath);

	private:
		ImageMetadata m_Metadata;
		void* m_ImageData = nullptr;
	};
}
