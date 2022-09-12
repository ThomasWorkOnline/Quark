#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Utility/FileStream.h"

#include "Formats.h"

namespace Quark {

	struct ImageMetadata
	{
		size_t   Size = 0;
		uint32_t Width = 0, Height = 0;
		uint32_t BitsPerPixel = 0;
		uint32_t Channels = 0;

		ColorDataFormat DataFormat{};
	};

	class Image
	{
	public:
		Image(std::string_view filepath);
		~Image();

		uint32_t Width() const { return m_Metadata.Width; }
		uint32_t Height() const { return m_Metadata.Height; }
		uint32_t Channels() const { return m_Metadata.Channels; }

		void* GetData() const { return m_ImageData; }
		void* operator*() const { return GetData(); }

		const ImageMetadata& GetMetadata() const { return m_Metadata; }

		// Non-Copyable
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

	private:
		void DecodePNG(FileStream& in);
		void DecodeHDR(FileStream& in);

	private:
		ImageMetadata m_Metadata{};
		void* m_ImageData = nullptr;
	};
}
