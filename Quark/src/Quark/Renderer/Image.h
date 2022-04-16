#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	struct ImageDescriptor
	{
		bool FlipVertically = true;
	};

	class Image
	{
	public:
		Image(std::string_view filepath, const ImageDescriptor& descriptor = {});
		~Image();

		int32_t Width() const { return m_Width; }
		int32_t Height() const { return m_Height; }
		int32_t Channels() const { return m_Channels; }

		void* Data() const { return m_Data; }
		size_t Size() const;

		void* operator*() const { return m_Data; }

	private:
		void* m_Data = nullptr;
		int32_t m_Width, m_Height, m_Channels;
	};

	class HDRImage
	{
	public:
		HDRImage(std::string_view filepath, const ImageDescriptor& descriptor = {});
		~HDRImage();

		int32_t Width() const { return m_Width; }
		int32_t Height() const { return m_Height; }
		int32_t Channels() const { return m_Channels; }

		void* Data() const { return m_Data; }
		size_t Size() const;

		void* operator*() const { return m_Data; }

	private:
		void* m_Data = nullptr;
		int32_t m_Width, m_Height, m_Channels;
	};
}
