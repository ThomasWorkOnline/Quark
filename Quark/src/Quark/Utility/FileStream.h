#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class FileStream
	{
	public:
		FileStream(std::string_view filepath);
		~FileStream();

		size_t GetFileSize() const;
		FILE* GetHandle() const { return m_Handle; }

	private:
		FILE* m_Handle;
	};
}
