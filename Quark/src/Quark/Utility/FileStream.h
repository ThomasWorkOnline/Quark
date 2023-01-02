#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class FileStream
	{
	public:
		FileStream(std::string_view filepath, const char* pOpenMode = "rb");
		FileStream(const FileStream&) = delete;
		FileStream(FileStream&& other) noexcept;
		~FileStream();

		size_t GetFileSize() const;
		FILE*  GetHandle() const { return m_Handle; }

		operator FILE* () const { return GetHandle(); }

	private:
		FILE* m_Handle;
	};
}
