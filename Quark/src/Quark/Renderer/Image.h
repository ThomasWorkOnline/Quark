#pragma once

#include "../Core/Core.h"

namespace Quark {

	typedef unsigned char ImageData;

	struct ImageProperties
	{
		int32_t Width, Height, Channels;
		std::string Filepath;
	};

	class Image
	{
	public:
		Image(const std::string& filepath, bool flipVertically = false);
		~Image();

		const ImageProperties& GetProperties() const { return m_Spec; }
		ImageData* Data() const { return m_Data; }
		size_t Size() const;
			
	private:
		ImageData* m_Data = nullptr;

		ImageProperties m_Spec;
	};
}
