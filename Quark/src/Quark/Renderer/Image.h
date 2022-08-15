#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Utility/FileStream.h"

#include "ColorFormats.h"

namespace Quark {

	struct ImageMetadata
	{
		uint32_t Width, Height;
		uint8_t  Channels;
		uint8_t  BitsPerPixel;

		ColorDataFormat DataFormat{};
	};

	class Image
	{
	public:
		Image(std::string_view filepath);
		~Image();

		// Non-Copyable
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		// Non-Movable
		Image(Image&&) = delete;
		Image& operator=(Image&&) = delete;

		uint32_t Width() const { return m_Metadata.Width; }
		uint32_t Height() const { return m_Metadata.Height; }
		uint8_t Channels() const { return m_Metadata.Channels; }

		size_t Size() const;
		const ImageMetadata& GetMetadata() const { return m_Metadata; }

		void* Data() const { return m_ImageData; }
		void* operator*() const { return m_ImageData; }

		static Ref<Image> Shared(std::string_view filepath);
		static Scope<Image> Create(std::string_view filepath);

	private:
		void DecodePNG(FileStream& in);
		void DecodeHDR(FileStream& in);

	private:
		ImageMetadata m_Metadata{};

		void* m_ImageData = nullptr;
		void (*DeallocateImageDataCallback)(void*);
	};
}
