#include "qkpch.h"
#include "FileStream.h"

namespace Quark {

	FileStream::FileStream(std::string_view filepath, const char* pOpenMode)
		: m_Handle(std::fopen(filepath.data(), pOpenMode))
	{
		QK_CORE_VERIFY(m_Handle, "Could not open file at: {0}", filepath);
	}

	FileStream::FileStream(FileStream&& other) noexcept
		: m_Handle(other.m_Handle)
	{
		other.m_Handle = nullptr;
	}

	FileStream::~FileStream()
	{
		std::fclose(m_Handle);
	}

	size_t FileStream::GetFileSize() const
	{
		std::fseek(m_Handle, 0, SEEK_END);

		size_t size = std::ftell(m_Handle);
		QK_CORE_ASSERT(size != LONG_MAX, "Invalid file size");

		std::fseek(m_Handle, 0, SEEK_SET);
		return size;
	}
}
