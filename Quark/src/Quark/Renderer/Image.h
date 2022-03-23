#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	typedef unsigned char ImageData;

	struct ImageProperties
	{
		int32_t Width, Height, Channels;
		bool SRGB = true;
	};

	class Image
	{
	public:
		Image(std::string_view filepath, bool flipVertically = false);
		~Image();

		const ImageProperties& GetProperties() const { return m_Properties; }
		ImageData* Data() const { return m_Data; }
		size_t Size() const;
			
	private:
		ImageData* m_Data = nullptr;
		ImageProperties m_Properties{};
	};
}
