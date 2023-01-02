#pragma once

#include "Quark/Core/Core.h"

#include "Formats.h"

#include <cstdio>

namespace Quark {

	struct ImageSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t BitsPerPixel = 0;
		uint32_t Channels = 0;

		ColorFormat DataFormat{};
	};

	class Image
	{
	public:
		Image(std::string_view filepath);
		Image(Image&& other) noexcept;
		~Image() noexcept;

		Image& operator=(Image&& other) noexcept;

		void* GetData() const { return m_ImageData; }

		size_t GetSize() const;

		uint32_t Width() const { return m_Spec.Width; }
		uint32_t Height() const { return m_Spec.Height; }
		uint32_t Channels() const { return m_Spec.Channels; }

		const ImageSpecification& GetSpecification() const { return m_Spec; }

		// Non-Copyable
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

	private:
		void DecodePNG(FILE* in);
		void DecodeHDR(FILE* in);

	private:
		ImageSpecification m_Spec{};
		void* m_ImageData = nullptr;
	};
}
