#pragma once

#include "Quark/Core/Core.h"
#include "TextureFormats.h"

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
		bool IsHDR() const { return m_HDR; }

	private:
		void* m_Data = nullptr;
		int32_t m_Width = 0, m_Height = 0, m_Channels = 0, m_BPC = 0;
		bool m_HDR = false;
	};
}
